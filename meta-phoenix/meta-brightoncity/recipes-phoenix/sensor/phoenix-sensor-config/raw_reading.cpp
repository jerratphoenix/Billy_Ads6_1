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

#include "phoenix-dbus-sensors.hpp"
#include "sensorapi.hpp"

#include <stdlib.h>
#include <string.h>

#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <functional>
#include <gpiod.hpp>
#include <iostream>
#include <random>
#include <stdexcept>

// User may implement their own "is_power_on" function to get power state.
// Or use our api_get_power_on to get from OpenBMC chassis dbus
bool is_power_on(void)
{
    bool system_power_good = api_get_power_on();

    return system_power_good;
}

int32_t get_system_crash(double* reading)
{
    bool cpu_err = false;
    int gpio_caterr, gpio_err0, gpio_err1, gpio_err2;

    // Read from GPIO
    api_get_gpio("CPU_CATERR", &gpio_caterr, gpiod::line::ACTIVE_LOW);
    api_get_gpio("CPU_ERR0", &gpio_err0, gpiod::line::ACTIVE_LOW);
    api_get_gpio("CPU_ERR1", &gpio_err1, gpiod::line::ACTIVE_LOW);
    api_get_gpio("CPU_ERR2", &gpio_err2, gpiod::line::ACTIVE_LOW);

    if (gpio_caterr == 1 || gpio_err0 == 1 || gpio_err1 == 1 || gpio_err2 == 1)
    {
        cpu_err = true;
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
    return SENSOR_STATUS::NORMAL;
}

