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

#include "type.hpp"
#include "debug.hpp"

namespace phoenix
{
namespace dbus
{
namespace monitor
{

static bool last_power_good;

struct VariantToStrVisitor {
    template <typename T> std::string operator()(const T& t) const
    {
        return static_cast<std::string>(t);
    }
};

inline static bool is_power_on(void)
{
    DPRINT ("%s!!\n", __func__);

    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);
    bool powerGood = false;

    auto method = conn->new_method_call(
                      sevice_chassis,
                      objpath_chassis,
                      property_interface_name,
                      "GetAll");

    method.append("xyz.openbmc_project.State.Chassis");
    boost::container::flat_map<std::string, std::variant<std::string>> chassisStatus;
    try {
        sdbusplus::message::message getChassisStatusResp = conn->call(method);
        getChassisStatusResp.read(chassisStatus);
    }

    catch (sdbusplus::exception::SdBusError& e) {
        DPRINT ( "Get chassisStatus error\n");
        return false;
    }

    auto findVal = chassisStatus.find("CurrentPowerState");
    std::string powerState;
    if (findVal != chassisStatus.end()) {
        powerState = std::visit(VariantToStrVisitor(), findVal->second);
    }

    if (powerState == "xyz.openbmc_project.State.Chassis.PowerState.On")
    {
        powerGood = true;
    } else {
        powerGood = false;
    }

    if (last_power_good != powerGood)
    {
        last_power_good = powerGood;
    }

    return powerGood;
}

inline static void power_on(void)
{
    DPRINT ("%s!!\n", __func__);

    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);

    conn->async_method_call(
    [](boost::system::error_code ec) {
        if (ec)
        {
            std::cerr << "failed to set Chassis State power on\n";
            return;
        }
    },
    sevice_chassis,
    objpath_chassis,
    property_interface_name, "Set",
    interf_chassis, "RequestedPowerTransition",
    std::variant<std::string>{
        "xyz.openbmc_project.State.Chassis.Transition.On"});
}

inline static void power_off(void)
{
    DPRINT ("%s!!\n", __func__);

    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);

    conn->async_method_call(
    [](boost::system::error_code ec) {
        if (ec)
        {
            std::cerr << "failed to set Chassis State power off\n";
            return;
        }
    },
    sevice_chassis,
    objpath_chassis,
    property_interface_name, "Set",
    interf_chassis, "RequestedPowerTransition",
    std::variant<std::string>{
        "xyz.openbmc_project.State.Chassis.Transition.Off"});
}

inline static void power_cycle(void)
{
    DPRINT ("%s!!\n", __func__);

    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);

    conn->async_method_call(
    [](boost::system::error_code ec) {
        if (ec)
        {
            std::cerr << "failed to set Chassis State power cycle\n";
            return;
        }
    },
    sevice_chassis,
    objpath_chassis,
    property_interface_name, "Set",
    interf_chassis, "RequestedPowerTransition",
    std::variant<std::string>{
        "xyz.openbmc_project.State.Chassis.Transition.PowerCycle"});
}

inline static void hard_reset(void)
{
    DPRINT ("%s!!\n", __func__);

    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);

    conn->async_method_call(
    [](boost::system::error_code ec) {
        if (ec)
        {
            std::cerr << "failed to set Chassis State power cycle\n";
            return;
        }
    },
    sevice_host,
    objpath_host,
    property_interface_name, "Set",
    interf_host, "RequestedHostTransition",
    std::variant<std::string>{
        "xyz.openbmc_project.State.Host.Transition.ForceWarmReboot"});
}

}//monitor
}//dbus
}//phoenix

