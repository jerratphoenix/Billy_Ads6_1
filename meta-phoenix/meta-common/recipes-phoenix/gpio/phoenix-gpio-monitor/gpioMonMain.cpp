/**
 * Copyright © 2021 Phoenix Technologies Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>

#include <CLI/CLI.hpp>
#include <boost/asio/io_service.hpp>
#include <nlohmann/json.hpp>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/server.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/asio.hpp>

#include "gpioMon.hpp"

using namespace phosphor::logging;

/**
 * @brief  Initial GPIO dbus
 *
 * @param  io       io context
 * @param  gpios    gpio class
 */
void registerGpioDbusService(boost::asio::io_context& io, std::vector<std::unique_ptr<gpio::GpioMonitor>>& gpios) {
    /* Init Service and Object */
    std::string logMsg = "Register GPIO Dbus Service";
    log<level::INFO>(logMsg.c_str());

    auto conn = std::make_shared<sdbusplus::asio::connection>(io);
    conn->request_name(GPIO_SERVICE);
    auto server = sdbusplus::asio::object_server(conn);

    for (unsigned i = 0; i < gpios.size() ; i++) {
        if (gpios[i]->gpioTable.ObjPath.size() <= 0) {
            continue;
        }

        logMsg = "Object Path: " + gpios[i]->gpioTable.ObjPath;
        log<level::INFO>(logMsg.c_str());

        gpios[i]->gpioTable.gpioMonIntf = server.add_interface(
                                            gpios[i]->gpioTable.ObjPath,
                                            GPIO_INTERFACE);
        gpios[i]->gpioTable.gpioMonIntf->register_property("Name",
                (std::string)gpios[i]->gpioTable.Name,
                sdbusplus::asio::PropertyPermission::readOnly);
        gpios[i]->gpioTable.gpioMonIntf->register_property("Value",
                (uint8_t)gpios[i]->gpioTable.Value,
                sdbusplus::asio::PropertyPermission::readWrite);
        gpios[i]->gpioTable.gpioMonIntf->register_property("Offset",
                (uint32_t)gpios[i]->gpioTable.Offset,
                sdbusplus::asio::PropertyPermission::readOnly);
        gpios[i]->gpioTable.gpioMonIntf->register_property("Direction",
                (std::string)gpios[i]->gpioTable.Direction,
                sdbusplus::asio::PropertyPermission::readWrite);
        gpios[i]->gpioTable.gpioMonIntf->register_property("TriggerMode",
                (std::string)gpios[i]->gpioTable.TriggerMode,
                sdbusplus::asio::PropertyPermission::readWrite);

        gpios[i]->gpioTable.gpioMonIntf->initialize();
    }
}

/**
 * @brief  Replace string space with "_"
 *
 * @param  strInput    input string
 *
 * @return - string without space
 */
std::string replaceStringSpace(std::string strInput) {
    std::string strNoSpace;

    strNoSpace.assign(strInput.c_str(), strlen(strInput.c_str()));
    strNoSpace = boost::replace_all_copy(strNoSpace, " ", "_");

    return strNoSpace;
}

int main(int argc, char** argv) {
    boost::asio::io_context io;
    std::string gpioFileName;
    nlohmann::json gpioMonObj;
    std::vector<std::unique_ptr<gpio::GpioMonitor>> gpios;

    CLI::App app{"Monitor GPIO line for requested state change"};

    /* Add an input option */
    app.add_option("-c,--config", gpioFileName, "Name of config json file")
        ->required()
        ->check(CLI::ExistingFile);

    /* Parse input parameter */
    try {
        app.parse(argc, argv);
    } catch (const CLI::Error& e) {
        return app.exit(e);
    }

    /* Get list of gpio config details from json file */
    std::ifstream file(gpioFileName);
    if (!file) {
        log<level::ERR>("GPIO monitor config file not found",
                        entry("GPIO_MON_FILE=%s", gpioFileName.c_str()));
        return -1;
    }

    file >> gpioMonObj;
    file.close();

    for (auto& obj : gpioMonObj) {
        /* GPIO Line message */
        std::string lineMsg = "GPIO Line ";

        /* GPIO line */
        gpiod_line* line = NULL;

        /* Log message string */
        std::string errMsg;

        /* GPIO line configuration, default to monitor both edge */
        struct gpiod_line_request_config config {
            "gpio-monitor", GPIOD_LINE_REQUEST_EVENT_BOTH_EDGES, 0
        };

        /* flag to monitor */
        bool flag = false;

        /* GPIO name without spaces */
        std::string gpioName;

        if (obj.find("LineName") == obj.end()) {
            /* If there is no line Name defined then gpio num nd chip
             * id must be defined. GpioNum is integer mapping to the
             * GPIO key configured by the kernel
             */
            if (obj.find("GpioNum") == obj.end() ||
                obj.find("ChipId") == obj.end())
            {
                log<level::ERR>(
                    "Failed to find line name or gpio number",
                    entry("GPIO_JSON_FILE_NAME=%s", gpioFileName.c_str()));
                return -1;
            }

            std::string chipIdStr = obj["ChipId"];
            int gpioNum = obj["GpioNum"];

            /* Get the GPIO line */
            if (obj.find("Name") == obj.end()) {
                gpioName = "Line" + std::to_string(gpioNum);
                lineMsg += std::to_string(gpioNum);
            } else {
                std::string name = obj["Name"];
                gpioName = replaceStringSpace(name);
                lineMsg += gpioName;
            }
            line = gpiod_line_get(chipIdStr.c_str(), gpioNum);
        } else {
            /* Find the GPIO line */
            std::string lineName = obj["LineName"];
            gpioName = replaceStringSpace(lineName);
            lineMsg += gpioName;
            line = gpiod_line_find(lineName.c_str());
        }

        if (line == NULL) {
            errMsg = "Failed to find the " + lineMsg;
            log<level::ERR>(errMsg.c_str());
            return -1;
        }

        /* Get event to be monitored, if it is not defined then
         * Both rising falling edge will be monitored.
         *
         * Set request type to "BOTH" for catching state change of value,
         * the "EnentMon" setting is for logging events.
         */
        std::string eventStr = "BOTH";
        if (obj.find("EventMon") != obj.end()) {
            eventStr = obj["EventMon"];
            config.request_type = GPIOD_LINE_REQUEST_EVENT_BOTH_EDGES;
        }

        /* Get flag if monitoring needs to continue after first event */
        if (obj.find("Continue") != obj.end()) {
            flag = obj["Continue"];
        }

        /* Create a monitor object and let it do all the rest */
        gpios.push_back(std::make_unique<gpio::GpioMonitor>(
            line, config, gpioName, io, lineMsg, eventStr, flag));
    }

    /* Enable dbus service for monitoring GPIOs */
    registerGpioDbusService(io, gpios);

    io.run();

    return 0;
}
