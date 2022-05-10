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

// User may implement their own "is_power_on" function to get power state.
// Or use our api_get_power_on to get from OpenBMC chassis dbus
bool is_power_on(void)
{
    bool system_power_good = api_get_power_on();

    return system_power_good;
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

int32_t get_cpu1_north_vr_temp(double* reading)
{
    int ret;

    ret = api_get_tmp75_temperature(0x6, 0x4c, reading);

/*
    // Or use SMBus API to read

    uint16_t read_buf;

    // Use SMBus Read Word for TMP75 temperature
    ret = api_smbus_read_word(0x6, 0x4c, 0, &read_buf, false, 0, 0);
    DPRINT("smbus read word 0x%04x\n", read_buf);

    // Then Convert the smbus read_buf into sensor reading
    // ...
*/

    if (ret != 0)
    {
        return SENSOR_STATUS::ERROR;
    }

    return SENSOR_STATUS::NORMAL;
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
