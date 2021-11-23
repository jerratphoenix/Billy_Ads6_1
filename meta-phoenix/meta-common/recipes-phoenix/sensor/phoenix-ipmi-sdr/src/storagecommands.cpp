/*
// Copyright (c) 2017-2019 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/
/*

#include "commandutils.hpp"
#include "ipmi_to_redfish_hooks.hpp"


*/
#include "types.hpp"
#include "storagecommands.hpp"
#include "sdrcommands.hpp"
#include "sdrutils.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/process.hpp>
#include <ipmid/api.hpp>
#include <ipmid/message.hpp>
#include <phosphor-ipmi-host/selutility.hpp>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/message/types.hpp>
#include <sdbusplus/timer.hpp>

#include <filesystem>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string_view>

static constexpr bool DEBUG = false;

namespace intel_oem::ipmi::sel
{
static const std::filesystem::path selLogDir = "/var/log";
static const std::string selLogFilename = "ipmi_sel";

static int getFileTimestamp(const std::filesystem::path& file)
{
    struct stat st;

    if (stat(file.c_str(), &st) >= 0) {
        return st.st_mtime;
    }
    return ::ipmi::sel::invalidTimeStamp;
}

namespace erase_time
{
static constexpr const char* selEraseTimestamp = "/var/lib/ipmi/sel_erase_time";

void save()
{
    // open the file, creating it if necessary
    int fd = open(selEraseTimestamp, O_WRONLY | O_CREAT | O_CLOEXEC, 0644);
    if (fd < 0) {
        std::cerr << "Failed to open file\n";
        return;
    }

    // update the file timestamp to the current time
    if (futimens(fd, NULL) < 0) {
        std::cerr << "Failed to update timestamp: "
                  << std::string(strerror(errno));
    }
    close(fd);
}

int get()
{
    return getFileTimestamp(selEraseTimestamp);
}
} // namespace erase_time
} // namespace intel_oem::ipmi::sel

namespace ipmi
{

namespace storage
{

constexpr static const size_t maxMessageSize = 64;
constexpr static const size_t maxFruSdrNameSize = 16;
using ObjectType = boost::container::flat_map<
                   std::string, boost::container::flat_map<std::string, DbusVariant>>;
using ManagedObjectType =
    boost::container::flat_map<sdbusplus::message::object_path, ObjectType>;
using ManagedEntry = std::pair<sdbusplus::message::object_path, ObjectType>;

constexpr static const char* fruDeviceServiceName =
    "xyz.openbmc_project.FruDevice";
constexpr static const char* entityManagerServiceName =
    "xyz.openbmc_project.EntityManager";
constexpr static const size_t writeTimeoutSeconds = 10;
constexpr static const char* chassisTypeRackMount = "23";

// event direction is bit[7] of eventType where 1b = Deassertion event
constexpr static const uint8_t deassertionEvent = 0x80;

static std::vector<uint8_t> fruCache;
static uint8_t cacheBus = 0xFF;
static uint8_t cacheAddr = 0XFF;
static uint8_t lastDevId = 0xFF;

static uint8_t writeBus = 0xFF;
static uint8_t writeAddr = 0XFF;

std::unique_ptr<phosphor::Timer> writeTimer = nullptr;
static std::vector<sdbusplus::bus::match::match> fruMatches;

ManagedObjectType frus;

// we unfortunately have to build a map of hashes in case there is a
// collision to verify our dev-id
boost::container::flat_map<uint8_t, std::pair<uint8_t, uint8_t>> deviceHashes;

void registerStorageFunctions() __attribute__((constructor));

static bool findSELEntry(const int recordID,
                         const std::vector<std::filesystem::path>& selLogFiles,
                         std::string& entry)
{
    // Record ID is the first entry field following the timestamp. It is
    // preceded by a space and followed by a comma
    std::string search = " " + std::to_string(recordID) + ",";

    // Loop through the ipmi_sel log entries
    for (const std::filesystem::path& file : selLogFiles) {
        std::ifstream logStream(file);
        if (!logStream.is_open()) {
            continue;
        }

        while (std::getline(logStream, entry)) {
            // Check if the record ID matches
            if (entry.find(search) != std::string::npos) {
                return true;
            }
        }
    }
    return false;
}

static uint16_t
getNextRecordID(const uint16_t recordID,
                const std::vector<std::filesystem::path>& selLogFiles)
{
    uint16_t nextRecordID = recordID + 1;
    std::string entry;
    if (findSELEntry(nextRecordID, selLogFiles, entry)) {
        return nextRecordID;
    } else {
        return ipmi::sel::lastEntry;
    }
}

static int fromHexStr(const std::string& hexStr, std::vector<uint8_t>& data)
{
    for (unsigned int i = 0; i < hexStr.size(); i += 2) {
        try {
            data.push_back(static_cast<uint8_t>(
                               std::stoul(hexStr.substr(i, 2), nullptr, 16)));
        } catch (std::invalid_argument& e) {
            phosphor::logging::log<phosphor::logging::level::ERR>(e.what());
            return -1;
        } catch (std::out_of_range& e) {
            phosphor::logging::log<phosphor::logging::level::ERR>(e.what());
            return -1;
        }
    }
    return 0;
}

static bool getSELLogFiles(std::vector<std::filesystem::path>& selLogFiles)
{
    // Loop through the directory looking for ipmi_sel log files
    for (const std::filesystem::directory_entry& dirEnt :
         std::filesystem::directory_iterator(intel_oem::ipmi::sel::selLogDir)) {
        std::string filename = dirEnt.path().filename();
        if (boost::starts_with(filename, intel_oem::ipmi::sel::selLogFilename)) {
            // If we find an ipmi_sel log file, save the path
            selLogFiles.emplace_back(intel_oem::ipmi::sel::selLogDir /
                                     filename);
        }
    }
    // As the log files rotate, they are appended with a ".#" that is higher for
    // the older logs. Since we don't expect more than 10 log files, we
    // can just sort the list to get them in order from newest to oldest
    std::sort(selLogFiles.begin(), selLogFiles.end());

    return !selLogFiles.empty();
}


using systemEventType = std::tuple<
                        uint32_t, // Timestamp
                        uint16_t, // Generator ID
                        uint8_t,  // EvM Rev
                        uint8_t,  // Sensor Type
                        uint8_t,  // Sensor Number
                        uint7_t,  // Event Type
                        bool,     // Event Direction
                        std::array<uint8_t, intel_oem::ipmi::sel::systemEventSize>>; // Event Data
using oemTsEventType = std::tuple<
                       uint32_t,                                                   // Timestamp
                       std::array<uint8_t, intel_oem::ipmi::sel::oemTsEventSize>>; // Event Data
using oemEventType =
    std::array<uint8_t, intel_oem::ipmi::sel::oemEventSize>; // Event Data

ipmi::RspType<uint16_t, // Next Record ID
     uint16_t, // Record ID
     uint8_t,  // Record Type
     std::variant<systemEventType, oemTsEventType,
     oemEventType>> // Record Content
     ipmiStorageGetSELEntry(uint16_t reservationID, uint16_t targetID,
                            uint8_t offset, uint8_t size)
{
    // Only support getting the entire SEL record. If a partial size or non-zero
    // offset is requested, return an error
    if (offset != 0 || size != ipmi::sel::entireRecord) {
        return ipmi::responseRetBytesUnavailable();
    }

    // Check the reservation ID if one is provided or required (only if the
    // offset is non-zero)
    if (reservationID != 0 || offset != 0) {
        if (!checkSELReservation(reservationID)) {
            return ipmi::responseInvalidReservationId();
        }
    }

    // Get the ipmi_sel log files
    std::vector<std::filesystem::path> selLogFiles;
    if (!getSELLogFiles(selLogFiles)) {
        return ipmi::responseSensorInvalid();
    }

    std::string targetEntry;

    if (targetID == ipmi::sel::firstEntry) {
        // The first entry will be at the top of the oldest log file
        std::ifstream logStream(selLogFiles.back());
        if (!logStream.is_open()) {
            return ipmi::responseUnspecifiedError();
        }

        if (!std::getline(logStream, targetEntry)) {
            return ipmi::responseUnspecifiedError();
        }
    } else if (targetID == ipmi::sel::lastEntry) {
        // The last entry will be at the bottom of the newest log file
        std::ifstream logStream(selLogFiles.front());
        if (!logStream.is_open()) {
            return ipmi::responseUnspecifiedError();
        }

        std::string line;
        while (std::getline(logStream, line)) {
            targetEntry = line;
        }
    } else {
        if (!findSELEntry(targetID, selLogFiles, targetEntry)) {
            return ipmi::responseSensorInvalid();
        }
    }

    // The format of the ipmi_sel message is "<Timestamp>
    // <ID>,<Type>,<EventData>,[<Generator ID>,<Path>,<Direction>]".
    // First get the Timestamp
    size_t space = targetEntry.find_first_of(" ");
    if (space == std::string::npos) {
        return ipmi::responseUnspecifiedError();
    }
    std::string entryTimestamp = targetEntry.substr(0, space);
    // Then get the log contents
    size_t entryStart = targetEntry.find_first_not_of(" ", space);
    if (entryStart == std::string::npos) {
        return ipmi::responseUnspecifiedError();
    }
    std::string_view entry(targetEntry);
    entry.remove_prefix(entryStart);
    // Use split to separate the entry into its fields
    std::vector<std::string> targetEntryFields;
    boost::split(targetEntryFields, entry, boost::is_any_of(","),
                 boost::token_compress_on);
    if (targetEntryFields.size() < 3) {
        return ipmi::responseUnspecifiedError();
    }
    std::string& recordIDStr = targetEntryFields[0];
    std::string& recordTypeStr = targetEntryFields[1];
    std::string& eventDataStr = targetEntryFields[2];

    uint16_t recordID;
    uint8_t recordType;
    try {
        recordID = std::stoul(recordIDStr);
        recordType = std::stoul(recordTypeStr, nullptr, 16);
    } catch (const std::invalid_argument&) {
        return ipmi::responseUnspecifiedError();
    }
    uint16_t nextRecordID = getNextRecordID(recordID, selLogFiles);
    std::vector<uint8_t> eventDataBytes;
    if (fromHexStr(eventDataStr, eventDataBytes) < 0) {
        return ipmi::responseUnspecifiedError();
    }

    if (recordType == intel_oem::ipmi::sel::systemEvent &&
        targetEntryFields.size() > 4) {
        // Get the timestamp
        std::tm timeStruct = {};
        std::istringstream entryStream(entryTimestamp);

        uint32_t timestamp = ipmi::sel::invalidTimeStamp;
        if (entryStream >> std::get_time(&timeStruct, "%Y-%m-%dT%H:%M:%S")) {
            timestamp = std::mktime(&timeStruct);
        }

        // Set the event message revision
        uint8_t evmRev = intel_oem::ipmi::sel::eventMsgRev;

        uint16_t generatorID = 0;
        uint8_t sensorType = 0;
        uint16_t sensorAndLun = 0;
        uint8_t sensorNum = 0xFF;
        uint7_t eventType = 0;
        bool eventDir = 0;
        // System type events should have six fields
        if (targetEntryFields.size() >= 6) {
            std::string& generatorIDStr = targetEntryFields[3];
            std::string& sensorPath = targetEntryFields[4];
            std::string& eventDirStr = targetEntryFields[5];

            // Get the generator ID
            try {
                generatorID = std::stoul(generatorIDStr, nullptr, 16);
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid Generator ID\n";
            }

            std::string_view sensorName(sensorPath);
            sensorName.remove_prefix(
                std::min(sensorName.find_last_of("/") + 1, sensorName.size())
            );

            // sensorName is from sensorPath, so sensorName size may more then 16
            // we need to compare sting with SDR, so only match first 16 bytes
            if (sensorName.size() > 16)
                sensorName.remove_suffix(sensorName.size() - 16);

            std::cerr << "sensorName " << sensorName << "\n";

            bool findSdr = false;
            for (int i = 0; i < sdrTotalRecordCount; i++) {
                if (strlen(sdrMapEntry[i].sensor_name) != sensorName.size())
                    continue;

                std::string sensorNameString;
                sensorNameString.assign(std::string(sensorName).c_str(), strlen(std::string(sensorName).c_str()));
                sensorNameString = boost::replace_all_copy(sensorNameString, "_", " ");

                if (strcmp(sdrMapEntry[i].sensor_name, std::string(sensorName).c_str()) == 0 ||
                    strcmp(sdrMapEntry[i].sensor_name, sensorNameString.c_str()) == 0) {
                    sensorNum = sdrMapEntry[i].sensor_number;
                    sensorType = sdrMapEntry[i].sensor_type;
                    eventType = sdrMapEntry[i].event_type;
                    break;
                }
            }

            // Get the sensor type, sensor number, and event type for the sensor
            //sensorType = getSensorTypeFromPath(sensorPath);
            //sensorAndLun = getSensorNumberFromPath(sensorPath);
            //sensorNum = static_cast<uint8_t>(sensorAndLun);
            generatorID |= sensorAndLun >> 8;
            //eventType = getSensorEventTypeFromPath(sensorPath);

            // Get the event direction
            try {
                eventDir = std::stoul(eventDirStr) ? 0 : 1;
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid Event Direction\n";
            }
        }

        // Only keep the eventData bytes that fit in the record
        std::array<uint8_t, intel_oem::ipmi::sel::systemEventSize> eventData{};
        std::copy_n(eventDataBytes.begin(),
                    std::min(eventDataBytes.size(), eventData.size()),
                    eventData.begin());

        return ipmi::responseSuccess(
                   nextRecordID, recordID, recordType,
                   systemEventType{timestamp, generatorID, evmRev, sensorType,
                                   sensorNum, eventType, eventDir, eventData});
    } else if ((recordType == intel_oem::ipmi::sel::systemEvent &&
                targetEntryFields.size() == 4) ||
               (recordType >= intel_oem::ipmi::sel::oemTsEventFirst &&
                recordType <= intel_oem::ipmi::sel::oemTsEventLast)) {
        // Get the timestamp
        std::tm timeStruct = {};
        std::istringstream entryStream(entryTimestamp);

        uint32_t timestamp = ipmi::sel::invalidTimeStamp;
        if (entryStream >> std::get_time(&timeStruct, "%Y-%m-%dT%H:%M:%S")) {
            timestamp = std::mktime(&timeStruct);
        }

        // Only keep the bytes that fit in the record
        std::array<uint8_t, intel_oem::ipmi::sel::oemTsEventSize> eventData{};
        std::copy_n(eventDataBytes.begin(),
                    std::min(eventDataBytes.size(), eventData.size()),
                    eventData.begin());

        return ipmi::responseSuccess(nextRecordID, recordID, recordType,
                                     oemTsEventType{timestamp, eventData});
    } else if (recordType >= intel_oem::ipmi::sel::oemEventFirst) {
        // Only keep the bytes that fit in the record
        std::array<uint8_t, intel_oem::ipmi::sel::oemEventSize> eventData{};
        std::copy_n(eventDataBytes.begin(),
                    std::min(eventDataBytes.size(), eventData.size()),
                    eventData.begin());

        return ipmi::responseSuccess(nextRecordID, recordID, recordType,
                                     eventData);
    }

    return ipmi::responseUnspecifiedError();
}

void registerStorageFunctions()
{
    // <Get SEL Entry>
    ipmi::registerHandler(ipmi::prioOdmBase, ipmi::netFnStorage,
                          ipmi::storage::cmdGetSelEntry, ipmi::Privilege::User,
                          ipmiStorageGetSELEntry);
}
} // namespace storage
} // namespace ipmi
