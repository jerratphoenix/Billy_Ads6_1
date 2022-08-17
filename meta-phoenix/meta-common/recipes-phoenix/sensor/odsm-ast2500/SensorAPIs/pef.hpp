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
 * @file    pef.hpp
 *
 * @brief   PEF Sensor API
 *
 * @details This file contains PEF Sensor access functions for using in Phoenix
 *          sensor customization layer.
 */

#pragma once

#include <cstdint>

 /**
 * @Name   api_sensor_pef()
 *
 * @Description   This function completes a discrete sensor as IPMI Platform
 *                Event Filter. The sensor value and event log is compatible
 *                with IPMI Generic Event/Reading Type Codes 0x6F, Sensor Type
 *                Code 0x12(System Event). Support assert and deassert offset
 *                0x4 PEF Action and Event Data 2. The following bits reflect
 *                the PEF Actions that are about to be taken after the event
 *                filters have been matched. The event is captured before the
 *                actions are taken.
 *                [7:6] - reserved
 *                [5] - 1b = Diagnostic Interrupt (NMI)
 *                [4] - 1b = OEM action
 *                [3] - 1b = power cycle
 *                [2] - 1b = reset
 *                [1] - 1b = power off
 *                [0] - 1b = Alert
 *
 * @param[out] *reading - Pointer to a variable buffer that reports sensor
 *                        reading value.
 *
 * @return  Sensor status. Refer to enum ::SENSOR_STATUS
 *
 * @code Example code:
 *
 *  // IPMI PEF sensor
 *  int32_t get_ipmi_pef(double* reading)
 *  {
 *      return api_sensor_pef(reading);
 *  }
  *
 * @endcode
 */
int32_t api_sensor_pef(double* reading);

sdbusplus::bus::match::match register_pef_event_handler(
    std::shared_ptr<sdbusplus::asio::connection> conn);
