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
 * @file    bmc_factory_reset.hpp
 *
 * @brief   BMC Factory Reset Sensor API
 *
 * @details This file contains BMC Factory Reset Sensor access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>

/**
 * @Name   api_is_last_bmc_factory_reset()
 *
 * @Description    This function report BMC factory reset or not before last
 *                 BMC boot.
 *
 * @return  BMC Factory Reset or not.
 *          On last BMC factory reset, it returns true;
 *          others return false.
 */
bool api_is_last_bmc_factory_reset (void);

/**
 * @Name   api_sensor_bmc_factory_reset()
 *
 * @Description   This function completes a discrete sensor as BMC Factory
 *                Reset. The sensor value and event log is compatible with
 *                IPMI Generic Event/Reading Type Codes 0x03. Support assert
 *                and deassert offset 0x1 "State Asserted".
 *
 * @param[out] *reading - Pointer to a variable buffer that reports sensor
 *                        reading value.
 *
 * @return  Sensor status. Refer to enum ::SENSOR_STATUS
 *
 * @code Example code:
 *
 *  // BMC Factory Reset sensor
 *  int32_t get_bmc_factory_reset(double* reading)
 *  {
 *      return api_sensor_bmc_factory_reset(reading);
 *  }
 *
 * @endcode
 */
int32_t api_sensor_bmc_factory_reset(double* reading);


