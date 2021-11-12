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
#include <peci.h>

#include "debug.hpp"
#include "phoenix-dbus-sensors.hpp"

#include "host_cpu.hpp"

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

static bool is_cpu_mca_err(uint8_t cpu_index)
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


bool api_peci_ping(int cpu_index)
{
    EPECIStatus ret;
    uint8_t addr = 0x30 + cpu_index; // Intel CPU PECI addr 0x30~0x38
    
    ret = peci_Ping(addr); 

    if (ret != 0) {
        return false;
    } else {
        return true;
    }

    return false;
}

bool api_is_cpu_err (uint8_t cpu_index)
{
    bool cpu_err = false;

    if (is_cpu_mca_err(cpu_index) == true) {
        cpu_err = true;
    }

    // TODO: check more cpu err
    
    return cpu_err;
}

// TODO: Create more API here


