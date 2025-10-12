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
 * @file    adc.hpp
 *
 * @brief   ASPEED ADC Sensor API
 *
 * @details This file contains ASPEED ADC access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>

/** @enum Aspeed ADC Controller
 */
enum ADC_CONTROLLER {
    AST2500_ADC,        /**< AST2500 ADC controller */
    AST2600_ADC,        /**< AST2600 ADC controller */
};

/** @enum Aspeed ADC Channel
 */
enum AST_ADC {
    ADC_CHANNEL_0 = 0,  /**< ADC channel 0 analog input */
    ADC_CHANNEL_1,      /**< ADC channel 1 analog input */
    ADC_CHANNEL_2,      /**< ADC channel 2 analog input */
    ADC_CHANNEL_3,      /**< ADC channel 3 analog input */
    ADC_CHANNEL_4,      /**< ADC channel 4 analog input */
    ADC_CHANNEL_5,      /**< ADC channel 5 analog input */
    ADC_CHANNEL_6,      /**< ADC channel 6 analog input */
    ADC_CHANNEL_7,      /**< ADC channel 7 analog input */
    ADC_CHANNEL_8,      /**< ADC channel 8 analog input */
    ADC_CHANNEL_9,      /**< ADC channel 9 analog input */
    ADC_CHANNEL_10,     /**< ADC channel 10 analog input */
    ADC_CHANNEL_11,     /**< ADC channel 11 analog input */
    ADC_CHANNEL_12,     /**< ADC channel 12 analog input */
    ADC_CHANNEL_13,     /**< ADC channel 13 analog input */
    ADC_CHANNEL_14,     /**< ADC channel 14 analog input */
    ADC_CHANNEL_15,     /**< ADC channel 15 analog input */
};

/**
 * @Name   api_get_adc()
 *
 * @Description    This function can read ASPEED BMC ADC(Analog-to-Digital
 *                 Convertor) channel and report as a real world value,
 *                 eg, 3.3 Volt, 12 Volt, ....
 *
 * @param[in] controller - The controller.
 *                         refer to enum ::ADC_CONTROLLER
 * @param[in] channel    - The ADC channel.
 *                         refer to enum ::AST_ADC
 * @param[out] *value    - Pointer to buffer into which data will be read.
 * @param[in] r1         - The R1 resistor value from schematic.
 * @param[in] r2         - The R2 resistor value from schematic.
 *
 * @return  ADC channel status.
 *          On success, api_get_adc() returns 0;
 *          on error, it returns a nonzero value.
 *
 * @code Example code:
 *
 * int32_t get_3V3_AUX(double* reading)
 * {
 *     int32_t ret;
 *     ret = api_get_adc (ADC_CONTROLLER::AST2500_ADC, AST_ADC::ADC_CHANNEL_7, reading, 1000, 3000);
 *
 *     if (ret != 0)
 *     {
 *          return SENSOR_STATUS::ERROR;
 *     }
 *
 *     return SENSOR_STATUS::NORMAL;
 * }
 *
 * @endcode
 */
int32_t api_get_adc(uint8_t controller, uint8_t channel, double *value, int r1, int r2);

