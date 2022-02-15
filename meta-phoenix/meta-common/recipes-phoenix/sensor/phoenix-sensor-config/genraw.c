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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#define WRITE_TO_FILE(format, ...)      \
do {                                    \
    fprintf(fp, format, ##__VA_ARGS__); \
} while(0)

#define LOG_DBG(format, ...)      \
do {                                    \
    fprintf(stderr, "[Sensor-config] " format, ##__VA_ARGS__); \
} while(0)

#define WRITE_START_BRACKETS WRITE_TO_FILE("%s\n", "{")
#define WRITE_END_BRACKETS WRITE_TO_FILE("%s\n", "};")
#define NOT_DEFINE_STATE "NOT_DEFINE"

const char *funType = "int32_t";
const char *varType = "double";
const char *varName = "*ptr";

static void writeContentHead(FILE *fp)
{
    WRITE_TO_FILE("\n%s\n","#pragma once");

    /* Write struct callback_map_t*/
    WRITE_TO_FILE("\n%s\n","struct callback_map_t");
    WRITE_START_BRACKETS;
    WRITE_TO_FILE("\t%s\n","char name[32];");
    WRITE_TO_FILE("\t%s\n","int32_t (*read_func) (double*);");
    WRITE_TO_FILE("\t%s\n","uint8_t reading_state;");

    WRITE_TO_FILE("\t%s\n","long int scan_period;");

    WRITE_TO_FILE("\t%s\n","long int power_on_delay;");
    WRITE_TO_FILE("\t%s\n","long int power_off_delay;");

    WRITE_TO_FILE("\t%s\n","int32_t crit_high_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t crit_low_retry;");
    
    WRITE_TO_FILE("\t%s\n","int32_t warn_high_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t warn_low_retry;");

    WRITE_TO_FILE("\t%s\n","int32_t offset0_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset1_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset2_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset3_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset4_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset5_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset6_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset7_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset8_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset9_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset10_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset11_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset12_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset13_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset14_retry;");
    WRITE_TO_FILE("\t%s\n","int32_t offset15_retry;");

    WRITE_TO_FILE("\t%s\n","int32_t offset0_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset1_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset2_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset3_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset4_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset5_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset6_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset7_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset8_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset9_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset10_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset11_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset12_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset13_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset14_assert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset15_assert_servrity;");

    WRITE_TO_FILE("\t%s\n","int32_t offset0_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset1_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset2_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset3_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset4_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset5_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset6_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset7_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset8_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset9_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset10_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset11_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset12_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset13_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset14_deassert_servrity;");
    WRITE_TO_FILE("\t%s\n","int32_t offset15_deassert_servrity;");

    //TODO: Extend here.
    WRITE_END_BRACKETS;

    /* Write sensor reading state enum*/
    WRITE_TO_FILE("\n%s\n","enum READING_STATE");
    WRITE_START_BRACKETS;
    WRITE_TO_FILE("\t%s\n","BOTH = 0,");
    WRITE_TO_FILE("\t%s\n","POWERON,");
    WRITE_TO_FILE("\t%s\n","POWEROFF,");
    WRITE_TO_FILE("\t%s\n","NOT_DEFINE,");
    //TODO: Extend here.
    WRITE_END_BRACKETS;
    WRITE_TO_FILE("\n");
}

static void writeCopyRight(FILE *fp)
{
    WRITE_TO_FILE("%s\n","/*"                                                                           );
    WRITE_TO_FILE("%s\n","// Copyright (c) 2021 Phoenix Technologies Ltd."                              );
    WRITE_TO_FILE("%s\n","//"                                                                           );
    WRITE_TO_FILE("%s\n","// Licensed under the Apache License, Version 2.0 (the \"License\");"         );
    WRITE_TO_FILE("%s\n","// you may not use this file except in compliance with the License."          );
    WRITE_TO_FILE("%s\n","// You may obtain a copy of the License at"                                   );
    WRITE_TO_FILE("%s\n","//"                                                                           );
    WRITE_TO_FILE("%s\n","//      http://www.apache.org/licenses/LICENSE-2.0"                           );
    WRITE_TO_FILE("%s\n","//"                                                                           );
    WRITE_TO_FILE("%s\n","// Unless required by applicable law or agreed to in writing, software"       );
    WRITE_TO_FILE("%s\n","// distributed under the License is distributed on an \"AS IS\" BASIS,"       );
    WRITE_TO_FILE("%s\n","// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied."  );
    WRITE_TO_FILE("%s\n","// See the License for the specific language governing permissions and"       );
    WRITE_TO_FILE("%s\n","// limitations under the License."                                            );
    WRITE_TO_FILE("%s\n","*/"                                                                           );
}

static void parseSensorFunction(json_object *jsonRecord, FILE *fp)
{
    json_object *jsonObj;
    const char *sensorFunName;

    if( !json_object_object_get_ex (jsonRecord, "Function", &jsonObj) )
    {
        /* Not define sensor function in the JSON config */
        return;
    }
    sensorFunName = json_object_get_string(jsonObj);
    
    /* Write define funtion*/
    /* Format: funType sensorFunName (varType varName);*/
    /* Example: int64_t get_cpu0_temp_value (char *ptr);*/
    /* TODO: Improve funType, varType, varName  to dynamic*/
    WRITE_TO_FILE("%s %s (%s %s);\n"
        , funType
        , sensorFunName
        , varType
        , varName
        //TODO: Extend here.
    );
}

static void parseSensorName(json_object *jsonRecord, FILE *fp)
{
    json_object *jsonObj;
    const char *sensorName;
    const char *sensorFunName;
    const char *sensorReadingState;

    const char *sensor_scan_period;

    const char *sensor_power_on_delay;
    const char *sensor_power_off_delay;

    const char *sensor_crit_high_retry;
    const char *sensor_crit_low_retry;
    
    const char *sensor_warn_high_retry;
    const char *sensor_warn_low_retry;

    const char *sensor_offset0_retry;
    const char *sensor_offset1_retry;
    const char *sensor_offset2_retry;
    const char *sensor_offset3_retry;
    const char *sensor_offset4_retry;
    const char *sensor_offset5_retry;
    const char *sensor_offset6_retry;
    const char *sensor_offset7_retry;
    const char *sensor_offset8_retry;
    const char *sensor_offset9_retry;
    const char *sensor_offset10_retry;
    const char *sensor_offset11_retry;
    const char *sensor_offset12_retry;
    const char *sensor_offset13_retry;
    const char *sensor_offset14_retry;
    const char *sensor_offset15_retry;

    const char *sensor_offset0_assert_servrity;
    const char *sensor_offset1_assert_servrity;
    const char *sensor_offset2_assert_servrity;
    const char *sensor_offset3_assert_servrity;
    const char *sensor_offset4_assert_servrity;
    const char *sensor_offset5_assert_servrity;
    const char *sensor_offset6_assert_servrity;
    const char *sensor_offset7_assert_servrity;
    const char *sensor_offset8_assert_servrity;
    const char *sensor_offset9_assert_servrity;
    const char *sensor_offset10_assert_servrity;
    const char *sensor_offset11_assert_servrity;
    const char *sensor_offset12_assert_servrity;
    const char *sensor_offset13_assert_servrity;
    const char *sensor_offset14_assert_servrity;
    const char *sensor_offset15_assert_servrity;

    const char *sensor_offset0_deassert_servrity;
    const char *sensor_offset1_deassert_servrity;
    const char *sensor_offset2_deassert_servrity;
    const char *sensor_offset3_deassert_servrity;
    const char *sensor_offset4_deassert_servrity;
    const char *sensor_offset5_deassert_servrity;
    const char *sensor_offset6_deassert_servrity;
    const char *sensor_offset7_deassert_servrity;
    const char *sensor_offset8_deassert_servrity;
    const char *sensor_offset9_deassert_servrity;
    const char *sensor_offset10_deassert_servrity;
    const char *sensor_offset11_deassert_servrity;
    const char *sensor_offset12_deassert_servrity;
    const char *sensor_offset13_deassert_servrity;
    const char *sensor_offset14_deassert_servrity;
    const char *sensor_offset15_deassert_servrity;

    json_object_object_get_ex (jsonRecord, "SensorName", &jsonObj);
    sensorName = json_object_get_string(jsonObj);

    if( !json_object_object_get_ex (jsonRecord, "Function", &jsonObj) )
    {
        sensorFunName = "NULL";
        LOG_DBG("Use NULL value instead user function name, \"%s\" lost sensor function, please check JSON file\n", sensorName);
    } else {
        sensorFunName = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "ReadingState", &jsonObj) )
    {
        sensorReadingState = NOT_DEFINE_STATE;
        LOG_DBG("Not define ReadingState, \"%s\" lost sensor reading state, please check JSON file\n", sensorName);
    } else {
        sensorReadingState = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "ScanPeriod", &jsonObj) )
    {
        sensor_scan_period = "-1";
        LOG_DBG("Not define ScanPeriod, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_scan_period = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "PowerOnDelay", &jsonObj) )
    {
        sensor_power_on_delay = "-1";
        LOG_DBG("Not define PowerOnDelay, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_power_on_delay = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "PowerOffDelay", &jsonObj) )
    {
        sensor_power_off_delay = "-1";
        LOG_DBG("Not define PowerOffDelay, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_power_off_delay = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryCritHigh", &jsonObj) )
    {
        sensor_crit_high_retry = "-1";
        LOG_DBG("Not define RetryCritHigh, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_crit_high_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryCritLow", &jsonObj) )
    {
        sensor_crit_low_retry = "-1";
        LOG_DBG("Not define RetryCritLow, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_crit_low_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryWarnHigh", &jsonObj) )
    {
        sensor_warn_high_retry = "-1";
        LOG_DBG("Not define RetryWarnHigh, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_warn_high_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryWarnLow", &jsonObj) )
    {
        sensor_warn_low_retry = "-1";
        LOG_DBG("Not define RetryWarnLow, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_warn_low_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset0", &jsonObj) )
    {
        sensor_offset0_retry = "-1";
        LOG_DBG("Not define RetryOffset0, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset0_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset1", &jsonObj) )
    {
        sensor_offset1_retry = "-1";
        LOG_DBG("Not define RetryOffset1, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset1_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset2", &jsonObj) )
    {
        sensor_offset2_retry = "-1";
        LOG_DBG("Not define RetryOffset2, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset2_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset3", &jsonObj) )
    {
        sensor_offset3_retry = "-1";
        LOG_DBG("Not define RetryOffset3, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset3_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset4", &jsonObj) )
    {
        sensor_offset4_retry = "-1";
        LOG_DBG("Not define RetryOffset4, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset4_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset5", &jsonObj) )
    {
        sensor_offset5_retry = "-1";
        LOG_DBG("Not define RetryOffset5, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset5_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset6", &jsonObj) )
    {
        sensor_offset6_retry = "-1";
        LOG_DBG("Not define RetryOffset6, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset6_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset7", &jsonObj) )
    {
        sensor_offset7_retry = "-1";
        LOG_DBG("Not define RetryOffset7, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset7_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset8", &jsonObj) )
    {
        sensor_offset8_retry = "-1";
        LOG_DBG("Not define RetryOffset8, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset8_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset9", &jsonObj) )
    {
        sensor_offset9_retry = "-1";
        LOG_DBG("Not define RetryOffset9, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset9_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset10", &jsonObj) )
    {
        sensor_offset10_retry = "-1";
        LOG_DBG("Not define RetryOffset10, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset10_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset11", &jsonObj) )
    {
        sensor_offset11_retry = "-1";
        LOG_DBG("Not define RetryOffset11, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset11_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset12", &jsonObj) )
    {
        sensor_offset12_retry = "-1";
        LOG_DBG("Not define RetryOffset12, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset12_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset13", &jsonObj) )
    {
        sensor_offset13_retry = "-1";
        LOG_DBG("Not define RetryOffset13, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset13_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset14", &jsonObj) )
    {
        sensor_offset14_retry = "-1";
        LOG_DBG("Not define RetryOffset14, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset14_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "RetryOffset15", &jsonObj) )
    {
        sensor_offset15_retry = "-1";
        LOG_DBG("Not define RetryOffset15, \"%s\" lost sensor scan period, please check JSON file\n", sensorName);
    } else {
        sensor_offset15_retry = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset0", &jsonObj) )
    {
        sensor_offset0_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset0, please check JSON file\n", sensorName);
    } else {
        sensor_offset0_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset1", &jsonObj) )
    {
        sensor_offset1_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset1, please check JSON file\n", sensorName);
    } else {
        sensor_offset1_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset2", &jsonObj) )
    {
        sensor_offset2_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset2, please check JSON file\n", sensorName);
    } else {
        sensor_offset2_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset3", &jsonObj) )
    {
        sensor_offset3_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset3, please check JSON file\n", sensorName);
    } else {
        sensor_offset3_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset4", &jsonObj) )
    {
        sensor_offset4_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset4, please check JSON file\n", sensorName);
    } else {
        sensor_offset4_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset5", &jsonObj) )
    {
        sensor_offset5_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset5, please check JSON file\n", sensorName);
    } else {
        sensor_offset5_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset6", &jsonObj) )
    {
        sensor_offset6_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset6, please check JSON file\n", sensorName);
    } else {
        sensor_offset6_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset7", &jsonObj) )
    {
        sensor_offset7_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset7, please check JSON file\n", sensorName);
    } else {
        sensor_offset7_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset8", &jsonObj) )
    {
        sensor_offset8_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset8, please check JSON file\n", sensorName);
    } else {
        sensor_offset8_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset9", &jsonObj) )
    {
        sensor_offset9_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset9, please check JSON file\n", sensorName);
    } else {
        sensor_offset9_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset10", &jsonObj) )
    {
        sensor_offset10_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset10, please check JSON file\n", sensorName);
    } else {
        sensor_offset10_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset11", &jsonObj) )
    {
        sensor_offset11_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset11, please check JSON file\n", sensorName);
    } else {
        sensor_offset11_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset12", &jsonObj) )
    {
        sensor_offset12_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset12, please check JSON file\n", sensorName);
    } else {
        sensor_offset12_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset13", &jsonObj) )
    {
        sensor_offset13_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset13, please check JSON file\n", sensorName);
    } else {
        sensor_offset13_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset14", &jsonObj) )
    {
        sensor_offset14_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset14, please check JSON file\n", sensorName);
    } else {
        sensor_offset14_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "AssertServrityOffset15", &jsonObj) )
    {
        sensor_offset15_assert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define AssertServrityOffset15, please check JSON file\n", sensorName);
    } else {
        sensor_offset15_assert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset0", &jsonObj) )
    {
        sensor_offset0_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset0, please check JSON file\n", sensorName);
    } else {
        sensor_offset0_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset1", &jsonObj) )
    {
        sensor_offset1_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset1, please check JSON file\n", sensorName);
    } else {
        sensor_offset1_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset2", &jsonObj) )
    {
        sensor_offset2_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset2, please check JSON file\n", sensorName);
    } else {
        sensor_offset2_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset3", &jsonObj) )
    {
        sensor_offset3_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset3, please check JSON file\n", sensorName);
    } else {
        sensor_offset3_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset4", &jsonObj) )
    {
        sensor_offset4_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset4, please check JSON file\n", sensorName);
    } else {
        sensor_offset4_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset5", &jsonObj) )
    {
        sensor_offset5_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset5, please check JSON file\n", sensorName);
    } else {
        sensor_offset5_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset6", &jsonObj) )
    {
        sensor_offset6_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset6, please check JSON file\n", sensorName);
    } else {
        sensor_offset6_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset7", &jsonObj) )
    {
        sensor_offset7_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset7, please check JSON file\n", sensorName);
    } else {
        sensor_offset7_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset8", &jsonObj) )
    {
        sensor_offset8_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset8, please check JSON file\n", sensorName);
    } else {
        sensor_offset0_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset9", &jsonObj) )
    {
        sensor_offset9_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset9, please check JSON file\n", sensorName);
    } else {
        sensor_offset9_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset10", &jsonObj) )
    {
        sensor_offset10_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset10, please check JSON file\n", sensorName);
    } else {
        sensor_offset10_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset11", &jsonObj) )
    {
        sensor_offset11_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset11, please check JSON file\n", sensorName);
    } else {
        sensor_offset11_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset12", &jsonObj) )
    {
        sensor_offset12_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset12, please check JSON file\n", sensorName);
    } else {
        sensor_offset12_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset13", &jsonObj) )
    {
        sensor_offset13_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset13, please check JSON file\n", sensorName);
    } else {
        sensor_offset13_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset14", &jsonObj) )
    {
        sensor_offset14_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset14, please check JSON file\n", sensorName);
    } else {
        sensor_offset10_deassert_servrity = json_object_get_string(jsonObj);
    }

    if( !json_object_object_get_ex (jsonRecord, "DeassertServrityOffset15", &jsonObj) )
    {
        sensor_offset15_deassert_servrity = "UNKNOW";
        LOG_DBG("\"%s\" does not define DeassertServrityOffset15, please check JSON file\n", sensorName);
    } else {
        sensor_offset15_deassert_servrity = json_object_get_string(jsonObj);
    }

    /* Write sensor_callback_map array elements*/
    /* Format:  { "sensorName"  ,   sensorFunName, sensorReadingState, .... },*/
    /* Example: { "CPU0_TEMP"   ,   get_cpu0_temp_value,    both, .... },*/
    WRITE_TO_FILE("\t{ \"%s\"\t,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s,\t%s },\n"
        , sensorName
        , sensorFunName
        , sensorReadingState
        , sensor_scan_period
        , sensor_power_on_delay
        , sensor_power_off_delay
        , sensor_crit_high_retry
        , sensor_crit_low_retry
        , sensor_warn_high_retry
        , sensor_warn_low_retry
        , sensor_offset0_retry
        , sensor_offset1_retry
        , sensor_offset2_retry
        , sensor_offset3_retry
        , sensor_offset4_retry
        , sensor_offset5_retry
        , sensor_offset6_retry
        , sensor_offset7_retry
        , sensor_offset8_retry
        , sensor_offset9_retry
        , sensor_offset10_retry
        , sensor_offset11_retry
        , sensor_offset12_retry
        , sensor_offset13_retry
        , sensor_offset14_retry
        , sensor_offset15_retry
        , sensor_offset0_assert_servrity
        , sensor_offset1_assert_servrity
        , sensor_offset2_assert_servrity
        , sensor_offset3_assert_servrity
        , sensor_offset4_assert_servrity
        , sensor_offset5_assert_servrity
        , sensor_offset6_assert_servrity
        , sensor_offset7_assert_servrity
        , sensor_offset8_assert_servrity
        , sensor_offset9_assert_servrity
        , sensor_offset10_assert_servrity
        , sensor_offset11_assert_servrity
        , sensor_offset12_assert_servrity
        , sensor_offset13_assert_servrity
        , sensor_offset14_assert_servrity
        , sensor_offset15_assert_servrity
        , sensor_offset0_deassert_servrity
        , sensor_offset1_deassert_servrity
        , sensor_offset2_deassert_servrity
        , sensor_offset3_deassert_servrity
        , sensor_offset4_deassert_servrity
        , sensor_offset5_deassert_servrity
        , sensor_offset6_deassert_servrity
        , sensor_offset7_deassert_servrity
        , sensor_offset8_deassert_servrity
        , sensor_offset9_deassert_servrity
        , sensor_offset10_deassert_servrity
        , sensor_offset11_deassert_servrity
        , sensor_offset12_deassert_servrity
        , sensor_offset13_deassert_servrity
        , sensor_offset14_deassert_servrity
        , sensor_offset15_deassert_servrity
        //TODO: Extend here.
    );
}

static int parseSdrJsonFile(char *content, char *filename)
{
    json_object *jsonSDRInfo;
    FILE *fp;
    int i;

    fp = fopen (filename, "w+");
    if (!fp) {
        printf ("Error opening file: %s\n", filename);
        return -1;
    }

    json_object_object_get_ex (json_tokener_parse(content), "Access", &jsonSDRInfo);
    
    writeCopyRight(fp);

    /* Write define callback_map_t struct*/
    writeContentHead(fp);

    /* Write define function*/
    for (i = 0; i < json_object_array_length(jsonSDRInfo); i++) {
        parseSensorFunction (json_object_array_get_idx(jsonSDRInfo, i),
                             fp);
    }

    /* Write define sensor_callback_map[] struct header*/
    WRITE_TO_FILE("\n%s\n","struct callback_map_t sensor_callback_map[] =");
    WRITE_START_BRACKETS;

    /* Write sensor_callback_map elements*/ 
    for (i = 0; i < json_object_array_length(jsonSDRInfo); i++) {
        parseSensorName (json_object_array_get_idx(jsonSDRInfo, i),
                        fp);
    };

    WRITE_END_BRACKETS;

    fclose (fp);

    return 0;
}

int main (int argc, char*argv[])
{
    FILE *fp;
    char *data;
    long fileSize;
    int n;

    if(argc != 3) {
        printf("Usage : %s JsonFile OutputFile\n", argv[0]);
        return -1;
    }

    fp = fopen (argv[1], "rb");
    if (!fp) {
        return -1;
    }

    fseek (fp, 0, SEEK_END);
    fileSize = ftell (fp);
    rewind(fp);

    data = (char *) malloc (fileSize);
    if (!data) {
        fclose (fp);
        return -1;
    }

    n = fread (data, sizeof (char), fileSize, fp);
    fclose (fp);

    if (parseSdrJsonFile(data, argv[2]) != 0) {
        printf("Fail\n");
    }

    free (data);

    return 0;
}
