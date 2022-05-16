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
 * @file    tmp75.hpp
 *
 * @brief   TMP75 API
 *
 * @details This file contains TMP75 or TMP175 or LM75 access functions for using in
 *          Phoenix sensor customization layer.
 */

#pragma once

#include <cstdint>

/**
 * @Name api_get_tmp75_temperature()
 *
 * @Description Reads TMP75 temperature and this function automatically selects
 *              the most suitable reading method. If the device already exists
 *              in kernel device tree list, this function will read through
 *              the kobject via sysfs.
 *
 * @param[in]  bus            - The I2C channel on which the slave is connected.
 * @param[in]  addr           - The I2C address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[out] *value         - Pointer to buffer into which data will be read.
 *
 * @return Status.
 *         On success, api_get_tmp75_temperature() returns 0;
 *         on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 *  // Get TMP75 temperature in i2c bus 6 address 0x4c to readbuf variable.
 *  double readbuf;
 *  api_get_tmp75_temperature (6, 0x4a, &readbuf);
 *  printf("readbuf is %lf", readbuf);
 * @endcode
 */
int32_t api_get_tmp75_temperature(uint8_t bus, uint8_t address, double *value);

