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
 * @file    i2c.hpp
 *
 * @brief   I2C API
 *
 * @details This file contains I2c access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>

/**
 * @Name api_i2c_write_read()
 *
 * @Description This function do I2C combined write and read transaction
 *              without i2c stop signal in between.
 *
 * @param[in]  bus            - The I2C channel on which the slave is connected.
 * @param[in]  addr           - The I2C address of slave peripheral, that without
 *                              write or read bit. Address range is 0x01 ~ 0x7F.
 * @param[in]  write_count    - The count of data bytes to be written.
 * @param[in]  *write_buffer  - Pointer to buffer from which data will be written
 *                              to slave.
 * @param[in]  read_count     - The count of data bytes to be read.
 * @param[out] *read_buffer   - Pointer to buffer into which data will be read.
 * @param[in]  retry          - The retry count when I2C transaction failed.
 * @param[in]  retry_delay_ms - The millisecond delay before do retry.
 *
 * @return I2C transaction status.
 *         On success, api_i2c_write_read() returns 0;
 *         on error, it returns a nonzero value.
 *
 * @code Example code #1:
 *
 *  // Access I2C bus 6 address 0x50, write 1 byte and read 1 byte to rd_buff.
 *  uint8_t wd_buff, rd_buff;
 *  wd_buff = 0x41;
 *  api_i2c_write_read (6, 0x50, 1, &wd_buff, 1, &rd_buff, 0, 0);
 *
 * @endcode
 *
 * @code Example code #2:
 *
 *  // Access I2C bus 0 address 0x5A, write 3 byte and read 10 byte to rd_buff.
 *  uint8_t wd_buff[3], rd_buff[10];
 *  wd_buff[0] = 0x41;
 *  wd_buff[1] = 0x42;
 *  wd_buff[2] = 0x43;
 *  api_i2c_write_read (0, 0x5A, 3, wd_buff, 10, rd_buff, 0, 0);
 *
 * @endcode
 */
int32_t api_i2c_write_read(uint8_t bus, uint8_t addr, int write_count,
                           uint8_t* write_buffer, int read_count,
                           uint8_t* read_buffer, int retry, int retry_delay_ms);

/**
 * @Name api_i2c_set_bus_timeout()
 *
 * @Description This function use to change the default i2c bus timeout configuration
 *              in kernel i2c-dev interface.
 *
 * @param[in]  bus            - The I2C channel on which the slave is connected.
 * @param[in]  timeout_ms     - The I2C timeout, that restricted by kernel
 *                              driver limitations, needs to be a multiple of 10 ms.
 *
 * @return I2C setting status.
 *         On success, api_i2c_set_bus_timeout() returns 0;
 *         on error, it returns a nonzero value.
 */
int32_t api_i2c_set_bus_timeout(uint8_t bus, uint32_t timeout_ms);

/**
 * @Name api_i2c_set_bus_retry()
 *
 * @Description This function use to change the default i2c bus retry configuration
 *              in kernel i2c-dev interface.
 *
 * @param[in]  bus            - The I2C channel on which the slave is connected.
 * @param[in]  retry          - The I2C retry.
 *
 * @return I2C setting status.
 *         On success, api_i2c_set_bus_retry() returns 0;
 *         on error, it returns a nonzero value.
 */
int32_t api_i2c_set_bus_retry(uint8_t bus, uint32_t retry);

