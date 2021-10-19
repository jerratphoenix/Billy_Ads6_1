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
#include "phoenix_discrete_monitor.hpp"
#include "phoenix_util.hpp"


namespace phoenix
{
namespace dbus
{
namespace monitor
{

void DiscreteSensorMonitor::getMsgData(sdbusplus::message::message& m)
{
    DPRINT("%s!!\n", __func__);

    // Get the event type and assertion details from the message
    std::string interface;
    boost::container::flat_map<std::string, std::variant<unsigned int>> properties;
    std::vector<uint8_t> eventData(selEvtDataMaxSize, UNSPECIFIED);

    try
    {
        m.read(interface, properties);
    }
    catch (sdbusplus::exception_t&)
    {
        std::cerr << "error getting assert signal data from "
                  << m.get_path() << "\n";
        return;
    }

    std::string sensorName = getSensorNameFromPath(m.get_path());

    std::string event = properties.begin()->first;

    unsigned int *pval = std::get_if<unsigned int>(&properties.begin()->second);
    if (!pval)
    {
        std::cerr << "discrete offset has an invalid type\n";
        return;
    }
    unsigned int offset = *pval;

    DPRINT ("%s, %d, Interface: %s, Event: %s\n", __func__, __LINE__
            , interface.c_str()
            , event.c_str());

    if ( offset > 14 )
    {
        //Ignore events with invalid offsets (which occur at init time when properties are created)
        return;
    }

    // Grab the offset
    eventData[0] = static_cast<uint8_t>(offset);

    DPRINT ("%s, %d, EventData[0]: %x, EventData[1]: %x, EventData[2]: %x\n"
            , __func__, __LINE__
            , eventData[0]
            , eventData[1]
            , eventData[2]);

   if( !checkEventfilterTable(eventData) )
   {
       DPRINT("Sensor event (%s) not match!!\n", sensorName.c_str());
       return;
   }
}

sdbusplus::bus::match::match
    DiscreteSensorMonitor::listenMatch(std::shared_ptr<sdbusplus::asio::connection> conn)
{
    sdbusplus::bus::match::match discreteMacth(
        static_cast<sdbusplus::bus::bus&>(*conn),
        sdbusplus::bus::match::rules::type::signal() 
        + sdbusplus::bus::match::rules::interface(property_interface_name)
        + sdbusplus::bus::match::rules::member("PropertiesChanged")
        + sdbusplus::bus::match::rules::arg0namespace(sensor_discrete_interface_name),
        [&](sdbusplus::message::message& m) { 
            if( global_pef_table.control & PEF_CTL_EN ){
                DiscreteSensorMonitor::getMsgData(m);
            }
        });

    return discreteMacth;
}


}//monitor
}//dbus
}//phoenix
