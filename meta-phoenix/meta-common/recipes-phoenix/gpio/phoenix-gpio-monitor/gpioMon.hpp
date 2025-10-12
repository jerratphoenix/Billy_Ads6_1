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

#pragma once

#include <gpiod.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/algorithm/string.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/server.hpp>

/* GPIO dbus service */
constexpr auto GPIO_SERVICE = "xyz.openbmc_project.GpioMonitor";
constexpr auto GPIO_OBJPATH_PREFIX = "/xyz/openbmc_project/GpioMonitor/";
constexpr auto GPIO_INTERFACE = "xyz.openbmc_project.GpioMonitor";

namespace gpio {

/** @class GpioTable
 *  @brief Responsible for syncing GPIO information to dbus.
 */
class GpioTable {
  public:
    std::shared_ptr<sdbusplus::asio::dbus_interface> gpioMonIntf;
    uint8_t Value;
    uint32_t Offset;
    std::string Name;
    std::string ObjPath;
    std::string Direction;
    std::string TriggerMode;

    GpioTable() {
        Value = 0;
        Offset = 0;
        Name = "";
        ObjPath = "";
        Direction = "";
        TriggerMode = "";
    }
};

/** @class GpioMonitor
 *  @brief Responsible for catching GPIO state change conditions.
 */
class GpioMonitor {
  public:
    GpioMonitor() = delete;
    ~GpioMonitor() = default;
    GpioMonitor(const GpioMonitor&) = delete;
    GpioMonitor& operator=(const GpioMonitor&) = delete;
    GpioMonitor(GpioMonitor&&) = delete;
    GpioMonitor& operator=(GpioMonitor&&) = delete;

    /** @brief Constructs GpioMonitor object.
     *
     *  @param[in] line        - GPIO line from libgpiod
     *  @param[in] config      - configuration of line with event
     *  @param[in] name        - GPIO name
     *  @param[in] io          - io context
     *  @param[in] lineMsg     - GPIO line message to be used for log
     *  @param[in] triggerMode - event trigger mode
     *  @param[in] continueRun - Whether to continue after event occur
     */
    GpioMonitor(gpiod_line* line, gpiod_line_request_config& config,
                const std::string& name, boost::asio::io_context& io,
                const std::string& lineMsg, const std::string& triggerMode,
                bool continueRun) :
        gpioLine(line), gpioConfig(config),
        gpioName(name), gpioEventDescriptor(io),
        gpioLineMsg(lineMsg), gpioTriggerMode(triggerMode),
        continueAfterEvent(continueRun)
    {
        this->gpioTable = GpioTable();
        requestGPIOEvents();
    };

    GpioTable gpioTable;

  private:
    /** @brief GPIO line */
    gpiod_line* gpioLine;

    /** @brief GPIO line configuration */
    gpiod_line_request_config gpioConfig;

    /** @brief GPIO name */
    std::string gpioName;

    /** @brief GPIO event descriptor */
    boost::asio::posix::stream_descriptor gpioEventDescriptor;

    /** @brief GPIO line name message */
    std::string gpioLineMsg;

    /** @brief event trigger mode */
    std::string gpioTriggerMode;

    /** @brief If the monitor should continue after event */
    bool continueAfterEvent;

    /** @brief register handler for gpio event
     *
     *  @return  - 0 on success and -1 otherwise
     */
    int requestGPIOEvents();

    /** @brief Schedule an event handler for GPIO event to trigger */
    void scheduleEventHandler();

    /** @brief Handle the GPIO event and starts configured target */
    void gpioEventHandler();

    /** @brief Initial all GPIO information */
    void initialGpioCapabilityInfo();

    /** @brief Set GPIO properties to dbus */
    void gpioSetProperties();
};

} // namespace gpio
