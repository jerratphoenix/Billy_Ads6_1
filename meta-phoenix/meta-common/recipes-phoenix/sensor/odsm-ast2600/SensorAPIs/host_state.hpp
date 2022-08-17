/*
// Copyright (c) 2022 Phoenix Technologies Ltd.
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
 * @file    host_state.hpp
 *
 * @brief   Host State Sensor API
 *
 * @details This file contains Host State Sensor access functions for using in Phoenix sensor
 *          customization layer.
 */

#pragma once

#include <cstdint>

int32_t api_get_host_running(double* reading);

sdbusplus::bus::match::match register_host_state_event_handler(
    std::shared_ptr<sdbusplus::asio::connection> conn);

enum HOST_STATE {
    HOST_STATE_UNKNOW,
    HOST_STATE_RUNNING,
    HOST_STATE_OFF
};
