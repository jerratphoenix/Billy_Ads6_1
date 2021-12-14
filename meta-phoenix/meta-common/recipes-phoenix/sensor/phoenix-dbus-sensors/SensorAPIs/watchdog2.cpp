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

#include "debug.hpp"
#include "phoenix-dbus-sensors.hpp"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/asio.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/server.hpp>
#include <string>
#include <string_view>
#include <thread>
#include <variant>

enum class watchdogEventOffsets : uint8_t
{
    noAction = 0x00,
    hardReset = 0x01,
    powerDown = 0x02,
    powerCycle = 0x03,
};

enum class watchdogTimerUseOffsets : uint8_t
{
    reserved = 0x00,
    BIOSFRB2 = 0x01,
    BIOSPOST = 0x02,
    OSLoad = 0x03,
    SMSOS = 0x04,
    OEM = 0x05,
    unspecified = 0x0f,
};

enum class watchdogInterruptTypeOffsets : uint8_t
{
    none = 0x00,
    SMI = 0x01,
    NMI = 0x02,
    messageInterrupt = 0x03,
    unspecified = 0x0f,
};

static constexpr const uint8_t wdtNologBit = (1 << 7);
static constexpr int interruptTypeBits = 4;

static std::string watchdog2_sensor_path;
static double watchdog2_sensor_reading = 0;

inline static void sendWatchdogEventLog(
    std::shared_ptr<sdbusplus::asio::connection> conn,
    sdbusplus::message::message& msg,
    bool assert,
    std::optional<std::string_view> expireAction = std::nullopt)
{
    std::vector<uint8_t> eventData(3, 0xFF);

    sdbusplus::message::message getWatchdogStatus =
        conn->new_method_call(msg.get_sender(),
                              msg.get_path(),
                              "org.freedesktop.DBus.Properties",
                              "GetAll");
    getWatchdogStatus.append("xyz.openbmc_project.State.Watchdog");
    boost::container::flat_map<std::string,
                               std::variant<std::string, uint64_t, bool>>
        watchdogStatus;

    try
    {
        sdbusplus::message::message getWatchdogStatusResp =
            conn->call(getWatchdogStatus);
        getWatchdogStatusResp.read(watchdogStatus);
    }
    catch (const sdbusplus::exception_t&)
    {
        std::cerr << "error getting watchdog status from " << msg.get_path()
                  << "\n";
        return;
    }

    if (!expireAction)
    {
        auto getExpireAction = watchdogStatus.find("ExpireAction");
        if (getExpireAction != watchdogStatus.end())
        {
            expireAction = std::get<std::string>(getExpireAction->second);
            expireAction->remove_prefix(std::min(
                expireAction->find_last_of(".") + 1, expireAction->size()));
        }
    }

    if (*expireAction == "HardReset")
    {
        eventData[0] = static_cast<uint8_t>(watchdogEventOffsets::hardReset);
    }
    else if (*expireAction == "PowerOff")
    {
        eventData[0] = static_cast<uint8_t>(watchdogEventOffsets::powerDown);
    }
    else if (*expireAction == "PowerCycle")
    {
        eventData[0] = static_cast<uint8_t>(watchdogEventOffsets::powerCycle);
    }
    else if (*expireAction == "None")
    {
        eventData[0] = static_cast<uint8_t>(watchdogEventOffsets::noAction);
    }
    
    watchdog2_sensor_reading = eventData[0];

    auto getPreTimeoutInterrupt = watchdogStatus.find("PreTimeoutInterrupt");
    std::string_view preTimeoutInterrupt;
    if (getPreTimeoutInterrupt != watchdogStatus.end())
    {
        preTimeoutInterrupt =
            std::get<std::string>(getPreTimeoutInterrupt->second);
        preTimeoutInterrupt.remove_prefix(
            std::min(preTimeoutInterrupt.find_last_of(".") + 1,
                     preTimeoutInterrupt.size()));
    }
    if (preTimeoutInterrupt == "None")
    {
        eventData[1] &=
            (static_cast<uint8_t>(watchdogInterruptTypeOffsets::none)
             << interruptTypeBits);
    }
    else if (preTimeoutInterrupt == "SMI")
    {
        eventData[1] &= (static_cast<uint8_t>(watchdogInterruptTypeOffsets::SMI)
                         << interruptTypeBits);
    }
    else if (preTimeoutInterrupt == "NMI")
    {
        eventData[1] &= (static_cast<uint8_t>(watchdogInterruptTypeOffsets::NMI)
                         << interruptTypeBits);
    }
    else if (preTimeoutInterrupt == "MI")
    {
        eventData[1] &= (static_cast<uint8_t>(
                             watchdogInterruptTypeOffsets::messageInterrupt)
                         << interruptTypeBits);
    }

    auto getCurrentTimerUse = watchdogStatus.find("CurrentTimerUse");
    std::string_view currentTimerUse;
    if (getCurrentTimerUse != watchdogStatus.end())
    {
        currentTimerUse = std::get<std::string>(getCurrentTimerUse->second);
        currentTimerUse.remove_prefix(std::min(
            currentTimerUse.find_last_of(".") + 1, currentTimerUse.size()));
    }
    if (currentTimerUse == "BIOSFRB2")
    {
        eventData[1] |= static_cast<uint8_t>(watchdogTimerUseOffsets::BIOSFRB2);
    }
    else if (currentTimerUse == "BIOSPOST")
    {
        eventData[1] |= static_cast<uint8_t>(watchdogTimerUseOffsets::BIOSPOST);
    }
    else if (currentTimerUse == "OSLoad")
    {
        eventData[1] |= static_cast<uint8_t>(watchdogTimerUseOffsets::OSLoad);
    }
    else if (currentTimerUse == "SMSOS")
    {
        eventData[1] |= static_cast<uint8_t>(watchdogTimerUseOffsets::SMSOS);
    }
    else if (currentTimerUse == "OEM")
    {
        eventData[1] |= static_cast<uint8_t>(watchdogTimerUseOffsets::OEM);
    }
    else
    {
        eventData[1] |=
            static_cast<uint8_t>(watchdogTimerUseOffsets::unspecified);
    }

    auto getWatchdogInterval = watchdogStatus.find("Interval");
    uint64_t watchdogInterval = 0;
    if (getWatchdogInterval != watchdogStatus.end())
    {
        watchdogInterval = std::get<uint64_t>(getWatchdogInterval->second);
    }

    // get watchdog status properties
    bool wdt_nolog;
    sdbusplus::bus::bus bus = sdbusplus::bus::new_default();
    uint8_t netFn = 0x06;
    uint8_t lun = 0x00;
    uint8_t cmd = 0x25;
    std::vector<uint8_t> commandData;
    std::map<std::string, std::variant<int>> options;

    auto ipmiCall = bus.new_method_call("xyz.openbmc_project.Ipmi.Host",
                                        "/xyz/openbmc_project/Ipmi",
                                        "xyz.openbmc_project.Ipmi.Server",
                                        "execute");
    ipmiCall.append(netFn, lun, cmd, commandData, options);
    std::tuple<uint8_t, uint8_t, uint8_t, uint8_t, std::vector<uint8_t>> rsp;
    auto ipmiReply = bus.call(ipmiCall);
    ipmiReply.read(rsp);
    auto& [rnetFn, rlun, rcmd, cc, responseData] = rsp;

    wdt_nolog = responseData[0] & wdtNologBit;

    // Set Watchdog Timer byte1[7]-1b=don't log
    if (!wdt_nolog)
    {
        add_ipmi_std_sel_entry("IPMI Watchdog Event",
                               watchdog2_sensor_path,
                               eventData,
                               true,
                               0x20);
    }
}

sdbusplus::bus::match::match register_watchdog2_event_handler(
    std::shared_ptr<sdbusplus::asio::connection> conn)
{
    FUNC_START();

    auto watchdogEventMatcherCallback = 
        [conn](sdbusplus::message::message& msg) {
        if (watchdog2_sensor_path.empty())
        {
            // Watchdog sensor path is NULL, not do anything.
            return;
        }

        std::string expiredAction;
        msg.read(expiredAction);

        std::string_view action = expiredAction;
        action.remove_prefix(
            std::min(action.find_last_of(".") + 1, action.size()));

        sendWatchdogEventLog(conn, msg, true, action);
    };

    sdbusplus::bus::match::match watchdogEventMatcher(
        static_cast<sdbusplus::bus::bus&>(*conn),
        "type='signal',interface='xyz.openbmc_project.Watchdog',"
        "member='Timeout'",
        std::move(watchdogEventMatcherCallback));

    FUNC_END();

    return watchdogEventMatcher;
}

void api_probe_watchdog2_sensor_path(std::string sensor_path)
{
    FUNC_START();

    watchdog2_sensor_path = sensor_path;

    FUNC_END();
}

int32_t api_sensor_watchdog2(double* reading)
{
    static bool watchdog2_sensor_probe = false;
    
    *reading = watchdog2_sensor_reading;

    // Sensor already probe, no need to handle anything for IPMI watchdog sensor
    if (watchdog2_sensor_probe == true)
    {
        return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
    }

    api_probe_watchdog2_sensor_path(get_processing_sensor_path());
    watchdog2_sensor_probe = true;

    // Notify sensor daemon we already handled event in here.
    return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
}

// TODO: Create more API here
