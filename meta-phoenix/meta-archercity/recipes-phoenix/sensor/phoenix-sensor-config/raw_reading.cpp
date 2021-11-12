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

#include <random>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <gpiod.hpp>

#include <boost/algorithm/string.hpp>
#include "phoenix-dbus-sensors.hpp"
#include "sensorapi.hpp"

// User may implement their own "is_power_on" function to get power state.
// Or use our api_get_power_on to get from OpenBMC chassis dbus
bool is_power_on(void)
{
    bool system_power_good = api_get_power_on();

    return system_power_good;
}

// ArcherCity CPU is not SOC
static constexpr const bool cpu_is_soc = false;

static bool is_cpu_present(int cpu_index)
{
    if (cpu_is_soc == true) {
        return true;
    }

    return api_peci_ping(cpu_index);
}

int32_t get_system_crash (double *reading)
{
    int max_cpu_count = 2;
    int cpu;
    bool cpu_err = false;
    
    for (cpu = CPU_ID1; cpu < (CPU_ID1 + max_cpu_count) ; cpu++) {
        if (cpu > MAX_CPU_ID) {
            DPRINT("Invalid CPU numbers\n");
            break;
        }

        if ((is_cpu_present(cpu) == true && 
             api_is_cpu_err(cpu) == true)) {
             cpu_err = true;
        }
    }

    if (cpu_err == true) {
        *reading = BIT(1); // offset 01h: State Asserted
    } else {
        *reading = 0;
    }

    return SENSOR_STATUS::NORMAL;
}

int32_t get_sys_pwr_state (double *reading)
{
    if (is_power_on()) {
        *reading = BIT(0);  // SensorType 22h, offset 00h: S0 / G0 “working”
    } else {
        *reading = BIT(5);  // SensorType 22h, offset 05h: S5 / G2 “soft-off”
    }
    
    return SENSOR_STATUS::NORMAL;
}

int32_t get_bmc_reset (double *reading)
{
    int bmc_reset_cause;
    static bool assert_event = false;
    uint8_t event_data[3];

    enum BMC_RESET_CAUSE_EVENT_DATA2 {
        BMC_RESET_CAUSE_UNSPECIFIED = 0x00,
        BMC_RESET_CAUSE_BY_IPMI_COLD_RESET_CMD = 0x01, // Currently only implement this for example
    };

    if (assert_event == true) {
        return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
    }

    // Get BMC reset cause
    bmc_reset_cause = api_get_bmc_last_reboot_cause();

    event_data[0] = 0x82; // Event Data 1

    // Assign OEM Event Data2
    switch (bmc_reset_cause) {
        case STATEMANAGER_BMC_REBOOT_CAUSE_UNKNOW:
        case STATEMANAGER_BMC_REBOOT_CAUSE_ERROR:
        case PHOENIX_BMC_REBOOT_CAUSE_UNKNOW:
        case PHOENIX_BMC_REBOOT_CAUSE_ERROR:
            event_data[1] = BMC_RESET_CAUSE_UNSPECIFIED; // TODO: define your event data2
            break;
        case PHOENIX_BMC_REBOOT_CAUSE_IPMI_COLD_RESET_CMD:
            event_data[1] = BMC_RESET_CAUSE_BY_IPMI_COLD_RESET_CMD; // TODO: define your event data2
            break;
        case STATEMANAGER_BMC_REBOOT_CAUSE_POWER_ON_RESET:
            event_data[1] = BMC_RESET_CAUSE_UNSPECIFIED; // TODO: define your event data2
            break;
        case STATEMANAGER_BMC_REBOOT_CAUSE_WATCHDOG:
            event_data[1] = BMC_RESET_CAUSE_UNSPECIFIED; // TODO: define your event data2
            break; 
        default:
            event_data[1] = BMC_RESET_CAUSE_UNSPECIFIED; // TODO: define your event data2
            break; 
    }

    event_data[2] = 0xff; // Event Data 3, un-used

    std::vector<uint8_t> vector_event_data(event_data, event_data + 3);

    // Because sensor value don't have information for event data1~3, 
    // we assert SEL / REDFISH log at here.
    int ret = add_ipmi_std_sel_entry("BmcResetCause",
                                "/xyz/openbmc_project/sensors/specific/BMC_Reset", //FIXME: not hard code sensor path
                                vector_event_data, 
                                true,
                                0x20);
    if (ret == 0) {
        assert_event = true;
    }

    // Notify sensor daemon we already handled event in here.    
    return SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED;
}

