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

#include "sensorapi.hpp"

#include <cstdint>
#include <string>
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <functional>
#include <gpiod.hpp>
#include <iostream>
#include <random>
#include <stdexcept>

#include <sdbusplus/bus.hpp>
#include <sdbusplus/message.hpp>

// User may implement their own "is_power_on" function to get power state.
// Or use our api_get_power_on to get from OpenBMC chassis dbus
//bool is_power_on(void)
//{
//    bool system_power_good = api_get_power_on();
//
//    return system_power_good;
//}

// Use D-Bus chassis power state for accurate detection
bool is_power_on(void)
{
//    static constexpr auto chassisService = "xyz.openbmc_project.State.Chassis";
//    static constexpr auto chassisPath = "/xyz/openbmc_project/state/chassis0";
//    static constexpr auto iface = "xyz.openbmc_project.State.Chassis";
//    static constexpr auto prop = "CurrentPowerState";
//
//    try
//    {
//        auto bus = sdbusplus::bus::new_default();
//        auto method = bus.new_method_call(chassisService, chassisPath,
//                                          "org.freedesktop.DBus.Properties", "Get");
//        method.append(iface, prop);
//
//        sdbusplus::message::variant<std::string> value;
//        bus.call(method).read(value);
//        std::string state = std::get<std::string>(value);
//
//        // Typical values:
//        // "xyz.openbmc_project.State.Chassis.PowerState.On"
//        // "xyz.openbmc_project.State.Chassis.PowerState.Off"
//        if (state.find("On") != std::string::npos)
//        {
//            return true;
//        }
//    }
//    catch (const std::exception& e)
//    {
//        std::cerr << "Failed to read chassis power state: " << e.what() << std::endl;
//    }
//
//    return false; // default: treat as power off if cannot read

    try
    {
        static constexpr auto chassisService = "xyz.openbmc_project.State.Chassis";
        static constexpr auto chassisPath = "/xyz/openbmc_project/state/chassis0";
        static constexpr auto iface = "xyz.openbmc_project.State.Chassis";
        static constexpr auto prop = "CurrentPowerState";

        auto bus = sdbusplus::bus::new_default();
        auto msg = bus.new_method_call(
            chassisService, chassisPath, "org.freedesktop.DBus.Properties", "Get");

        msg.append(iface, prop);

        std::string value; // just std::string
        bus.call(msg).read(value); // read directly into std::string

        return value.find("On") != std::string::npos;
    }
    catch (...)
    {
        return false;
    }
}

// Let user could implement initial code, before first sensor get raw reading
void initial_before_raw_reading(void)
{
    // Configure Host CPU GPIOs
    // If does not want sensor module to monitor that GPIO, just remove it.
    // As in case of enable host-error-monitor service, that GPIO line can not be requested again.
    api_config_cpu_caterr_gpio("CPU_CATERR", gpiod::line::ACTIVE_LOW);
    api_config_cpu_err0_gpio("CPU_ERR0", gpiod::line::ACTIVE_LOW);
    api_config_cpu_err1_gpio("CPU_ERR1", gpiod::line::ACTIVE_LOW);
    api_config_cpu_err2_gpio("CPU_ERR2", gpiod::line::ACTIVE_LOW);
    api_config_cpu1_present_gpio("CPU1_PRESENCE", gpiod::line::ACTIVE_LOW);
    api_config_cpu2_present_gpio("CPU2_PRESENCE", gpiod::line::ACTIVE_LOW);

    // Add other inital code here
}

int32_t get_system_crash(double* reading)
{
    bool cpu_err = false;

    if (is_power_on() == true)
    {
        if (api_is_cpu_caterr_gpio_assert() == true ||
            api_is_cpu_err0_gpio_assert() == true ||
            api_is_cpu_err1_gpio_assert() == true ||
            api_is_cpu_err2_gpio_assert() == true)
        {
            cpu_err = true;
        }
    }

    if (cpu_err == true)
    {
        *reading = BIT(1); // offset 01h: State Asserted
    }
    else
    {
        *reading = 0;
    }

    return SENSOR_STATUS::NORMAL;
}

int32_t get_sys_pwr_state(double* reading)
{
    if (is_power_on())
    {
        *reading = BIT(0); // SensorType 22h, offset 00h: S0 / G0 "working"
    }
    else
    {
        *reading = BIT(5); // SensorType 22h, offset 05h: S5 / G2 "soft-off"
    }

    return SENSOR_STATUS::NORMAL;
}

int32_t get_bmc_reset(double* reading)
{
    return api_sensor_bmc_reset(reading);
}

int32_t get_bmc_fw_update(double* reading)
{
    return api_sensor_bmc_fw_update(reading);
}

int32_t get_ipmi_sel(double* reading)
{
    return api_sensor_sel_clear(reading);
}

int32_t get_bmc_factory_reset(double* reading)
{
    return api_sensor_bmc_factory_reset(reading);
}

int32_t get_ipmi_wdt(double* reading)
{
    return api_sensor_watchdog2(reading);
}

int32_t get_sel_time(double* reading)
{
    return api_sensor_sel(reading);
}

int32_t get_ipmi_pef(double* reading)
{
    return api_sensor_pef(reading);
}

int32_t get_bmc_reboot (double *reading)
{
    *reading = BIT(1); // offset 01h: State Asserted

    return SENSOR_STATUS::NORMAL;
}
