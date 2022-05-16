/*
// Copyright (c) 2022 Phoenix Technologies Ltd.
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

#pragma once

#include <stdlib.h>

int32_t api_get_dimm_presence(double* reading);

sdbusplus::bus::match::match register_dimm_presence_event_handler(
    std::shared_ptr<sdbusplus::asio::connection> conn);

enum OS_STATE
{
    OS_STATE_STANDBY,
    OS_STATE_INACTIVE
};

enum DIMM_PRESENCE
{
    NOPRESENT = 0,
    PRESENT,
};

constexpr const char* smbios_mdrv2_service = "xyz.openbmc_project.Smbios.MDR_V2";
constexpr const char* dimm_obj_path[] = {
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm0",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm1",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm2",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm3",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm4",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm5",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm6",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm7",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm8",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm9",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm10",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm11",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm12",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm13",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm14",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm15",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm16",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm17",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm18",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm19",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm20",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm21",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm22",
    "/xyz/openbmc_project/inventory/system/chassis/motherboard/dimm23"

    };

