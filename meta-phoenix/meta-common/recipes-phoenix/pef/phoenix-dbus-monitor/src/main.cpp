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

#include "config.h"

#include "type.hpp"
#include "json_parser.hpp"
#include "phoenix_threshold_monitor.hpp"
#include "phoenix_discrete_monitor.hpp"


using namespace phoenix::dbus::monitor;
using json = nlohmann::json;


int main(int argc, char *argv[])
{
    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);

    /* Init json file data to PEF table */
    initJsonfile();

    /* Match threshold and discrete event monitor */
    DiscreteSensorMonitor discreteObj;
    ThresholdSensorMonitor thresholdObj;

    sdbusplus::bus::match::match discreteMatch = discreteObj.listenMatch(conn); 
    sdbusplus::bus::match::match thresholdMatch = thresholdObj.listenMatch(conn);

    io.run();

    return 0;
}

