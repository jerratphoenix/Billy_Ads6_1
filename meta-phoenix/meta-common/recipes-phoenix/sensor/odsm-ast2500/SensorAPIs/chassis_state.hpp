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
 * @file    chassis_state.hpp
 *
 * @brief   Chassis State Sensor API
 *
 * @details This file contains Chassis State Sensor access functions for using
 *          in Phoenix sensor customization layer.
 */

#pragma once

#include <stdlib.h>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/server.hpp>

/**
 * @Name   api_get_power_on()
 *
 * @Description    This function detect chassis current power state is PowerOn
 *                 or not.
 *
 * @return  chassis power state is PowerOn or PowerOff.
 *          On power state is PowerOn, it returns true;
 *          others return false.
 */
bool api_get_power_on (void);

sdbusplus::bus::match::match register_chassis_state_change_handler(
    std::shared_ptr<sdbusplus::asio::connection> conn);

