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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "debug.hpp"
#include "phoenix-dbus-sensors.hpp"
#include "bmc_state.hpp"

const std::string phoenix_bmc_reset_cause_file =
    "/var/log/bmc_reset_cause";

static int32_t get_current_bmc_state_from_state_manager_dbus(void)
{
    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);
    int CurrentBMCState;
    
    auto method = conn->new_method_call(
                      "xyz.openbmc_project.State.BMC",
                      "/xyz/openbmc_project/state/bmc0",
                      property_interface_name,
                      "GetAll");

    method.append("xyz.openbmc_project.State.BMC");
    boost::container::flat_map<std::string, std::variant<std::string>> bmcStatus;
    try {
        sdbusplus::message::message getBmcStatusResp = conn->call(method);
        getBmcStatusResp.read(bmcStatus);
    } 

    catch (sdbusplus::exception::SdBusError& e) {
        DPRINT ( "Get bmcStatus error\n");
        return BMC_STATE::BMC_STATE_ERROR;
    }

    auto findVal = bmcStatus.find("CurrentBMCState");
    std::string bmcState;
    if (findVal != bmcStatus.end()) {
        bmcState = std::visit(VariantToStrVisitor(), findVal->second);
    }
    
    if (bmcState == "xyz.openbmc_project.State.BMC.BMCState.Ready") {
        CurrentBMCState = BMC_STATE::BMC_STATE_READY;
    } else if (bmcState == "xyz.openbmc_project.State.BMC.BMCState.NotReady") {
        CurrentBMCState = BMC_STATE::BMC_STATE_NOT_READY;
    } else if (bmcState == "xyz.openbmc_project.State.BMC.BMCState.UpdateInProgress") {
        CurrentBMCState = BMC_STATE::BMC_STATE_UPDATE_IN_PROGRESS;
    } else if (bmcState == "xyz.openbmc_project.State.BMC.BMCState.Quiesced") {
        CurrentBMCState = BMC_STATE::BMC_STATE_QUIESCED;
    } else {
        CurrentBMCState = BMC_STATE::BMC_STATE_UNKNOW;
    }

    return CurrentBMCState;
}

static int32_t get_bmc_last_reboot_cause_from_state_manager_dbus(void)
{
    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);
    int reboot_cause = BMC_REBOOT_CAUSE::STATEMANAGER_BMC_REBOOT_CAUSE_UNKNOW;
    
    auto method = conn->new_method_call(
                      "xyz.openbmc_project.State.BMC",
                      "/xyz/openbmc_project/state/bmc0",
                      property_interface_name,
                      "GetAll");

    method.append("xyz.openbmc_project.State.BMC");
    boost::container::flat_map<std::string, std::variant<std::string>> bmcStatus;
    try {
        sdbusplus::message::message getBmcStatusResp = conn->call(method);
        getBmcStatusResp.read(bmcStatus);
    } 

    catch (sdbusplus::exception::SdBusError& e) {
        DPRINT ( "Get bmcStatus error\n");
        return BMC_REBOOT_CAUSE::STATEMANAGER_BMC_REBOOT_CAUSE_ERROR;
    }

    auto findVal = bmcStatus.find("LastRebootCause");
    std::string cause;
    if (findVal != bmcStatus.end()) {
        cause = std::visit(VariantToStrVisitor(), findVal->second);
    }
    
    if (cause == "xyz.openbmc_project.State.BMC.RebootCause.POR") {
        reboot_cause = BMC_REBOOT_CAUSE::STATEMANAGER_BMC_REBOOT_CAUSE_POWER_ON_RESET;
    } else if (cause == "xyz.openbmc_project.State.BMC.RebootCause.Watchdog") {
        reboot_cause = BMC_REBOOT_CAUSE::STATEMANAGER_BMC_REBOOT_CAUSE_WATCHDOG;
    } else {
        reboot_cause = BMC_REBOOT_CAUSE::STATEMANAGER_BMC_REBOOT_CAUSE_UNKNOW;
    }

    return reboot_cause;
}

int32_t get_bmc_last_reboot_cause_from_phoenix_extend(void)
{
    static int phoenix_bmc_reboot_cause_cache = BMC_REBOOT_CAUSE::PHOENIX_BMC_REBOOT_CAUSE_NOT_INITIALIZED;
    char read_buf[256];

    if (phoenix_bmc_reboot_cause_cache != BMC_REBOOT_CAUSE::PHOENIX_BMC_REBOOT_CAUSE_NOT_INITIALIZED) {
        return phoenix_bmc_reboot_cause_cache;
    }

    if (!std::filesystem::exists(phoenix_bmc_reset_cause_file)) {
        DPRINT("Error file %s not exists\n", phoenix_bmc_reset_cause_file.c_str());
        phoenix_bmc_reboot_cause_cache = BMC_REBOOT_CAUSE::PHOENIX_BMC_REBOOT_CAUSE_ERROR;
        return phoenix_bmc_reboot_cause_cache;
    }

    std::ifstream fileStream (phoenix_bmc_reset_cause_file.c_str());
    fileStream.getline (read_buf, 256);
    fileStream.close();

    // Remove file
    std::remove(phoenix_bmc_reset_cause_file.c_str());
    
    DPRINT("Get Phoenix BMC Reset Cause: %s\n", read_buf);

    int cause = std::stof(read_buf);
    
    // decode reset cause
    switch (cause) {
        case 1:
            phoenix_bmc_reboot_cause_cache = BMC_REBOOT_CAUSE::PHOENIX_BMC_REBOOT_CAUSE_IPMI_COLD_RESET_CMD;
            break;
        // TODO: implement more
        default:
            phoenix_bmc_reboot_cause_cache = BMC_REBOOT_CAUSE::PHOENIX_BMC_REBOOT_CAUSE_UNKNOW;
            break;
    }

    return phoenix_bmc_reboot_cause_cache;
}

int32_t api_get_current_bmc_state (void)
{
    return get_current_bmc_state_from_state_manager_dbus();
}

int32_t api_get_bmc_last_reboot_cause (void)
{
    int32_t bmc_last_reboot_cause;

    bmc_last_reboot_cause = get_bmc_last_reboot_cause_from_phoenix_extend();
        
    if (bmc_last_reboot_cause == BMC_REBOOT_CAUSE::PHOENIX_BMC_REBOOT_CAUSE_ERROR ||
        bmc_last_reboot_cause == BMC_REBOOT_CAUSE::PHOENIX_BMC_REBOOT_CAUSE_UNKNOW) {

        // Try to get reboot cause from state manager 
        bmc_last_reboot_cause = get_bmc_last_reboot_cause_from_state_manager_dbus();
    }

    return bmc_last_reboot_cause;
}

// TODO: Create more API here


