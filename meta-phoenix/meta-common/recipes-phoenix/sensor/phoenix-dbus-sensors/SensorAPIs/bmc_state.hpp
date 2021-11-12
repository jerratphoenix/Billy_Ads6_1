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

#include <stdlib.h>
#include "debug.hpp"

enum BMC_STATE {
    BMC_STATE_ERROR,
    BMC_STATE_UNKNOW,
    BMC_STATE_READY,
    BMC_STATE_NOT_READY,
    BMC_STATE_UPDATE_IN_PROGRESS,
    BMC_STATE_QUIESCED
};

enum BMC_REBOOT_CAUSE {
    STATEMANAGER_BMC_REBOOT_CAUSE_ERROR,
    STATEMANAGER_BMC_REBOOT_CAUSE_UNKNOW,
    STATEMANAGER_BMC_REBOOT_CAUSE_POWER_ON_RESET,
    STATEMANAGER_BMC_REBOOT_CAUSE_WATCHDOG,

    PHOENIX_BMC_REBOOT_CAUSE_NOT_INITIALIZED,
    PHOENIX_BMC_REBOOT_CAUSE_ERROR,
    PHOENIX_BMC_REBOOT_CAUSE_UNKNOW,
    PHOENIX_BMC_REBOOT_CAUSE_IPMI_COLD_RESET_CMD,
    PHOENIX_BMC_REBOOT_CAUSE_IPMI_WARN_RESET_CMD, //FIXME: not implement yet
    PHOENIX_BMC_REBOOT_CAUSE_WEB, //FIXME: not implement yet
    PHOENIX_BMC_REBOOT_CAUSE_REDFISH, //FIXME: not implement yet
};

int32_t api_get_current_bmc_state (void);
int32_t api_get_bmc_last_reboot_cause (void);


