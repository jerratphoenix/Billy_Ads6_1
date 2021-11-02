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
    
    /* Alert policy table */
    struct alertPolicyTableInfo
    {
        uint8_t policyNumber;       //Byte 1, bits[7:4]
        uint8_t enable;             //Byte 1, bits[3]
        uint8_t policy;             //Byte 1, bits[2:0]

        uint8_t channel;            //Byte 2, bits[7:4]
        uint8_t destination;        //Byte 2, bits[3:0]

        uint8_t is_event_specific;  //Byte 3, bit[7]
        uint8_t alertSrtingKey;     //Byte 3, bit[6:0]
    };

    /* Alert string table */
    struct alertStringTableInfo
    {
        uint8_t selector;
        uint8_t eventFilterNum;
        uint8_t alertStringSet;
        std::string alertString;
    };

    /* PEF Configuration Parameters */
    struct globalTableinfo {
        uint8_t control;                //#1
        uint8_t actionGlobalControl;    //#2
        uint8_t startupDelay;           //#3
        uint8_t alertstartupDelay;      //#4
    };

    /* Email table */
    struct emailTableInfo
    {
        std::string smtpIP;
        std::string recipient;
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
#define MAX_PEF_ALERT_POLICY_ENTRIES 127
#define MAX_PEF_ALERT_STRING_ENTRIES 127

constexpr const char* configFile = "/usr/share/phoenix-dbus-monitor/PtecPEFConfig.json";
constexpr const char* emailConfigFile = "/usr/share/phoenix-dbus-monitor/PtecEmailConfig.json";
constexpr const char* TABLE_NAME = "PEF_TABLE";
constexpr const char* GLOBAL_TABLE_NAME = "PEF_GLOBAL";
constexpr const char* ALERT_TABLE_NAME = "ALERT_POLICY";
constexpr const char* ALERT_STRING_NAME = "ALERT_STRING";
constexpr const char* SERVER_STRING_NAME = "Service_IP";
constexpr const char* REC_STRING_NAME = "Recipient";

extern struct jsonns::tableinfo pef_table[MAX_PEF_EVENT_ENTRIES];
extern struct jsonns::alertPolicyTableInfo alert_policy_table[MAX_PEF_ALERT_POLICY_ENTRIES];
extern struct jsonns::alertStringTableInfo string_table[MAX_PEF_ALERT_STRING_ENTRIES];
extern struct jsonns::globalTableinfo global_pef_table;
extern struct jsonns::emailTableInfo mail_table;

void initJsonfile();
void initMailFile();
void display_table_info(int size, int globalSize);
int getJsonSize();

}//monitor
}//dbus
}//phoenix
