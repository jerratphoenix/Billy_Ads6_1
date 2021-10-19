/*
 * Copyright (c) 2021 Phoenix Technologies Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace phoenix
{
namespace dbus
{
namespace monitor
{

namespace jsonns {

    /* Event filter table */
    struct tableinfo {
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
    };

    /* PEF Configuration Parameters */
    struct globalTableinfo {
        uint8_t control;                //#1
        uint8_t actionGlobalControl;    //#2
        uint8_t startupDelay;           //#3
        uint8_t alertstartupDelay;      //#4
    };
}//jsonns

enum Options {
      invalid,
      powerOn,
      powerOff,
      powerCycle,
      sendAlert
};


#define MAX_PEF_EVENT_ENTRIES 20

constexpr const char* configFile = "/usr/share/phoenix-dbus-monitor/PtecPEFConfig.json";
constexpr const char* TABLE_NAME = "PEF_TABLE";
constexpr const char* GLOBAL_TABLE_NAME = "PEF_GLOBAL";

extern struct jsonns::tableinfo pef_table[MAX_PEF_EVENT_ENTRIES];
extern struct jsonns::globalTableinfo global_pef_table;

void initJsonfile();
int getJsonSize();
void display_table_info(int size, int globalSize);

}//monitor
}//dbus
}//phoenix
