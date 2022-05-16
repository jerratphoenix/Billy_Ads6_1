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
 * @file    watchdog2.hpp
 *
 * @brief   Watchdog2 Sensor API
 *
 * @details This file contains Watchdog2 Sensor access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>

/**
 * @Name   api_sensor_watchdog2()
 *
 * @Description   This function completes a discrete sensor as BMC Watchdog2.
 *                The sensor value is compatible with  IPMI Generic
 *                Event/Reading Type Codes 0x6F(Sensor-specific), Sensor Type
 *                Code 0x23(Watchdog2).
 *                Support assert and deassert offset
 *                0h Timer expired, status only (no action, no interrupt)
 *                1h Hard Reset
 *                2h Power Down
 *                3h Power Cycle
 *                8h Timer interrupt
 *                The Event Data 2 field for this command can be used to
 *                provide an event extension code, with the following
 *                definition:
 *                7:4 interrupt type
 *                0h = none
 *                1h = SMI
 *                2h = NMI
 *                3h = Messaging Interrupt
 *                Fh = unspecified
 *                all other = reserved
 *                3:0 timer use at expiration:
 *                0h = reserved
 *                1h = BIOS FRB2
 *                2h = BIOS/POST
 *                3h = OS Load
 *                4h = SMS/OS
 *                5h = OEM
 *                Fh = unspecified
 *                all other = reserved
 *
 * @param[out] *reading - Pointer to a variable buffer that reports sensor
 *                        reading value.
 *
 * @return  Sensor status. Refer to enum ::SENSOR_STATUS
 *          SENSOR_STATUS::NORMAL,
 *          SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED,
 *          SENSOR_STATUS::ABSENT,
 *          SENSOR_STATUS::UNAVAILABLE,
 *          SENSOR_STATUS::ERROR
 *
 * @code Example code:
 *
 *  // BMC IPMI Watchdog sensor
 *  int32_t get_ipmi_wdt(double* reading)
 *  {
 *      return api_sensor_watchdog2(reading);
 *  }
 *
 * @endcode
 */
int32_t api_sensor_watchdog2(double* reading);

sdbusplus::bus::match::match register_watchdog2_event_handler(
    std::shared_ptr<sdbusplus::asio::connection> conn);
