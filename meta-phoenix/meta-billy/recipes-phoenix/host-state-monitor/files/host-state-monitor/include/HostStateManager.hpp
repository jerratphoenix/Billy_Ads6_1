#pragma once
#include "HostStateMonitor.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
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

class HostStateManager
{
  public:
    explicit HostStateManager(boost::asio::io_context& ioc,
                         std::shared_ptr<sdbusplus::asio::connection> Bus,
                         std::shared_ptr<sdbusplus::asio::object_server> Server);
    
  
  private:
    std::shared_ptr<sdbusplus::asio::connection> bus;
    std::shared_ptr<sdbusplus::asio::object_server> server;
    std::shared_ptr<sdbusplus::asio::dbus_interface> iface;
    boost::asio::signal_set signals;
    std::unique_ptr<HostStateMonitor> hoststateMonitor;
};
