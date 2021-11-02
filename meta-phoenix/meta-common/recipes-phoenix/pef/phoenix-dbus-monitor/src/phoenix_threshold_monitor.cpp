/*
 * Copyright (c) 2021 Phoenix Technologies Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "type.hpp"
#include "json_parser.hpp"
#include "phoenix_threshold_monitor.hpp"
#include "phoenix_util.hpp"
#include "sensorutils.hpp"

namespace phoenix
{
namespace dbus
{
namespace monitor
{

std::vector<uint8_t> ThresholdSensorMonitor::processEventData(std::string event
    , std::string interface
    , double assertValue
    , sdbusplus::message::message& m)
{
    DPRINT("%s!!\n", __func__);
    std::vector<uint8_t> ed(selEvtDataMaxSize, UNSPECIFIED);

    /* Event data 1 */
    if (event == "CriticalAlarmLow")
    {
        ed[0] = lowerCritGoingLow;
    }
    else if (event == "WarningAlarmLow")
    {
        ed[0] = lowerNonCritGoingLow;
    }
    else if (event == "WarningAlarmHigh")
    {
        ed[0] = upperNonCritGoingHigh;
    }
    else if (event == "CriticalAlarmHigh")
    {
        ed[0] = upperCritGoingHigh;
    }
    // Indicate that bytes 2 and 3 are threshold sensor trigger values
    ed[0] |= (triggerReadingByte2 | triggerReadingByte3);

    DPRINT("%s, ed[0] = %x!!\n", __func__, ed[0]);

    /* Event data 2 */
    // Get the sensor reading to put in the event data
    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);

    sdbusplus::message::message getSensorValue =
        conn->new_method_call(m.get_sender()
                            , m.get_path()
                            , property_interface_name
                            , "GetAll");
    getSensorValue.append(sensor_value_interface_name);
    boost::container::flat_map<std::string, std::variant<double, int64_t>>
        sensorValue;
    try
    {
        sdbusplus::message::message getSensorValueResp =
            conn->call(getSensorValue);
        getSensorValueResp.read(sensorValue);
    }
    catch (sdbusplus::exception_t&)
    {
        std::cerr << "error getting sensor value from " << m.get_path()
                  << "\n";
        ed[1] = UNSPECIFIED;
    }

    double max = 0;
    auto findMax = sensorValue.find("MaxValue");

    if (findMax != sensorValue.end())
    {
        max = std::visit(ipmi::VariantToDoubleVisitor(), findMax->second);
    }

    double min = 0;
    auto findMin = sensorValue.find("MinValue");

    if (findMin != sensorValue.end())
    {
        min = std::visit(ipmi::VariantToDoubleVisitor(), findMin->second);
    }

    try
    {
        ed[1] = ipmi::getScaledIPMIValue(assertValue, max, min);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
        ed[1] = UNSPECIFIED;
    }

    DPRINT("%s, ed[1] = %x!!\n", __func__, ed[1]);

    /* Event data 3 */
    // Get the threshold value to put in the event data
    // Get the threshold parameter by removing the "Alarm" text from the
    // event string
    std::string alarm("Alarm");
    if (std::string::size_type pos = event.find(alarm);
        pos != std::string::npos)
    {
        event.erase(pos, alarm.length());
    }

    sdbusplus::message::message getThreshold =
        conn->new_method_call(m.get_sender()
                            , m.get_path()
                            , property_interface_name
                            , "Get");
    getThreshold.append(interface, event);

    std::variant<double, int64_t> thresholdValue;

    try
    {
        sdbusplus::message::message getThresholdResp =
            conn->call(getThreshold);
        getThresholdResp.read(thresholdValue);
    }
    catch (sdbusplus::exception_t&)
    {
        std::cerr << "error getting sensor threshold from "
                  << m.get_path() << "\n";
        ed[2] = UNSPECIFIED;
    }

    double thresholdVal =
        std::visit(ipmi::VariantToDoubleVisitor(), thresholdValue);

    double scale = 0;
    auto findScale = sensorValue.find("Scale");

    if (findScale != sensorValue.end())
    {
        scale = std::visit(ipmi::VariantToDoubleVisitor(), findScale->second);
        thresholdVal *= std::pow(10, scale);
    }

    try
    {
        ed[2] = ipmi::getScaledIPMIValue(thresholdVal, max, min);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
        ed[2] = UNSPECIFIED;
    }

    DPRINT("%s, ed[2] = %x!!\n", __func__, ed[2]);


    return ed;
}

void ThresholdSensorMonitor::getMsgData(sdbusplus::message::message& m)
{

    DPRINT("%s!!\n", __func__);

    std::string sensorName;
    std::string thresholdInterface;
    std::string event;
    bool assert;
    double assertValue;
    std::vector<uint8_t> eventData(selEvtDataMaxSize, UNSPECIFIED);

    try
    {
        m.read(sensorName, thresholdInterface, event, assert,
            assertValue);
    }
    catch (sdbusplus::exception_t&)
    {
        std::cerr << "error getting assert signal data from "
                  << m.get_path() << "\n";
        return;
    }

    eventData = ThresholdSensorMonitor::processEventData(event, thresholdInterface , assertValue, m);

    DPRINT("%s, %d, Name: %s, Interface: %s, Event: %s, Assert: %d, AssertVal: %f"
           ", ed[0]: %x, ed[1]: %x, ed[2]: %x\n"
        , __func__, __LINE__
        , sensorName.c_str()
        , thresholdInterface.c_str()
        , event.c_str()
        , assert
        , assertValue
        , eventData[0]
        , eventData[1]
        , eventData[2]);
    DPRINT("%s!!\n", m.get_path());

    if( !checkEventfilterTable(eventData) )
    {   
        DPRINT("Sensor event (%s) not match!!\n", sensorName.c_str());
        return;
    }
}

sdbusplus::bus::match::match
    ThresholdSensorMonitor::listenMatch(std::shared_ptr<sdbusplus::asio::connection> conn)
{
    sdbusplus::bus::match::match thresholdMacth(
        static_cast<sdbusplus::bus::bus&>(*conn),
        sdbusplus::bus::match::rules::type::signal()
        + sdbusplus::bus::match::rules::member("ThresholdAsserted"),
        [&](sdbusplus::message::message& m) { 
            if( global_pef_table.control & PEF_CTL_EN ){
                ThresholdSensorMonitor::getMsgData(m); 
            }
        });

    return thresholdMacth;
}

}//monitor
}//dbus
}//phoenix
