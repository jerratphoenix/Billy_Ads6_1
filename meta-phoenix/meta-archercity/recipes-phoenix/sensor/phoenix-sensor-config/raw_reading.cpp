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
    if (is_power_on()) {
        *reading = BIT(0);  // SensorType 22h, offset 00h: S0 / G0 "working"
    } else {
        *reading = BIT(5);  // SensorType 22h, offset 05h: S5 / G2 "soft-off"
    }

    return SENSOR_STATUS::NORMAL;
}

int32_t get_bmc_reset(double* reading)
{
    int bmc_reset_cause;
    static bool assert_event = false;
    uint8_t event_data[3];

    enum BMC_RESET_CAUSE_EVENT_DATA2
    {
        BMC_RESET_CAUSE_UNSPECIFIED = 0x00,
        BMC_RESET_CAUSE_BY_IPMI_COLD_RESET_CMD =
            0x01, // Currently only implement this for example
    };

    if (assert_event == true)
    {
        return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
    }

    // Get BMC reset cause
    bmc_reset_cause = api_get_bmc_last_reboot_cause();

    event_data[0] = 0x82; // Event Data 1

    // Assign OEM Event Data2
    switch (bmc_reset_cause)
    {
        case STATEMANAGER_BMC_REBOOT_CAUSE_UNKNOW:
        case STATEMANAGER_BMC_REBOOT_CAUSE_ERROR:
        case PHOENIX_BMC_REBOOT_CAUSE_UNKNOW:
        case PHOENIX_BMC_REBOOT_CAUSE_ERROR:
            // TODO: define your event data2
            event_data[1] = BMC_RESET_CAUSE_UNSPECIFIED;
            break;
        case PHOENIX_BMC_REBOOT_CAUSE_IPMI_COLD_RESET_CMD:
            // TODO: define your event data2
            event_data[1] = BMC_RESET_CAUSE_BY_IPMI_COLD_RESET_CMD;
            break;
        case STATEMANAGER_BMC_REBOOT_CAUSE_POWER_ON_RESET:
            // TODO: define your event data2
            event_data[1] = BMC_RESET_CAUSE_UNSPECIFIED;
            break;
        case STATEMANAGER_BMC_REBOOT_CAUSE_WATCHDOG:
            // TODO: define your event data2
            event_data[1] = BMC_RESET_CAUSE_UNSPECIFIED;
            break;
        default:
            // TODO: define your event data2
            event_data[1] = BMC_RESET_CAUSE_UNSPECIFIED;
            break;
    }

    event_data[2] = 0xff; // Event Data 3, un-used

    std::vector<uint8_t> vector_event_data(event_data, event_data + 3);

    // Because sensor value don't have information for event data1~3,
    // we assert SEL / REDFISH log at here.
    int ret = add_ipmi_std_sel_entry("BmcResetCause",
                                     "/xyz/openbmc_project/sensors/specific/"
                                     "BMC_Reset", // FIXME: not hard code
                                                  // sensor path
                                     vector_event_data,
                                     true,
                                     0x20);
    if (ret == 0)
    {
        assert_event = true;
    }

    // Notify sensor daemon we already handled event in here.
    return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
}

int32_t get_bmc_fw_update(double* reading)
{
    bool bmc_fw_updated = false;
    static bool assert_event = false;
    uint8_t event_data[3];

    if (assert_event == true)
    {
        return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
    }

    // Get last BMC fw updated
    bmc_fw_updated = api_is_last_bmc_updated();

    if (bmc_fw_updated == false)
    {
        assert_event = true;
        return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
    }

    event_data[0] = 0xC1; // Event Data 1
    event_data[1] = 0x01; // Event Data 2
    event_data[2] = 0xff; // Event Data 3

    std::vector<uint8_t> vector_event_data(event_data, event_data + 3);

    // Because sensor value don't have information for event data1~3,
    // we assert SEL / REDFISH log at here.
    int ret = add_ipmi_std_sel_entry(
        "BmcFwUpdate",
        "/xyz/openbmc_project/sensors/specific/BMC_FW_update",
        vector_event_data,
        true,
        0x20);
    if (ret == 0)
    {
        assert_event = true;
    }

    // Notify sensor daemon we already handled event in here.
    return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
}

int32_t get_ipmi_sel(double* reading)
{
    bool sel_clear = false;
    static bool assert_event = false;
    uint8_t event_data[3];

    // Get sel empty or not
    sel_clear = api_is_bmc_sel_empty();

    if (sel_clear == false)
    {
        assert_event = false;
        return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
    }

    if (assert_event == true && sel_clear == true)
    {
        return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
    }

    event_data[0] = 0x02; // Event Data 1
    event_data[1] = 0xff; // Event Data 2
    event_data[2] = 0xff; // Event Data 3

    std::vector<uint8_t> vector_event_data(event_data, event_data + 3);

    // Because sensor value don't have information for event data1~3,
    // we assert SEL / REDFISH log at here.
    int ret =
        add_ipmi_std_sel_entry("BmcSelClear",
                               "/xyz/openbmc_project/sensors/specific/IPMI_SEL",
                               vector_event_data,
                               true,
                               0x20);
    if (ret == 0)
    {
        assert_event = true;
    }

    // Notify sensor daemon we already handled event in here.
    return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
}
