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

#pragma once

#include <cstdint>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ipmid/api-types.hpp>
#include <ipmid/api.hpp>
#include <ipmid/message.hpp>
#include <ipmid/message/types.hpp>
#include <ipmid/types.hpp>
#include <ipmid/utils.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/process.hpp>
#include <ipmid/api.hpp>
#include <ipmid/message.hpp>

namespace ipmi
{
namespace pef
{
#define MAX_PEF_EVENT_ENTRIES 20

constexpr Cc ccParamNotSupported = 0x80;
constexpr Cc ccParamSetLocked = 0x81;
constexpr Cc ccParamReadOnly = 0x82;
constexpr Cc ccParamWriteOnly = 0x83;

enum class PefParam : uint8_t
{
    SetInProgress = 0,
    PEFCtrl = 1,
    EventFilterTable = 6,
    EventFilterTableData1 = 7,
};

enum class Progress : uint8_t
{
    SetComplete = 0,
    SetInProgress = 1,
    CommitWrite = 2,
    Reserved = 3,
};

namespace jsonns 
{
    constexpr const char* configFile = "/usr/share/phoenix-dbus-monitor/PtecPEFConfig.json";
    constexpr const char* TABLE_NAME = "PEF_TABLE";
    constexpr const char* GLOBAL_TABLE_NAME = "PEF_GLOBAL";

    /* Event filter table */
    struct eventFilter {
        uint8_t selector;
        uint8_t filterConfig;
        uint8_t filterAction;
        uint8_t alertPolicyNumber;
        uint8_t severity;
        uint8_t genID_1;
        uint8_t genID_2;
        uint8_t sensorType;
        uint8_t sensorNumber;
        uint8_t eventTrigger;
        uint16_t ED_1_OffsetMask;
        uint8_t ED_1_ANDMask;
        uint8_t ED_1_Compare_1;
        uint8_t ED_1_Compare_2;
        uint8_t ED_2_ANDMask;
        uint8_t ED_2_Compare_1;
        uint8_t ED_2_Compare_2;
        uint8_t ED_3_ANDMask;
        uint8_t ED_3_Compare_1;
        uint8_t ED_3_Compare_2;
    } __attribute__ ((packed));

    /* PEF Configuration Parameters */
    struct globalConfig {
        uint8_t control;                //#1
        uint8_t actionGlobalControl;    //#2
        uint8_t startupDelay;           //#3
        uint8_t alertstartupDelay;      //#4
    } __attribute__ ((packed));
}//jsonns

} // namespace pef
} // namespace ipmi