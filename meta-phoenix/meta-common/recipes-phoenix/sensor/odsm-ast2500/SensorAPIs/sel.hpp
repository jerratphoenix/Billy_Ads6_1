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
 * @file    sel.hpp
 *
 * @brief   SEL Sensor API
 *
 * @details This file contains SEL Sensor access functions for using in Phoenix
 *          sensor customization layer.
 */

#pragma once

#include <cstdint>

/**
 * @Name   api_is_bmc_sel_empty()
 *
 * @Description    This function detect BMC IPMI SEL is empty or not.
 *
 * @return  On BMC IPMI SEL is empty, it returns true;
 *          others return false.
 */
bool api_is_bmc_sel_empty(void);

/**
 * @Name   api_sensor_sel_clear()
 *
 * @Description   This function completes a discrete sensor as BMC SEL Clear.
 *                The sensor value and event log is compatible with IPMI
 *                Generic Event/Reading Type Codes 0x6F(Sensor-specific),
 *                Sensor Type Code 0x10(Event Logging Disabled). Support assert
 *                and deassert offset 0x2 Log Area Reset/Cleared.
 *
 * @param[out] *reading - Pointer to a variable buffer that reports sensor
 *                        reading value.
 *
 * @return  Sensor status. Refer to enum ::SENSOR_STATUS
 *
 * @code Example code:
 *
 *  // BMC IPMI SEL Clear sensor
 *  int32_t get_ipmi_sel_clear(double* reading)
 *  {
 *      return api_sensor_sel_clear(reading);
 *  }
 *
 * @endcode
 */
int32_t api_sensor_sel_clear(double* reading);

