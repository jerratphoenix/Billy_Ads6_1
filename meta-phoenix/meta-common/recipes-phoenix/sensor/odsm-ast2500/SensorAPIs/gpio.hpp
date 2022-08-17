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
 * @file    gpio.hpp
 *
 * @brief   GPIO API
 *
 * @details This file contains GPIO access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>
#include <string>

/**
 * @Name   api_set_gpio()
 *
 * @Description    This function contorl OpenBMC Linux GPIO line output.
 *
 * @param[in] name      - The GPIO line name, that can be declared in kernel
 *                        device tree.
 * @param[in] value     - The GPIO line value.
 * @param[in] polarity  - The GPIO line polarity.
 *                        enum  gpiod::line::ACTIVE_HIGH,
 *                              gpiod::line::ACTIVE_LOW
 *
 * @return  GPIO control status.
 *          On success, api_set_gpio() returns 0;
 *          on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 *  // Set BOARD_LED1 GPIO value to active
 *  api_set_gpio ("BOARD_LED1", 1, gpiod::line::ACTIVE_LOW);
 *
 * @endcode
 */
int api_set_gpio(const std::string& name, int value, const int polarity);

/**
 * @Name   api_get_gpio()
 *
 * @Description    This function can read OpenBMC Linux GPIO line value.
 *
 *
 * @param[in]  name      - The GPIO line name, that can be declared in kernel
 *                         device tree.
 * @param[out] *value    - The GPIO line value.
 * @param[in]  polarity  - The GPIO line polarity. Refer to enum ::gpiod::line
 *                         gpiod::line::ACTIVE_HIGH,
 *                         gpiod::line::ACTIVE_LOW
 *
 * @return  GPIO control status.
 *          On success, api_get_gpio() returns 0;
 *          on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 *  // Get BOARD_LED1 GPIO value
 *  int value;
 *  api_get_gpio ("BOARD_LED1", &value, gpiod::line::ACTIVE_LOW);
 *
 * @endcode
 */
int api_get_gpio(const std::string& name, int *value, const int polarity);

