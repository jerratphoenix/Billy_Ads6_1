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

#include "chassis_state.hpp"
#include "debug.hpp"
#include "phoenix-dbus-sensors.hpp"

#include <stdlib.h>
#include <string.h>
#include <filesystem>
#include <fstream>
#include <iostream>

static int32_t get_current_power_state_from_state_manager_dbus(void)
{
    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);
    int CurrentPowerState = POWER_STATE::POWER_STATE_UNKNOW;

    auto method = conn->new_method_call("xyz.openbmc_project.State.Chassis",
                                        "/xyz/openbmc_project/state/chassis0",
                                        property_interface_name,
                                        "GetAll");

    method.append("xyz.openbmc_project.State.Chassis");
    boost::container::flat_map<std::string, std::variant<std::string>>
        chassisStatus;
    try
    {
        sdbusplus::message::message getChassisStatusResp = conn->call(method);
        getChassisStatusResp.read(chassisStatus);
    }

    catch (sdbusplus::exception::SdBusError& e)
    {
        DPRINT("Get chassisStatus error\n");
        return POWER_STATE_UNKNOW;
    }

    auto findVal = chassisStatus.find("CurrentPowerState");
    std::string powerState;

    if (findVal != chassisStatus.end())
    {
        powerState = std::visit(VariantToStrVisitor(), findVal->second);
    }

    if (powerState == "xyz.openbmc_project.State.Chassis.PowerState.On")
    {
        CurrentPowerState = POWER_STATE::POWER_STATE_ON;
    }
    else if (powerState == "xyz.openbmc_project.State.Chassis.PowerState.Off")
    {
        CurrentPowerState = POWER_STATE::POWER_STATE_OFF;
    }
    else if (powerState ==
             "xyz.openbmc_project.State.Chassis.PowerState.TransitioningToOff")
    {
        CurrentPowerState = POWER_STATE::POWER_STATE_TRANSITIONING_TO_OFF;
    }
    else if (powerState ==
             "xyz.openbmc_project.State.Chassis.PowerState.TransitioningToOn")
    {
        CurrentPowerState = POWER_STATE::POWER_STATE_TRANSITIONING_TO_ON;
    }
    else
    {
        CurrentPowerState = POWER_STATE::POWER_STATE_UNKNOW;
    }

    return CurrentPowerState;
}

bool api_get_power_on(void)
{
    bool is_power_on = false;
    int current_power_state = get_current_power_state_from_state_manager_dbus();

    if (current_power_state == POWER_STATE_ON)
    {
        is_power_on = true;
    }

    return is_power_on;
}

// TODO: Create more API here
