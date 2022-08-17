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
/**
 * @file    bmc_state.hpp
 *
 * @brief   BMC State Sensor API
 *
 * @details This file contains BMC State Sensor access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>

enum BMC_STATE {
    BMC_STATE_ERROR,
    /**< BMC_STATE_ERROR implies got error when read BMC state */

    BMC_STATE_UNKNOW,
    /**< BMC_STATE_UNKNOW implies got the unknow BMC state */

    BMC_STATE_READY,
    /**< BMC_STATE_READY implies all services started and are running
         successfully */

    BMC_STATE_NOT_READY,
    /**< BMC_STATE_NOT_READY implies not all services have started or are not
         running successfully */

    BMC_STATE_UPDATE_IN_PROGRESS,
    /**< BMC is in firmware update mode. While starting image download and
         reset to Ready, once activation is done or error case during update
         process. */

    BMC_STATE_QUIESCED
    /**< BMC firmware is quiesced. The BMC firmware is enabled but either
         unresponsive or only processing a restricted set of commands. This
         state may be the result of a service within the BMC going into a
         failed state. */
};

enum BMC_REBOOT_CAUSE {
    STATEMANAGER_BMC_REBOOT_CAUSE_ERROR,
    /**< Got error when read last BMC reboot caused from State Manager
         service */

    STATEMANAGER_BMC_REBOOT_CAUSE_UNKNOW,
    /**< Got last BMC reboot is caused by Unknown from State Manager service */

    STATEMANAGER_BMC_REBOOT_CAUSE_POWER_ON_RESET,
    /**< Got last BMC reboot is caused by Power-On-Reset from State Manager
         service */

    STATEMANAGER_BMC_REBOOT_CAUSE_WATCHDOG,
    /**< Got last BMC reboot is caused by Watchdog from State Manager
         service */

    PHOENIX_BMC_REBOOT_CAUSE_NOT_INITIALIZED,
    /**< Got not initialized when read last BMC reboot caused from Phoenix
         extension record */

    PHOENIX_BMC_REBOOT_CAUSE_ERROR,
    /**< Got error when read last BMC reboot caused from Phoenix extension
         record */

    PHOENIX_BMC_REBOOT_CAUSE_UNKNOW,
    /**< Got last BMC reboot is caused by Unknown from Phoenix extension
         record */

    PHOENIX_BMC_REBOOT_CAUSE_IPMI_COLD_RESET_CMD,
    /**< Got last BMC reboot is caused by IPMI Cold Reset Command from Phoenix
         extension record */

    //PHOENIX_BMC_REBOOT_CAUSE_IPMI_WARN_RESET_CMD, //FIXME: not implement yet
    //PHOENIX_BMC_REBOOT_CAUSE_WEB, //FIXME: not implement yet
    //PHOENIX_BMC_REBOOT_CAUSE_REDFISH, //FIXME: not implement yet
};

enum ACBOOT_STATE {
    ACBOOT_STATE_FALSE,
    ACBOOT_STATE_TRUE,
    ACBOOT_STATE_UNKNOWN
};

/**
 * @Name   api_get_current_bmc_state()
 *
 * @Description    This function report current state of the BMC firmware.
 *
 * @return  BMC firmware state. Refer to enum ::BMC_STATE.
 */
int32_t api_get_current_bmc_state (void);

/**
 * @Name   api_get_bmc_last_reboot_cause()
 *
 * @Description    This function report the recorded reboot cause of the BMC.
 *
 * @return  BMC last reboot cause. Refer to enum ::BMC_REBOOT_CAUSE.
 */
int32_t api_get_bmc_last_reboot_cause (void);

int32_t api_sensor_bmc_reset(double* reading);

int32_t get_BMC_ACBoot_state_from_Intel_Settings_dbus(void);

