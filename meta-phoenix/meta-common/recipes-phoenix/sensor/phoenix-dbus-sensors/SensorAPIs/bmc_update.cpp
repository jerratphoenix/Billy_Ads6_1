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
#include "bmc_update.hpp"

const std::string phoenix_bmc_update_complete_file =
    "/var/log/phoenix_bmc_update_complete";

int32_t get_bmc_update_complete_from_phoenix_extend_file(void)
{
    static int phoenix_bmc_update_complete = BMC_UPDATE_STATE::PHOENIX_BMC_UPDATE_STATE_NOT_INITIALIZED;
    char read_buf[256];

    if (phoenix_bmc_update_complete != BMC_UPDATE_STATE::PHOENIX_BMC_UPDATE_STATE_NOT_INITIALIZED) {
        return phoenix_bmc_update_complete;
    }

    if (!std::filesystem::exists(phoenix_bmc_update_complete_file)) {
        phoenix_bmc_update_complete = BMC_UPDATE_STATE::PHOENIX_BMC_UPDATE_STATE_NONE;
        return phoenix_bmc_update_complete;
    }

    std::ifstream fileStream (phoenix_bmc_update_complete_file.c_str());
    fileStream.getline (read_buf, 256);
    fileStream.close();

    // Remove file
    std::remove(phoenix_bmc_update_complete_file.c_str());
    
    DPRINT("Get Phoenix BMC Update Complete\n");

    // Update static variable
    phoenix_bmc_update_complete = BMC_UPDATE_STATE::PHOENIX_BMC_UPDATE_STATE_COMPLETED;

    return phoenix_bmc_update_complete;
}

bool api_is_last_bmc_updated (void)
{
    bool last_bmc_updated = false;

    if (get_bmc_update_complete_from_phoenix_extend_file() == BMC_UPDATE_STATE::PHOENIX_BMC_UPDATE_STATE_COMPLETED) {
        last_bmc_updated = true;
    }
    
    return last_bmc_updated;
}

// TODO: Create more API here


