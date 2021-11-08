/*
// Copyright (c) 2021 Phoenix Technologies Ltd.
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

#include <iostream>
#include <ipmid/api.h>
#include <ipmid/utils.hpp>
#include <ipmid/types.hpp>
#include <ipmid/handler.hpp>
#include <boost/container/flat_map.hpp>
#include <mutex>
#include <fcntl.h>
#include <sdbusplus/message/types.hpp>
#include <sdrcommands.hpp>
#include <algorithm>

#define ACTIVE_SDR_FILE "/usr/share/phoenix-sensor-config/SDR.active"

#define MAX_SDR_MAP_ENTRIES 512
#define MAX_SDR_RECORD_SIZE (int)(sizeof(struct sdr_rec_header) + 255)

// Mapping index for faster search SDR Repository
struct sdr_rec_map *sdrMapEntry;

int sdrTotalRecordCount = 0;

namespace ipmi
{
std::mutex sdrMutex;

void sdrRepositoryDeviceSetup() __attribute__((constructor));

static uint16_t sdrReservationID;

constexpr static const uint32_t noTimestamp = 0xFFFFFFFF;

// TODO: The sdrLastAdd and sdrLastRemove should consider to save at NV.
static uint32_t sdrLastAdd = noTimestamp;
static uint32_t sdrLastRemove = noTimestamp;

// All SDR raw bytes point
static uint8_t *sdrRawCachePtr;
constexpr static const uint32_t sdrRawCachePtrSize = MAX_SDR_RECORD_SIZE *
        MAX_SDR_MAP_ENTRIES;

SensorSubTree sensorTree;

static constexpr int sensorMapUpdatePeriod = 10;
using ManagedObjectType =
    std::map<sdbusplus::message::object_path,
    std::map<std::string, std::map<std::string, DbusVariant>>>;
static boost::container::flat_map<std::string, ManagedObjectType> SensorCache;

static sdbusplus::bus::match::match sensorAdded(
    *getSdBus(),
    "type='signal',member='InterfacesAdded',arg0path='/xyz/openbmc_project/"
    "sensors/'",
    [](sdbusplus::message::message& m)
{
    sensorTree.clear();
    sdrLastAdd = std::chrono::duration_cast<std::chrono::seconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                 .count();
});

static sdbusplus::bus::match::match sensorRemoved(
    *getSdBus(),
    "type='signal',member='InterfacesRemoved',arg0path='/xyz/openbmc_project/"
    "sensors/'",
    [](sdbusplus::message::message& m)
{
    sensorTree.clear();
    sdrLastRemove = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                    .count();
});

static bool getSensorMap(boost::asio::yield_context yield,
                         std::string sensorConnection, std::string sensorPath,
                         SensorMap& sensorMap,
                         int updatePeriod = sensorMapUpdatePeriod)
{
    static boost::container::flat_map<
    std::string, std::chrono::time_point<std::chrono::steady_clock>>
            updateTimeMap;

    auto updateFind = updateTimeMap.find(sensorConnection);
    auto lastUpdate = std::chrono::time_point<std::chrono::steady_clock>();
    if (updateFind != updateTimeMap.end()) {
        lastUpdate = updateFind->second;
    }

    auto now = std::chrono::steady_clock::now();

    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate)
        .count() > updatePeriod) {
        std::shared_ptr<sdbusplus::asio::connection> dbus = getSdBus();
        boost::system::error_code ec;
        auto managedObjects = dbus->yield_method_call<ManagedObjectType>(
                                  yield, ec, sensorConnection.c_str(), "/",
                                  "org.freedesktop.DBus.ObjectManager", "GetManagedObjects");
        if (ec) {
            phosphor::logging::log<phosphor::logging::level::ERR>(
                "GetMangagedObjects for getSensorMap failed",
                phosphor::logging::entry("ERROR=%s", ec.message().c_str()));

            return false;
        }

        SensorCache[sensorConnection] = managedObjects;
        // Update time after finish building the map which allow the
        // data to be cached for updatePeriod plus the build time.
        updateTimeMap[sensorConnection] = std::chrono::steady_clock::now();
    }
    auto connection = SensorCache.find(sensorConnection);
    if (connection == SensorCache.end()) {
        return false;
    }
    auto path = connection->second.find(sensorPath);
    if (path == connection->second.end()) {
        return false;
    }
    sensorMap = path->second;

    return true;
}

static inline uint8_t
scaleIPMIValueFromDouble(const double value, const int16_t mValue,
                         const int8_t rExp, const int16_t bValue,
                         const int8_t bExp, const bool bSigned)
{
    // Avoid division by zero below
    if (mValue == 0) {
        throw std::out_of_range("Scaling multiplier is uninitialized");
    }

    auto dM = static_cast<double>(mValue);
    auto dB = static_cast<double>(bValue);

    // Solve the IPMI equation for x, instead of y
    // https://www.wolframalpha.com/input/?i=solve+y%3D%28%28M*x%29%2B%28B*%2810%5EE%29%29%29*%2810%5ER%29+for+x
    // x = (10^(-rExp) (y - B 10^(rExp + bExp)))/M and M 10^rExp!=0
    // TODO(): Compare with this alternative solution from SageMathCell
    // https://sagecell.sagemath.org/?z=eJyrtC1LLNJQr1TX5KqAMCuATF8I0xfIdIIwnYDMIteKAggPxAIKJMEFkiACxfk5Zaka0ZUKtrYKGhq-CloKFZoK2goaTkCWhqGBgpaWAkilpqYmQgBklmasDlAlAMB8JP0=&lang=sage&interacts=eJyLjgUAARUAuQ==
    double dX =
        (std::pow(10.0, -rExp) * (value - (dB * std::pow(10.0, rExp + bExp)))) /
        dM;

    auto scaledValue = static_cast<int32_t>(std::round(dX));

    int32_t minClamp;
    int32_t maxClamp;

    // Because of rounding and integer truncation of scaling factors,
    // sometimes the resulting byte is slightly out of range.
    // Still allow this, but clamp the values to range.
    if (bSigned) {
        minClamp = std::numeric_limits<int8_t>::lowest();
        maxClamp = std::numeric_limits<int8_t>::max();
    } else {
        minClamp = std::numeric_limits<uint8_t>::lowest();
        maxClamp = std::numeric_limits<uint8_t>::max();
    }

    auto clampedValue = std::clamp(scaledValue, minClamp, maxClamp);

    // This works for both signed and unsigned,
    // because it is the same underlying byte storage.
    return static_cast<uint8_t>(clampedValue);
}

static int getFileSize(const char *fileName, long *fileSize)
{
    int fd;

    // Check the point empty
    if (fileName == NULL) {
        return StatusFileNameNull;
    }

    // Open file
    fd = open(fileName, O_RDONLY | O_SYNC);

    if (fd < 0) {
        return StatusFileOpenError;
    }

    // Reposition to the size of the file plus offset byte
    *fileSize = lseek(fd, 0, SEEK_END);

    // Close file
    close(fd);

    return StatusOK;
}

static int getPathfromName(char* sensorName, char* sensorConnect, char* sensorPath)
{
    if (sensorTree.empty() && !getSensorSubtree(sensorTree)) {
        return -1;
    }

    std::string sensorNameString;
    sensorNameString.assign(sensorName, strlen(sensorName));
    sensorNameString = boost::replace_all_copy(sensorNameString, " ", "_");

    for (const auto& sensor : sensorTree) {
        size_t typeEnd = sensor.first.rfind("/");
        std::string label = sensor.first.substr(typeEnd + 1, sensor.first.size() - typeEnd);

        if (strncmp(sensorNameString.c_str(), label.c_str(), strlen(sensorName)) == 0) {
            strcpy(sensorPath, sensor.first.c_str());
            strcpy(sensorConnect, sensor.second.begin()->first.c_str());
            break;
        }
    }

    return 0;
}

static int mappingSdrRec(int sdrRepositorySize,
                         uint8_t *sdrRepositoryPtr)
{
    long offset = 0;
    int sdrIndex = 0;
    struct sdr_rec_header sdr_header;

    std::lock_guard<std::mutex> lock(sdrMutex);

    while (offset < sdrRepositorySize) {
        // Load SDR header
        memcpy(&sdr_header, sdrRepositoryPtr + offset,
               sizeof(struct sdr_rec_header));

        // header invaild
        if (sdr_header.ver != ipmiSdrVersion ||
            sdr_header.type == 0x00) {
            continue;
        }

        // Save mapping information
        sdrMapEntry[sdrIndex].record_id = sdr_header.record_id;
        sdrMapEntry[sdrIndex].length = sizeof(struct sdr_rec_header)
                                       + sdr_header.length;

        int nameLength = 0;
        switch (sdr_header.type) {
        case SDR_RECORD_TYPE_FULL_SENSOR:
            nameLength = *(sdrRepositoryPtr + offset + 47) & 0x1f; //TODO: create struct, not hard code offset
            sdrMapEntry[sdrIndex].sensor_number = *(sdrRepositoryPtr + offset + 7);
            sdrMapEntry[sdrIndex].sensor_type = *(sdrRepositoryPtr + offset + 12);
            sdrMapEntry[sdrIndex].event_type = *(sdrRepositoryPtr + offset + 13);
            memcpy(sdrMapEntry[sdrIndex].sensor_name, sdrRepositoryPtr + offset + 48, nameLength);
            break;

        case SDR_RECORD_TYPE_COMPACT_SENSOR:
            nameLength = *(sdrRepositoryPtr + offset + 31) & 0x1f;
            sdrMapEntry[sdrIndex].sensor_number = *(sdrRepositoryPtr + offset + 7);
            sdrMapEntry[sdrIndex].sensor_type = *(sdrRepositoryPtr + offset + 12);
            sdrMapEntry[sdrIndex].event_type = *(sdrRepositoryPtr + offset + 13);
            memcpy(sdrMapEntry[sdrIndex].sensor_name, sdrRepositoryPtr + offset + 32, nameLength);
            break;

        case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
            nameLength = *(sdrRepositoryPtr + offset + 16) & 0x1f;
            sdrMapEntry[sdrIndex].sensor_number = *(sdrRepositoryPtr + offset + 7);
            sdrMapEntry[sdrIndex].sensor_type = *(sdrRepositoryPtr + offset + 10);
            sdrMapEntry[sdrIndex].event_type = *(sdrRepositoryPtr + offset + 11);
            memcpy(sdrMapEntry[sdrIndex].sensor_name, sdrRepositoryPtr + offset + 17, nameLength);
            break;
        }
        getPathfromName(sdrMapEntry[sdrIndex].sensor_name,
                        sdrMapEntry[sdrIndex].obj_connect,
                        sdrMapEntry[sdrIndex].obj_path);

        // Reocrd offset on sdrRepositoryPtr
        sdrMapEntry[sdrIndex].offset = offset;

        sdrIndex++;

        // Go throuth offset to next record
        offset += sizeof(struct sdr_rec_header) + sdr_header.length;

        // Adjust offset for next record
        if ((offset % 16) != 0)
            offset = (offset / 16 + 1) * 16;
    }

    sdrTotalRecordCount = sdrIndex;

    return StatusOK;
}

static int sdrRepositoryInit()
{
    int retVal = StatusOK;
    int fd;
    long fileSize;
    auto sdrFileName = ACTIVE_SDR_FILE;

    // Check SDR filesize.
    retVal = getFileSize(sdrFileName, &fileSize);
    if (retVal != 0 || fileSize <= 1) {
        return retVal;
    }

    // Allocate memory
    sdrMapEntry = (struct sdr_rec_map *)
                  malloc (sizeof(struct sdr_rec_map) * MAX_SDR_MAP_ENTRIES);

    // Can't allocate memory for sdrMapEntry.
    if (sdrMapEntry == NULL) {
        return StatusAllocateError;
    }

    // Initial sdrMapEntry
    memset(sdrMapEntry, 0, sizeof(struct sdr_rec_map) * MAX_SDR_MAP_ENTRIES);

    sdrRawCachePtr = (uint8_t *) malloc(sizeof(uint8_t) * sdrRawCachePtrSize);

    // Can't allocate memory for sdrRawCachePtr.
    if (sdrRawCachePtr == NULL) {
        free (sdrMapEntry);
        return StatusAllocateError;
    }

    // Initial sdrRawCachePtr
    memset(sdrRawCachePtr, 0, sdrRawCachePtrSize);

    // Open SDR file
    fd = open(sdrFileName, O_RDONLY | O_SYNC);
    if (fd < 0) {
        free (sdrMapEntry);
        free (sdrRawCachePtr);

        return StatusFileOpenError;
    }

    if (lseek(fd, 0, SEEK_SET) >= 0) {
        // Read full SDR file to Cache
        read(fd, sdrRawCachePtr, fileSize);
    }

    // Close SDR file
    close(fd);

    // Mapping SDR Record
    mappingSdrRec(fileSize, sdrRawCachePtr);

    return StatusOK;
}

ipmi::RspType<uint8_t,  // sdr version
     uint16_t, // record count
     uint16_t, // free space
     uint32_t, // most recent addition
     uint32_t, // most recent erase
     uint8_t   // operationSupport
     >
     ipmiStorageGetSDRRepositoryInfo(ipmi::Context::ptr ctx)
{
    constexpr const uint16_t unspecifiedFreeSpace = 0xFFFF;
    constexpr const uint8_t operationSupport = 0;

    uint16_t recordCount = sdrTotalRecordCount;

    return ipmi::responseSuccess(ipmiSdrVersion, recordCount,
                                 unspecifiedFreeSpace, sdrLastAdd,
                                 sdrLastRemove, operationSupport);
}

ipmi::RspType<uint16_t> ipmiStorageReserveSDR()
{
    sdrReservationID++;

    if (sdrReservationID == 0) {
        sdrReservationID++;
    }

    return ipmi::responseSuccess(sdrReservationID);
}

ipmi::RspType<uint16_t,            // next record ID
std::vector<uint8_t> // payload
>
ipmiStorageGetSDR(ipmi::Context::ptr ctx, uint16_t reservationID,
                  uint16_t recordID, uint8_t offset, uint8_t bytesToRead)
{
    // Reservation required for partial reads with non zero offset into
    // record
    if ((sdrReservationID == 0 || reservationID != sdrReservationID) &&
        offset) {
        return ipmi::responseInvalidReservationId();
    }

    // Check if SDR repository is empty
    if (sdrTotalRecordCount == 0) {
        return ipmi::responseResponseError();
    }

    int MapIndex = -1;

    // Check if request last record
    if (recordID == 0xFFFF) {
        MapIndex = sdrTotalRecordCount - 1;
    }
    // Check if request first record
    else if (recordID == 0x0000) {
        MapIndex = 0;
    }
    // Loop through entire SDR mapping, search for record ID match
    else {
        std::lock_guard<std::mutex> lock(sdrMutex);

        for (int j = 0; j < sdrTotalRecordCount; j++) {
            if (sdrMapEntry[j].record_id == recordID) {
                // Get the Index of Record
                MapIndex = j;
                break;
            }
        }

        // Not find record ID
        if (MapIndex < 0) {
            return ipmi::responseInvalidFieldRequest();
        }
    }

    // Check if request to read entire record (0xFF)
    if (bytesToRead == 0xFF) {
        bytesToRead = sdrMapEntry[MapIndex].length;
    }

    if ((offset + bytesToRead) > sdrMapEntry[MapIndex].length) {
        return ipmi::responseInvalidFieldRequest();
    }

    uint16_t nextRecord = (MapIndex + 1) >= sdrTotalRecordCount
                          ? 0xFFFF : sdrMapEntry[MapIndex + 1].record_id;

    std::vector<uint8_t> recordData;

    // Calculate record start address in sdrRawCachePtr
    uint8_t *respStart = sdrRawCachePtr
                         + sdrMapEntry[MapIndex].offset
                         + offset;

    recordData.insert(recordData.end(), respStart,
                      respStart + bytesToRead);

    return ipmi::responseSuccess(nextRecord, recordData);
}

ipmi::RspType<uint8_t, uint8_t, uint8_t, std::optional<uint8_t>>
        ipmiGetSensorReading(ipmi::Context::ptr ctx, uint8_t sensnum)
{
    uint8_t value, operation = 0, stateLsb, stateMsb;
    uint8_t sdrType;
    bool isThresholdSensor = false;

    int MapIndex = -1;
    for (int i = 0; i < sdrTotalRecordCount; i++) {
        struct sdr_rec_header sdr_header;

        memcpy(&sdr_header, sdrRawCachePtr + sdrMapEntry[i].offset,
               sizeof(struct sdr_rec_header));

        sdrType = sdr_header.type;
        if (sdrType != SDR_RECORD_TYPE_FULL_SENSOR &&
            sdrType != SDR_RECORD_TYPE_COMPACT_SENSOR) {
            continue;
        }

        if (sdrMapEntry[i].sensor_number == sensnum) {
            MapIndex = i;
            break;
        }
    }

    if (MapIndex == -1) {
        // Sensor number not present in SDR
        return ipmi::responseInvalidFieldRequest();
    }

    int16_t mValue = 1;
    int16_t bValue = 0;
    int8_t rExp = 0;
    int8_t bExp = 0;
    bool bSigned = false;

    if (sdrType == SDR_RECORD_TYPE_FULL_SENSOR) {
        struct sdr_record_full_sensor sdrData;
        memcpy(&sdrData,
               sdrRawCachePtr + sdrMapEntry[MapIndex].offset,
               sdrMapEntry[MapIndex].length);

        if (sdrData.event_type == 0x01) {
            isThresholdSensor = true;

            mValue = __TO_M(sdrData.mtol);
            bValue = __TO_B(sdrData.bacc);
            rExp = __TO_R_EXP(sdrData.bacc);
            bExp = __TO_B_EXP(sdrData.bacc);

            if (sdrData.unit.analog != 0)
                bSigned = true;
        }
    } else if (sdrType == SDR_RECORD_TYPE_COMPACT_SENSOR) {
        struct sdr_record_compact_sensor sdrData;
        memcpy(&sdrData,
               sdrRawCachePtr + sdrMapEntry[MapIndex].offset,
               sdrMapEntry[MapIndex].length);

        if (sdrData.event_type == 0x01) {
            isThresholdSensor = true;
        }
    }

    if (strlen(sdrMapEntry[MapIndex].obj_connect) == 0 || 
        strlen(sdrMapEntry[MapIndex].obj_path) == 0) {
            getPathfromName(sdrMapEntry[MapIndex].sensor_name,
                            sdrMapEntry[MapIndex].obj_connect,
                            sdrMapEntry[MapIndex].obj_path);
    }

    if (strlen(sdrMapEntry[MapIndex].obj_connect) == 0 || 
        strlen(sdrMapEntry[MapIndex].obj_path) == 0) {
        operation &=
            ~static_cast<uint8_t>(IPMISensorReadingByte2::sensorScanningEnable);

        return ipmi::responseSuccess(0, operation, 0, 0);
    }

    SensorMap sensorMap;
    if (!getSensorMap(ctx->yield, sdrMapEntry[MapIndex].obj_connect, sdrMapEntry[MapIndex].obj_path, sensorMap)) {
        std::fprintf(stderr, "Fail to getSensorMap for %s\n", sdrMapEntry[MapIndex].obj_path);

        operation &=
            ~static_cast<uint8_t>(IPMISensorReadingByte2::sensorScanningEnable);

        return ipmi::responseSuccess(0, operation, 0, 0);
    }

    auto sensorObject = sensorMap.find("xyz.openbmc_project.Sensor.Value");

    if (sensorObject == sensorMap.end() ||
        sensorObject->second.find("Value") == sensorObject->second.end()) {
        std::fprintf(stderr, "Fail to get Value\n");
        return ipmi::responseResponseError();
    }
    auto& valueVariant = sensorObject->second["Value"];
    double reading = std::visit(VariantToDoubleVisitor(), valueVariant);

    uint16_t rawValue = 0;

    if (isThresholdSensor == true) {
        rawValue =
            scaleIPMIValueFromDouble(reading, mValue, rExp, bValue, bExp, bSigned);

        value = static_cast<uint8_t>(rawValue);

        uint8_t stateLsb = 0;

        auto warningObject =
            sensorMap.find("xyz.openbmc_project.Sensor.Threshold.Warning");
        if (warningObject != sensorMap.end()) {
            auto alarmHigh = warningObject->second.find("WarningAlarmHigh");
            auto alarmLow = warningObject->second.find("WarningAlarmLow");
            if (alarmHigh != warningObject->second.end()) {
                if (std::get<bool>(alarmHigh->second)) {
                    stateLsb |= static_cast<uint8_t>(
                                     IPMISensorReadingByte3::upperNonCritical);
                }
            }
            if (alarmLow != warningObject->second.end()) {
                if (std::get<bool>(alarmLow->second)) {
                    stateLsb |= static_cast<uint8_t>(
                                     IPMISensorReadingByte3::lowerNonCritical);
                }
            }
        }

        auto criticalObject =
            sensorMap.find("xyz.openbmc_project.Sensor.Threshold.Critical");
        if (criticalObject != sensorMap.end()) {
            auto alarmHigh = criticalObject->second.find("CriticalAlarmHigh");
            auto alarmLow = criticalObject->second.find("CriticalAlarmLow");
            if (alarmHigh != criticalObject->second.end()) {
                if (std::get<bool>(alarmHigh->second)) {
                    stateLsb |=
                        static_cast<uint8_t>(IPMISensorReadingByte3::upperCritical);
                }
            }
            if (alarmLow != criticalObject->second.end()) {
                if (std::get<bool>(alarmLow->second)) {
                    stateLsb |=
                        static_cast<uint8_t>(IPMISensorReadingByte3::lowerCritical);
                }
            }
        }
    } else {
        rawValue = static_cast<int16_t>(std::round(reading));

        stateLsb = rawValue & 0xff;
        stateMsb = ((rawValue & 0xff00) >> 8) | (1 << 7); // reserved bit 7 returns as 1b
        value = 0;
    }

    operation |=
        static_cast<uint8_t>(IPMISensorReadingByte2::sensorScanningEnable);
    operation |=
        static_cast<uint8_t>(IPMISensorReadingByte2::eventMessagesEnable);

    auto availableObject =
        sensorMap.find("xyz.openbmc_project.State.Decorator.Availability");
    if (availableObject != sensorMap.end()) {
        auto available = availableObject->second.find("Available");

        if (available != availableObject->second.end()) {
            if (std::get<bool>(available->second) == false) {
                operation |=
                    static_cast<uint8_t>(IPMISensorReadingByte2::readingStateUnavailable);
            }
        }
    }

    auto operationalStatusObject =
        sensorMap.find("xyz.openbmc_project.State.Decorator.OperationalStatus");
    if (operationalStatusObject != sensorMap.end()) {
        auto functional = operationalStatusObject->second.find("Functional");

        if (functional != operationalStatusObject->second.end()) {
            if (std::get<bool>(functional->second) == false) {
                operation &=
                    ~static_cast<uint8_t>(IPMISensorReadingByte2::sensorScanningEnable);
            }
        }
    }

    return ipmi::responseSuccess(value, operation, stateLsb, stateMsb);
}

ipmi::RspType<uint8_t, // readable
              uint8_t, // lowerNCrit
              uint8_t, // lowerCrit
              uint8_t, // lowerNrecoverable
              uint8_t, // upperNC
              uint8_t, // upperCrit
              uint8_t> // upperNRecoverable
    ipmiSenGetSensorThresholds(ipmi::Context::ptr ctx, uint8_t sensorNumber)
{
    if (sensorNumber == reservedSensorNumber)
    {
        return ipmi::responseInvalidFieldRequest();
    }

    uint8_t sdrType;
    bool isThresholdSensor = false;

    int MapIndex = -1;
    for (int i = 0; i < sdrTotalRecordCount; i++) {
        struct sdr_rec_header sdr_header;

        memcpy(&sdr_header, sdrRawCachePtr + sdrMapEntry[i].offset,
               sizeof(struct sdr_rec_header));

        sdrType = sdr_header.type;
        if (sdrType != SDR_RECORD_TYPE_FULL_SENSOR &&
            sdrType != SDR_RECORD_TYPE_COMPACT_SENSOR) {
            continue;
        }

        if (sdrMapEntry[i].sensor_number == sensorNumber) {
            MapIndex = i;
            break;
        }
    }

    if (MapIndex == -1) {
        // Sensor number not present in SDR
        return ipmi::responseInvalidFieldRequest();
    }

    int16_t mValue = 1;
    int16_t bValue = 0;
    int8_t rExp = 0;
    int8_t bExp = 0;
    bool bSigned = false;

    if (sdrType == SDR_RECORD_TYPE_FULL_SENSOR) {
        struct sdr_record_full_sensor sdrData;
        memcpy(&sdrData,
               sdrRawCachePtr + sdrMapEntry[MapIndex].offset,
               sdrMapEntry[MapIndex].length);

        if (sdrData.event_type == 0x01) {
            isThresholdSensor = true;

            mValue = __TO_M(sdrData.mtol);
            bValue = __TO_B(sdrData.bacc);
            rExp = __TO_R_EXP(sdrData.bacc);
            bExp = __TO_B_EXP(sdrData.bacc);

            if (sdrData.unit.analog != 0)
                bSigned = true;
        }
    } else if (sdrType == SDR_RECORD_TYPE_COMPACT_SENSOR) {
        struct sdr_record_compact_sensor sdrData;
        memcpy(&sdrData,
               sdrRawCachePtr + sdrMapEntry[MapIndex].offset,
               sdrMapEntry[MapIndex].length);

        if (sdrData.event_type == 0x01) {
            isThresholdSensor = true;
        }
    }

    if (strlen(sdrMapEntry[MapIndex].obj_connect) == 0 ||
        strlen(sdrMapEntry[MapIndex].obj_path) == 0) {
            getPathfromName(sdrMapEntry[MapIndex].sensor_name,
                            sdrMapEntry[MapIndex].obj_connect,
                            sdrMapEntry[MapIndex].obj_path);
    }

    if (strlen(sdrMapEntry[MapIndex].obj_connect) == 0 ||
        strlen(sdrMapEntry[MapIndex].obj_path) == 0) {
        return ipmi::responseResponseError();
    }

    SensorMap sensorMap;
    if (!getSensorMap(ctx->yield, sdrMapEntry[MapIndex].obj_connect, sdrMapEntry[MapIndex].obj_path, sensorMap)) {
        std::fprintf(stderr, "Fail to getSensorMap for %s\n", sdrMapEntry[MapIndex].obj_path);

        return ipmi::responseResponseError();
    }

    IPMIThresholds thresholdData;

    auto warningInterface =
            sensorMap.find("xyz.openbmc_project.Sensor.Threshold.Warning");
    auto criticalInterface =
            sensorMap.find("xyz.openbmc_project.Sensor.Threshold.Critical");

    if ((warningInterface != sensorMap.end()) ||
        (criticalInterface != sensorMap.end())) {
        if (warningInterface != sensorMap.end())
        {
            auto& warningMap = warningInterface->second;

            auto warningHigh = warningMap.find("WarningHigh");
            auto warningLow = warningMap.find("WarningLow");

            if (warningHigh != warningMap.end())
            {
                double value =
                        std::visit(VariantToDoubleVisitor(), warningHigh->second);
                thresholdData.warningHigh = scaleIPMIValueFromDouble(
                        value, mValue, rExp, bValue, bExp, bSigned);
            }
            if (warningLow != warningMap.end())
            {
                double value =
                        std::visit(VariantToDoubleVisitor(), warningLow->second);
                thresholdData.warningLow = scaleIPMIValueFromDouble(
                        value, mValue, rExp, bValue, bExp, bSigned);
            }
        }
        if (criticalInterface != sensorMap.end())
        {
            auto& criticalMap = criticalInterface->second;

            auto criticalHigh = criticalMap.find("CriticalHigh");
            auto criticalLow = criticalMap.find("CriticalLow");

            if (criticalHigh != criticalMap.end())
            {
                double value =
                        std::visit(VariantToDoubleVisitor(), criticalHigh->second);
                thresholdData.criticalHigh = scaleIPMIValueFromDouble(
                        value, mValue, rExp, bValue, bExp, bSigned);
            }
            if (criticalLow != criticalMap.end())
            {
                double value =
                        std::visit(VariantToDoubleVisitor(), criticalLow->second);
                thresholdData.criticalLow = scaleIPMIValueFromDouble(
                        value, mValue, rExp, bValue, bExp, bSigned);
            }
        }
    }

    uint8_t readable = 0;
    uint8_t lowerNC = 0;
    uint8_t lowerCritical = 0;
    uint8_t lowerNonRecoverable = 0;
    uint8_t upperNC = 0;
    uint8_t upperCritical = 0;
    uint8_t upperNonRecoverable = 0;

    if (thresholdData.warningHigh)
    {
        readable |=
            1 << static_cast<uint8_t>(IPMIThresholdRespBits::upperNonCritical);
        upperNC = *thresholdData.warningHigh;
    }
    if (thresholdData.warningLow)
    {
        readable |=
            1 << static_cast<uint8_t>(IPMIThresholdRespBits::lowerNonCritical);
        lowerNC = *thresholdData.warningLow;
    }

    if (thresholdData.criticalHigh)
    {
        readable |=
            1 << static_cast<uint8_t>(IPMIThresholdRespBits::upperCritical);
        upperCritical = *thresholdData.criticalHigh;
    }
    if (thresholdData.criticalLow)
    {
        readable |=
            1 << static_cast<uint8_t>(IPMIThresholdRespBits::lowerCritical);
        lowerCritical = *thresholdData.criticalLow;
    }

    return ipmi::responseSuccess(readable, lowerNC, lowerCritical,
                                 lowerNonRecoverable, upperNC, upperCritical,
                                 upperNonRecoverable);
}

ipmi::RspType<> ipmiSenSetSensorThresholds(
    ipmi::Context::ptr ctx, uint8_t sensorNum, bool lowerNonCriticalThreshMask,
    bool lowerCriticalThreshMask, bool lowerNonRecovThreshMask,
    bool upperNonCriticalThreshMask, bool upperCriticalThreshMask,
    bool upperNonRecovThreshMask, uint2_t reserved, uint8_t lowerNonCritical,
    uint8_t lowerCritical, uint8_t lowerNonRecoverable,
    uint8_t upperNonCritical, uint8_t upperCritical,
    uint8_t upperNonRecoverable)
{
    if (sensorNum == reservedSensorNumber)
    {
        return ipmi::responseInvalidFieldRequest();
    }

    if (reserved)
    {
        return ipmi::responseInvalidFieldRequest();
    }

    // lower nc and upper nc not suppported on any sensor
    if (lowerNonRecovThreshMask || upperNonRecovThreshMask)
    {
        return ipmi::responseInvalidFieldRequest();
    }

    // if none of the threshold mask are set, nothing to do
    if (!(lowerNonCriticalThreshMask | lowerCriticalThreshMask |
          lowerNonRecovThreshMask | upperNonCriticalThreshMask |
          upperCriticalThreshMask | upperNonRecovThreshMask))
    {
        return ipmi::responseSuccess();
    }

    uint8_t sdrType;
    bool isThresholdSensor = false;

    int MapIndex = -1;
    for (int i = 0; i < sdrTotalRecordCount; i++) {
        struct sdr_rec_header sdr_header;

        memcpy(&sdr_header, sdrRawCachePtr + sdrMapEntry[i].offset,
               sizeof(struct sdr_rec_header));

        sdrType = sdr_header.type;
        if (sdrType != SDR_RECORD_TYPE_FULL_SENSOR &&
            sdrType != SDR_RECORD_TYPE_COMPACT_SENSOR) {
            continue;
        }

        if (sdrMapEntry[i].sensor_number == sensorNum) {
            MapIndex = i;
            break;
        }
    }

    if (MapIndex == -1) {
        // Sensor number not present in SDR
        return ipmi::responseInvalidFieldRequest();
    }

    int16_t mValue = 1;
    int16_t bValue = 0;
    int8_t rExp = 0;
    int8_t bExp = 0;
    bool bSigned = false;

    if (sdrType == SDR_RECORD_TYPE_FULL_SENSOR) {
        struct sdr_record_full_sensor sdrData;
        memcpy(&sdrData,
               sdrRawCachePtr + sdrMapEntry[MapIndex].offset,
               sdrMapEntry[MapIndex].length);

        if (sdrData.event_type == 0x01) {
            isThresholdSensor = true;

            mValue = __TO_M(sdrData.mtol);
            bValue = __TO_B(sdrData.bacc);
            rExp = __TO_R_EXP(sdrData.bacc);
            bExp = __TO_B_EXP(sdrData.bacc);

            if (sdrData.unit.analog != 0)
                bSigned = true;
        }
    } else if (sdrType == SDR_RECORD_TYPE_COMPACT_SENSOR) {
        struct sdr_record_compact_sensor sdrData;
        memcpy(&sdrData,
               sdrRawCachePtr + sdrMapEntry[MapIndex].offset,
               sdrMapEntry[MapIndex].length);

        if (sdrData.event_type == 0x01) {
            isThresholdSensor = true;
        }
    }

    if (strlen(sdrMapEntry[MapIndex].obj_connect) == 0 ||
        strlen(sdrMapEntry[MapIndex].obj_path) == 0) {
            getPathfromName(sdrMapEntry[MapIndex].sensor_name,
                            sdrMapEntry[MapIndex].obj_connect,
                            sdrMapEntry[MapIndex].obj_path);
    }

    if (strlen(sdrMapEntry[MapIndex].obj_connect) == 0 ||
        strlen(sdrMapEntry[MapIndex].obj_path) == 0) {
        return ipmi::responseResponseError();
    }

    SensorMap sensorMap;
    if (!getSensorMap(ctx->yield, sdrMapEntry[MapIndex].obj_connect, sdrMapEntry[MapIndex].obj_path, sensorMap)) {
        std::fprintf(stderr, "Fail to getSensorMap for %s\n", sdrMapEntry[MapIndex].obj_path);

        return ipmi::responseResponseError();
    }

    // store a vector of property name, value to set, and interface
    std::vector<std::tuple<std::string, uint8_t, std::string>> thresholdsToSet;

    constexpr uint8_t propertyName = 0;
    constexpr uint8_t thresholdValue = 1;
    constexpr uint8_t interface = 2;

    if (lowerCriticalThreshMask || upperCriticalThreshMask)
    {
        auto findThreshold =
            sensorMap.find("xyz.openbmc_project.Sensor.Threshold.Critical");
        if (findThreshold == sensorMap.end())
        {
            return ipmi::responseInvalidFieldRequest();
        }
        if (lowerCriticalThreshMask)
        {
            auto findLower = findThreshold->second.find("CriticalLow");
            if (findLower == findThreshold->second.end())
            {
                return ipmi::responseInvalidFieldRequest();
            }
            thresholdsToSet.emplace_back("CriticalLow", lowerCritical,
                                         findThreshold->first);
        }
        if (upperCriticalThreshMask)
        {
            auto findUpper = findThreshold->second.find("CriticalHigh");
            if (findUpper == findThreshold->second.end())
            {
                return ipmi::responseInvalidFieldRequest();
            }
            thresholdsToSet.emplace_back("CriticalHigh", upperCritical,
                                         findThreshold->first);
        }
    }
    if (lowerNonCriticalThreshMask || upperNonCriticalThreshMask)
    {
        auto findThreshold =
            sensorMap.find("xyz.openbmc_project.Sensor.Threshold.Warning");
        if (findThreshold == sensorMap.end())
        {
            return ipmi::responseInvalidFieldRequest();
        }
        if (lowerNonCriticalThreshMask)
        {
            auto findLower = findThreshold->second.find("WarningLow");
            if (findLower == findThreshold->second.end())
            {
                return ipmi::responseInvalidFieldRequest();
            }
            thresholdsToSet.emplace_back("WarningLow", lowerNonCritical,
                                         findThreshold->first);
        }
        if (upperNonCriticalThreshMask)
        {
            auto findUpper = findThreshold->second.find("WarningHigh");
            if (findUpper == findThreshold->second.end())
            {
                return ipmi::responseInvalidFieldRequest();
            }
            thresholdsToSet.emplace_back("WarningHigh", upperNonCritical,
                                         findThreshold->first);
        }
    }
    for (const auto& property : thresholdsToSet)
    {
        double valueToSet = ((mValue * std::get<thresholdValue>(property)) +
                             (bValue * std::pow(10.0, bExp))) *
                            std::pow(10.0, rExp);
        setDbusProperty(
            *getSdBus(), sdrMapEntry[MapIndex].obj_connect, sdrMapEntry[MapIndex].obj_path, std::get<interface>(property),
            std::get<propertyName>(property), ipmi::Value(valueToSet));
    }
    return ipmi::responseSuccess();
}


void registerSdrCommands()
{
    // <Get SDR Repository Info>
    ipmi::registerHandler(ipmi::prioOdmBase, ipmi::netFnStorage,
                          ipmi::storage::cmdGetSdrRepositoryInfo,
                          ipmi::Privilege::User,
                          ipmiStorageGetSDRRepositoryInfo);

    // <Reserve SDR Repository>
    ipmi::registerHandler(ipmi::prioOdmBase, ipmi::netFnSensor,
                          ipmi::sensor_event::cmdReserveDeviceSdrRepository,
                          ipmi::Privilege::User, ipmiStorageReserveSDR);

    ipmi::registerHandler(ipmi::prioOdmBase, ipmi::netFnStorage,
                          ipmi::storage::cmdReserveSdrRepository,
                          ipmi::Privilege::User, ipmiStorageReserveSDR);

    // <Get Sdr>
    ipmi::registerHandler(ipmi::prioOdmBase, ipmi::netFnSensor,
                          ipmi::sensor_event::cmdGetDeviceSdr,
                          ipmi::Privilege::User, ipmiStorageGetSDR);

    ipmi::registerHandler(ipmi::prioOdmBase, ipmi::netFnStorage,
                          ipmi::storage::cmdGetSdr, ipmi::Privilege::User,
                          ipmiStorageGetSDR);

    // <Get Sensor Reading>
    ipmi::registerHandler(ipmi::prioOdmBase, ipmi::netFnSensor,
                          ipmi::sensor_event::cmdGetSensorReading,
                          ipmi::Privilege::User, ipmiGetSensorReading);

    // <Get Sensor Threshold>
    ipmi::registerHandler(ipmi::prioOdmBase, ipmi::netFnSensor,
                          ipmi::sensor_event::cmdGetSensorThreshold,
                          ipmi::Privilege::User, ipmiSenGetSensorThresholds);

    // <Set Sensor Threshold>
    ipmi::registerHandler(ipmi::prioOdmBase, ipmi::netFnSensor,
                          ipmi::sensor_event::cmdSetSensorThreshold,
                          ipmi::Privilege::Operator,
                          ipmiSenSetSensorThresholds);

    //TODO: Support more sdr/sensor commands

}

void sdrRepositoryDeviceSetup()
{
    int ret = sdrRepositoryInit ();
    if (ret != StatusOK) {
        std::cerr << "[Err] sdrRepositoryInit failed..., err code: " << ret << '\n';
    }

    registerSdrCommands ();
}

void sdrRepositoryDeviceExit()
{
    free(sdrMapEntry);
    free(sdrRawCachePtr);
}

} // namespace ipmi
