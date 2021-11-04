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
#include <peci.h>
#include <boost/algorithm/string.hpp>

#include "phoenix-dbus-sensors.hpp"
#include "adc.hpp"
#include "gpio.hpp"
#include "sel.hpp"

#include "debug.hpp"

struct VariantToStrVisitor {
    template <typename T> std::string operator()(const T& t) const
    {
        return static_cast<std::string>(t);
    }
};

enum CPU_INDEX {
    CPU_ID1 = 0,
    MAX_CPU_ID,
};

bool cpu_present[MAX_CPU_ID] = { false };

static bool last_power_good;
static struct timespec last_poweron_ts;
static struct timespec last_poweroff_ts;

int get_system_timespec(struct timespec *ts);

struct timespec get_last_poweron_ts(void)
{
    return last_poweron_ts;
}

struct timespec get_last_poweroff_ts(void)
{
    return last_poweroff_ts;
}

bool is_power_on(void)
{
//FIXME: debug only
    return true;

    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);
    bool powerGood = false;
    
    auto method = conn->new_method_call(
                      "xyz.openbmc_project.State.Chassis",
                      "/xyz/openbmc_project/state/chassis0",
                      property_interface_name,
                      "GetAll");

    method.append("xyz.openbmc_project.State.Chassis");
    boost::container::flat_map<std::string, std::variant<std::string>> chassisStatus;
    try {
        sdbusplus::message::message getChassisStatusResp = conn->call(method);
        getChassisStatusResp.read(chassisStatus);
    } 

    catch (sdbusplus::exception::SdBusError& e) {
        DPRINT ( "Get chassisStatus error\n");
        return false;
    }

    auto findVal = chassisStatus.find("CurrentPowerState");
    std::string powerState;
    if (findVal != chassisStatus.end()) {
        powerState = std::visit(VariantToStrVisitor(), findVal->second);
    }
    
    if (powerState == "xyz.openbmc_project.State.Chassis.PowerState.On")
    {
        powerGood = true;
    } else {
        powerGood = false;
    }

    if (last_power_good != powerGood) {
        last_power_good = powerGood;

        if (powerGood == true) {
            get_system_timespec(&last_poweron_ts);
        } else {
            get_system_timespec(&last_poweroff_ts);
        }
    }

    return powerGood;
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

void update_cpu_present(int cpu_index)
{
    // BrightonCity is SOC
    cpu_present[CPU_ID1] = true;
}

bool is_cpu_err(uint8_t cpu_index)
{
    EPECIStatus ret;
    uint8_t completion_code = 0;
    uint8_t addr = 0x30 + cpu_index;
    uint32_t mca_err_log;
    uint8_t PkgIndex = 0x00;
    uint16_t PkgParam = 0x0005;

    ret = peci_RdPkgConfig(addr,
                           PkgIndex,
                           PkgParam,
                           sizeof(uint32_t),
                           (uint8_t *)&mca_err_log,
                           &completion_code);
/*
    DPRINT("ret = 0x%x, completion_code = 0x%x, MCA ERROR SOURCE LOG: 0x%x\n", 
            ret, 
            completion_code, 
            mca_err_log);
*/
#define MCA_ERR_MSMI_MCERR_INTERNAL BIT(18)
#define MCA_ERR_MSMI_IERR_INTERNAL  BIT(19)
#define MCA_ERR_MSMI_INTERNAL       BIT(20)
#define MCA_ERR_MSMI_MCERR          BIT(21)
#define MCA_ERR_MSMI_IERR           BIT(22)
#define MCA_ERR_MSMI                BIT(23)
#define MCA_ERR_MCERR_INTERNAL      BIT(26)
#define MCA_ERR_IERR_INTERNAL       BIT(27)
#define MCA_ERR_CATERR_INTERNAL     BIT(28)
#define MCA_ERR_MCERR               BIT(29)
#define MCA_ERR_IERR                BIT(30)
#define MCA_ERR_CATERR              BIT(31)

    if (ret != 0) {
        return false;
    }

    if (completion_code == 0x91) {
        return true;
    }

    if ((mca_err_log & MCA_ERR_MSMI_INTERNAL) ||
        (mca_err_log & MCA_ERR_MSMI) ||
        (mca_err_log & MCA_ERR_CATERR_INTERNAL) ||
        (mca_err_log & MCA_ERR_CATERR)) {
        return true;
    }

    return false;
}

int32_t get_system_crash (double *reading)
{
    update_cpu_present(CPU_ID1);

    if ((cpu_present[CPU_ID1] == true && is_cpu_err(CPU_ID1) == true)) {
        *reading = BIT(1); // offset 01h: State Asserted
    } else {
        *reading = 0;
    }

    return SENSOR_STATUS::NORMAL;
}
