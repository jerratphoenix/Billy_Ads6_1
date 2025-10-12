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
/**
 * @file    sysfs.hpp
 *
 * @brief   Sysfs API
 *
 * @details This file contains Sysfs access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>

/**
 * @Name api_read_first_line_to_value()
 *
 * @Description This function can read the first line of sysfs file content into
 *              a double type value. User can use it to read the sysfs device
 *              that has been created.
 *
 * @param[in]  file           - The file location contains the path.
 * @param[out] *value         - Pointer to a variable buffer that reports sensor
 *                              reading value.
 *
 * @return Read status.
 *         On success, api_read_first_line_to_value() returns 0;
 *         on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 * int32_t get_hwmon9_fan1(double* reading)
 * {
 *     int32_t ret;
 *     ret = api_read_first_line_to_value("/sys/class/hwmon/hwmon9/fan1_input", reading);
 *
 *     if (ret != 0)
 *     {
 *          return SENSOR_STATUS::ERROR;
 *     }
 *
 *     return SENSOR_STATUS::NORMAL;
 * }
 * @endcode
 */
int32_t api_read_first_line_to_value (const std::string file, double *value);

