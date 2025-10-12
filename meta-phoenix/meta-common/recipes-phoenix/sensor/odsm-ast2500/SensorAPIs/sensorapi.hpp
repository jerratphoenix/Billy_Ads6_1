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
 * @file    sensorapi.hpp
 *
 * @brief   Phoenix Sensor API General File
 *
 * @details This file contains all Phoenix Sensor API General Declaration for
 *          using in Phoenix sensor customization layer.
 */

#pragma once

#include "adc.hpp"
#include "gpio.hpp"
#include "sel.hpp"
#include "bmc_state.hpp"
#include "chassis_state.hpp"
#include "sysfs.hpp"
#include "host_cpu.hpp"
#include "bmc_update.hpp"
#include "bmc_factory_reset.hpp"
#include "watchdog2.hpp"
#include "set_sel_time_event.hpp"
#include "pef.hpp"
#include "host_state.hpp"
#include "dimm_presence.hpp"
#include "i2c.hpp"
#include "smbus.hpp"
#include "tmp75.hpp"

#define BIT(n) (1 << n)

enum SENSOR_STATUS
{
    NORMAL,
    NORMAL_AND_EVENT_HANDLED,
    ABSENT,
    UNAVAILABLE,
    ERROR,
};

