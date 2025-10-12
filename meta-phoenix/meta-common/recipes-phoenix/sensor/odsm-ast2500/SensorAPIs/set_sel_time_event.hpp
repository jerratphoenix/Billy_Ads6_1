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
 * @file    set_sel_time_event.hpp
 *
 * @brief   BMC Set SEL Time Sensor API
 *
 * @details This file contains BMC Set SEL Time Sensor access functions for
 *          using in Phoenix sensor customization layer.
 */

#pragma once

#include <cstdint>

int32_t api_sensor_sel(double* reading);

sdbusplus::bus::match::match register_sel_event_handler(
    std::shared_ptr<sdbusplus::asio::connection> conn);
