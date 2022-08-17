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

#include <phosphor-logging/log.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/server.hpp>

#include "gpioMon.hpp"

using namespace phosphor::logging;

namespace gpio {

void GpioMonitor::initialGpioCapabilityInfo() {
    /* the gpioName does not include spaces already */
    gpioTable.Name = gpioName;

    /* set object path with GPIO name */
    gpioTable.ObjPath = GPIO_OBJPATH_PREFIX;
    gpioTable.ObjPath += gpioName;

    /* get the current GPIO value and set it into the table */
    gpioTable.Value = gpiod_line_get_value(gpioLine);

    /* get the GPIO offset and set it into the table */
    gpioTable.Offset = gpiod_line_offset(gpioLine);

    /* get the current GPIO direction and set it into the table */
    gpioTable.Direction = (gpiod_line_direction(gpioLine) == GPIOD_LINE_DIRECTION_INPUT
                            ? "input" : "output");

    /* set the trigger mode */
    gpioTable.TriggerMode = gpioTriggerMode + "-EDGE";
}

void GpioMonitor::gpioSetProperties() {
    gpioTable.gpioMonIntf->set_property("Value", (uint8_t)(gpioTable.Value));
    gpioTable.gpioMonIntf->set_property("Direction", (std::string)gpioTable.Direction);
}

void GpioMonitor::scheduleEventHandler() {
    gpioEventDescriptor.async_wait(
        boost::asio::posix::stream_descriptor::wait_read,
        [this](const boost::system::error_code& ec) {
            if (ec) {
                std::string msg = gpioLineMsg + "event handler error" +
                                  std::string(ec.message());
                log<level::ERR>(msg.c_str());
                return;
            }
            gpioEventHandler();
        });
}

void GpioMonitor::gpioEventHandler() {
    gpiod_line_event gpioLineEvent;
    std::string logMessage;
    bool is_catch_rising_edge = ((gpioTriggerMode == "BOTH") || (gpioTriggerMode == "RISING"));
    bool is_catch_falling_edge = ((gpioTriggerMode == "BOTH") || (gpioTriggerMode == "FALLING"));
    bool is_log_event = false;

    if (gpiod_line_event_read_fd(gpioEventDescriptor.native_handle(),
                                 &gpioLineEvent) < 0)
    {
        log<level::ERR>("Failed to read gpioLineEvent from fd",
                        entry("GPIO_LINE=%s", gpioLineMsg.c_str()));
        return;
    }

    if ((is_catch_rising_edge == true) &&
        (gpioLineEvent.event_type == GPIOD_LINE_EVENT_RISING_EDGE))
    {
        logMessage = gpioLineMsg + " Rising Edge Triggered";
        is_log_event = true;
    }
    else if ((is_catch_falling_edge == true) &&
        (gpioLineEvent.event_type == GPIOD_LINE_EVENT_FALLING_EDGE))
    {
        logMessage = gpioLineMsg + " Falling Edge Triggered";
        is_log_event = true;
    }

    if (is_log_event == true) {
        /* also able to add event for IPMI SEL in here */
        log<level::INFO>(logMessage.c_str());
    }

    /* sync statuses to gpioTable and dbus */
    gpioTable.Value =  gpiod_line_get_value(gpioLine);
    gpioTable.Direction = (gpiod_line_direction(gpioLine) == GPIOD_LINE_DIRECTION_INPUT
                            ? "input" : "output");
    gpioSetProperties();

    /* if not required to continue monitoring then return */
    if (!continueAfterEvent) {
        return;
    }

    /* Schedule a wait event */
    scheduleEventHandler();
}

int GpioMonitor::requestGPIOEvents() {
    /* Request an event to monitor for respected gpio line */
    if (gpiod_line_request(gpioLine, &gpioConfig, 0) < 0) {
        log<level::ERR>("Failed to request gpioLineEvent",
                        entry("GPIO_LINE=%s", gpioLineMsg.c_str()));
        return -1;
    }

    int gpioLineFd = gpiod_line_event_get_fd(gpioLine);
    if (gpioLineFd < 0) {
        log<level::ERR>("Failed to get fd for gpioLineEvent",
                        entry("GPIO_LINE=%s", gpioLineMsg.c_str()));
        return -1;
    }

    std::string logMsg = gpioLineMsg + " monitoring started";
    log<level::INFO>(logMsg.c_str());

    /* Assign line fd to descriptor for monitoring */
    gpioEventDescriptor.assign(gpioLineFd);

    /* Initial all GPIO information */
    initialGpioCapabilityInfo();

    /* Schedule a wait event */
    scheduleEventHandler();

    return 0;
}
} // namespace gpio
