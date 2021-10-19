/*
 * Copyright (c) 2021 Phoenix Technologies Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <memory>

#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/server.hpp>

namespace phoenix
{
namespace dbus
{
namespace monitor
{

#define PEF_CTL_EN 0x01
#define FILTER_TABLE_EN 0x80
#define UNSPECIFIED 0xFF

#define PEF_ACTION_ALERT	(1 << 0)    //TODO
#define PEF_ACTION_POWER_DOWN	(1 << 1)
#define PEF_ACTION_POWER_RESET	(1 << 2)
#define PEF_ACTION_POWER_CYCLE	(1 << 3)

constexpr const char* sevice_chassis = "xyz.openbmc_project.State.Chassis";
constexpr const char* objpath_chassis = "/xyz/openbmc_project/state/chassis0";
constexpr const char* interf_chassis = "xyz.openbmc_project.State.Chassis";

constexpr const char* sevice_host = "xyz.openbmc_project.State.Host";
constexpr const char* objpath_host = "/xyz/openbmc_project/state/host0";
constexpr const char* interf_host = "xyz.openbmc_project.State.Host";

constexpr const char* property_interface_name = "org.freedesktop.DBus.Properties";

constexpr const char* sensor_discrete_interface_name = "xyz.openbmc_project.Sensor.Discrete.Event";
constexpr const char* sensor_value_interface_name = "xyz.openbmc_project.Sensor.Value";

static constexpr size_t selEvtDataMaxSize = 3;

}//monitor
}//dbus
}//phoenix
