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

#include "json_parser.hpp"
#include "phoenix_action.hpp"
#include "phoenix_util.hpp"

#include <stdio.h>
#include <stdlib.h>

namespace phoenix
{
namespace dbus
{
namespace monitor
{
namespace jsonns {

    #define STR_TO_HEX(s) strtoul(j[s].get<std::string>().c_str(), NULL, 0);

    /* Json init data to struct callbak */
    void from_json(const nlohmann::json& j, tableinfo& v) {
        v.selector =        STR_TO_HEX("Selector");
        v.filterConfig =    STR_TO_HEX("FilterConfiguration");
        v.filterAction =    STR_TO_HEX("EventFilterAction");
        v.alertPolicyNumber = STR_TO_HEX("AlertPolicyNumber");
        v.severity =        STR_TO_HEX("EventSeverity");
        v.genID_1 =         STR_TO_HEX("GeneratorIDByte_1");
        v.genID_2 =         STR_TO_HEX("GeneratorIDByte_2");
        v.sensorType =      STR_TO_HEX("SensorType");
        v.sensorNumber =    STR_TO_HEX("SensorNumber");
        v.eventTrigger =    STR_TO_HEX("EventTrigger");
        v.ED_1_OffsetMask = STR_TO_HEX("EventData_1_EventOffsetMask");
        v.ED_1_ANDMask =    STR_TO_HEX("EventData_1_ANDMask");
        v.ED_1_Compare_1 =  STR_TO_HEX("EventData_1_Compare_1");
        v.ED_1_Compare_2 =  STR_TO_HEX("EventData_1_Compare_2");
        v.ED_2_ANDMask =    STR_TO_HEX("EventData_2_ANDMask");
        v.ED_2_Compare_1 =  STR_TO_HEX("EventData_2_Compare_1");
        v.ED_2_Compare_2 =  STR_TO_HEX("EventData_2_Compare_2");
        v.ED_3_ANDMask =    STR_TO_HEX("EventData_3_ANDMask");
        v.ED_3_Compare_1 =  STR_TO_HEX("EventData_3_Compare_1");
        v.ED_3_Compare_2 =  STR_TO_HEX("EventData_3_Compare_2");
    }

    void from_json(const nlohmann::json& j, globalTableinfo& v) {
        v.control =             STR_TO_HEX("PEF_control");
        v.actionGlobalControl = STR_TO_HEX("PEF_Action_global_control");
        v.startupDelay =        STR_TO_HEX("PEF_Startup_Delay");
        v.alertstartupDelay =   STR_TO_HEX("PEF_Alert_Startup_Delay");
    }
}//jsonns

struct jsonns::tableinfo pef_table[MAX_PEF_EVENT_ENTRIES];
struct jsonns::globalTableinfo global_pef_table;

void display_table_info(int size, int globalSize)
{
    for ( int i = 0; i < globalSize; i++ ) {
        DPRINT ("%s, %d, ctrl = %x, actionCtrl = %x, startupDelay = %x, alertStartdelay = %x\n"
            , __func__, __LINE__
            , global_pef_table.control
            , global_pef_table.actionGlobalControl
            , global_pef_table.startupDelay
            , global_pef_table.alertstartupDelay);
    }

    for ( int i = 0; i < size; i++ ) {
        DPRINT ("%s, %d, "  
            "selector= %x, filterConfig = %x, filterAction = %x, alertPolicyNumber = %x"
            ", severity = %x, genID_1 = %x, genID_2 = %x, sensorType = %x"
            ", sensorNumber = %x, eventTrigger = %x, ED_1_OffsetMask = %x"
            ", ED_1_ANDMask = %x, ED_1_Compare_1 = %x, ED_1_Compare_2 = %x"
            ", ED_2_ANDMask = %x, ED_2_Compare_1 = %x, ED_2_Compare_2 = %x "
            ", ED_3_ANDMask = %x, ED_3_Compare_1 = %x, ED_3_Compare_2 = %x\n" 
            , __func__, __LINE__
            , pef_table[i].selector
            , pef_table[i].filterConfig
            , pef_table[i].filterAction
            , pef_table[i].alertPolicyNumber
            , pef_table[i].severity
            , pef_table[i].genID_1
            , pef_table[i].genID_2
            , pef_table[i].sensorType
            , pef_table[i].sensorNumber
            , pef_table[i].eventTrigger
            , pef_table[i].ED_1_OffsetMask
            , pef_table[i].ED_1_ANDMask
            , pef_table[i].ED_1_Compare_1
            , pef_table[i].ED_1_Compare_2
            , pef_table[i].ED_2_ANDMask
            , pef_table[i].ED_2_Compare_1
            , pef_table[i].ED_2_Compare_2
            , pef_table[i].ED_3_ANDMask
            , pef_table[i].ED_3_Compare_1
            , pef_table[i].ED_3_Compare_2);
    }
}

int getJsonSize()
{
    int length = 0;
    nlohmann::json j;
    std::ifstream jfile( configFile );

    if( !jfile )
    {
        std::cerr << "Open PEF json file fail!!\n";
        return length;
    }

    jfile >> j;
    jfile.close();

    length = j[TABLE_NAME].size();
    return length;
}

void initJsonfile()
{
    nlohmann::json j;

    /* Open PEF json file */
    std::ifstream jfile( configFile );

    if( !jfile )
    {
        std::cerr << "Open PEF json file fail!!\n";
        return;
    }

    jfile >> j;
    jfile.close();

#if DEBUG
    std::cout << "json j = " << j << std::endl;
#endif

    /* Get table size*/
    int peflength = j[TABLE_NAME].size();
    int globalpeflength = j[GLOBAL_TABLE_NAME].size();

    DPRINT ("%s, %d, PEF table size is %d, Globla table size is %d\n"
        , __func__, __LINE__
        , peflength, globalpeflength);

    if( peflength > MAX_PEF_EVENT_ENTRIES )
    {
        std::cerr << "PEF table size is out of range"
                  << " expect support " << MAX_PEF_EVENT_ENTRIES
                  << " entries\n";
        return;
    }

    /* Assign json data to struct */
    for ( int i = 0; i < peflength; i++ ) {
        /* Callback to from_json funciotn */
        pef_table[i] = j[TABLE_NAME][i];
    }

    for ( int i = 0; i < globalpeflength; i++ ) {
        /* Callback to from_json funciotn */
        global_pef_table = j[GLOBAL_TABLE_NAME][i];
    }

#if DEBUG
    display_table_info(peflength, globalpeflength);
#endif

    return;
}


}//monitor
}//dbus
}//phoenix
