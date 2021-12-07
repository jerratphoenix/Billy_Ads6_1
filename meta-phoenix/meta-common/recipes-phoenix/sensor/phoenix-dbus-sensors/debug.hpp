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

#define DEBUG_FUNC_START_END false

#define DEBUG_SENSOR_INFO false

#define DEBUG_ALL_SENSOR_VALUE false

#include <string.h>
#include <syslog.h>

#include <iostream>

/*
#define LOG_EMERG    0
#define LOG_ALERT    1
#define LOG_CRIT     2
#define LOG_ERR      3
#define LOG_WARNING  4
#define LOG_NOTICE   5
#define LOG_INFO     6
#define LOG_DEBUG    7
*/

#define DSYS_LOG(level, fmt, ...) ({ syslog(level, fmt, ##__VA_ARGS__); })

#define DPRINT_EMERG(fmt, ...) DSYS_LOG(LOG_EMERG, fmt, ##__VA_ARGS__)
#define DPRINT_ALERT(fmt, ...) DSYS_LOG(LOG_ALERT, fmt, ##__VA_ARGS__)
#define DPRINT_CRIT(fmt, ...) DSYS_LOG(LOG_CRIT, fmt, ##__VA_ARGS__)
#define DPRINT_ERR(fmt, ...) DSYS_LOG(LOG_ERR, fmt, ##__VA_ARGS__)
#define DPRINT_WARN(fmt, ...) DSYS_LOG(LOG_WARNING, fmt, ##__VA_ARGS__)
#define DPRINT_NOTICE(fmt, ...) DSYS_LOG(LOG_NOTICE, fmt, ##__VA_ARGS__)
#define DPRINT_INFO(fmt, ...) DSYS_LOG(LOG_INFO, fmt, ##__VA_ARGS__)
#define DPRINT_DEBUG(fmt, ...) DSYS_LOG(LOG_DEBUG, fmt, ##__VA_ARGS__)

#define DPRINT(fmt, ...) DPRINT_ERR(fmt, ##__VA_ARGS__)

#if (DEBUG_FUNC_START_END == true)
#define FUNC_START() DPRINT_DEBUG("%s Start\n", __FUNCTION__)
#define FUNC_END() DPRINT_DEBUG("%s End\n", __FUNCTION__)
#else
#define FUNC_START()                                                           \
    if (0)                                                                     \
    {                                                                          \
        ;                                                                      \
    }
#define FUNC_END()                                                             \
    if (0)                                                                     \
    {                                                                          \
        ;                                                                      \
    }
#endif
