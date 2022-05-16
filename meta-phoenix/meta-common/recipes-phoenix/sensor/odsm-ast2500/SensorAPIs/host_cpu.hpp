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
 * @file    host_cpu.hpp
 *
 * @brief   Host CPU Sensor API
 *
 * @details This file contains Host CPU Sensor access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>
#include <string>

enum CPU_INDEX
{
    CPU_ID1 = 0,
    CPU_ID2,
    CPU_ID3,
    CPU_ID4,
    MAX_CPU_ID,
};

bool api_peci_ping(int cpu_index);
bool api_is_cpu_mca_err(uint8_t cpu_index);

int api_config_cpu_caterr_gpio(const std::string& name, const int polarity);
int api_config_cpu_err0_gpio(const std::string& name, const int polarity);
int api_config_cpu_err1_gpio(const std::string& name, const int polarity);
int api_config_cpu_err2_gpio(const std::string& name, const int polarity);
int api_config_cpu1_present_gpio(const std::string& name, const int polarity);
int api_config_cpu2_present_gpio(const std::string& name, const int polarity);

int api_config_cpu_is_on_chip(bool on_chip);

bool api_is_cpu_caterr_gpio_assert(void);
bool api_is_cpu_err0_gpio_assert(void);
bool api_is_cpu_err1_gpio_assert(void);
bool api_is_cpu_err2_gpio_assert(void);

bool api_is_cpu1_present(void);
bool api_is_cpu2_present(void);
