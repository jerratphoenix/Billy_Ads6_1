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

#include "bmc_update.hpp"
#include "debug.hpp"
#include "phoenix-dbus-sensors.hpp"

#include <stdlib.h>
#include <string.h>

#include <filesystem>
#include <fstream>
#include <iostream>

const std::string uboot_env_mtd_file = "/dev/mtd/u-boot-env";
const std::string init_options_file = "/run/initramfs/init-options";

static bool get_factory_reset_in_ubootenv(void)
{
    std::ifstream uboot_env(uboot_env_mtd_file);
    std::string env_var;

    std::getline(uboot_env, env_var);

    // U-boot env set by xyz.openbmc_project.Software.BMC.Updater
    if (env_var.find("openbmconce=factory-reset") != std::string::npos)
    {
        DPRINT("get_factory_reset_in_ubootenv true\n");
        return true;
    }

    return false;
}

static bool get_factory_reset_in_init_options(void)
{

    std::ifstream file(init_options_file);
    std::ostringstream file_data;

    if (file.is_open() == true)
    {
        while (file.peek() != EOF)
        {
            char temp;
            file.get(temp);
            file_data << temp;
        }
    }

    file.close();

    std::size_t found = file_data.str().find("factory-reset");

    if (found != std::string::npos)
    {
        DPRINT("get_factory_reset_in_init_options true\n");
        return true;
    }

    return false;
}

bool api_is_last_bmc_factory_reset(void)
{
    static bool last_bmc_restore_default = false;
    static bool load_initial = false;

    if (load_initial == false)
    {
        // Detect BMC last restore default:
        // Uboot env is clear and factory-reset set in init-options
        if (get_factory_reset_in_init_options() == true &&
            get_factory_reset_in_ubootenv() == false)
        {
            DPRINT("last_bmc_restore_default true\n");
            last_bmc_restore_default = true;
        }

        load_initial = true;
    }

    return last_bmc_restore_default;
}

// TODO: Create more API here
