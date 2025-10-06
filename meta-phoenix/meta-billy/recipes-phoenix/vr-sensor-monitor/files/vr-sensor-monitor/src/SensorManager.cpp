#include "SensorManager.hpp"
#include <string>
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <boost/asio/io_service.hpp>
#include <thread>
#include <boost/algorithm/string/replace.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/bus/match.hpp>
#include <boost/asio/steady_timer.hpp>
#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <regex>
#include <utility>
#include <variant>
#include <vector>

static constexpr const char* sensorMonitorService =
    "xyz.openbmc_project.PHX.VRSensor.Monitor";
static constexpr const char* sensorMonitorIface =
    "xyz.openbmc_project.PHX.VRSensor.Monitor";
static constexpr const char* sensorMonitorPath =
    "/xyz/openbmc_project/phx/vrsensor/monitor";
static constexpr const char* sensorsManagerPath =
    "/xyz/openbmc_project/sensors";

SensorManager::SensorManager(boost::asio::io_context& ioc,
                         std::shared_ptr<sdbusplus::asio::connection> Bus,
                         std::shared_ptr<sdbusplus::asio::object_server> Server) :
    bus(std::move(Bus)), server(std::move(Server)),signals(ioc, SIGINT, SIGTERM)
{

    bus->request_name(sensorMonitorService);
    iface = server->add_interface(sensorMonitorPath, sensorMonitorIface);
    iface->initialize();
    server->add_manager(sensorMonitorPath);
    server->add_manager(sensorsManagerPath);

    signals.async_wait([&](const boost::system::error_code ec, const int&) {
        if (ec)
        {
            throw std::runtime_error("Signal should not be cancelled.");
        }
        ioc.stop();
    });
    vrsensorMonitor = std::make_unique<VRSensorMonitor>(ioc,server);
    

}
