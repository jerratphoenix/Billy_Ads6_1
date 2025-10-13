#pragma once
#include <sdbusplus/asio/object_server.hpp>
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
#include <nlohmann/json.hpp>

class HostStateMonitor
{
  public:
    explicit HostStateMonitor(boost::asio::io_context& ioc,
      std::shared_ptr<sdbusplus::asio::object_server> Server);
    void setupRead();
    void eventTrigger();
    void registerDbusMethod();
    bool isPowerOn();
    std::atomic<bool> isLogging{false};


  private:
    std::shared_ptr<sdbusplus::asio::object_server> server;
    std::shared_ptr<sdbusplus::asio::dbus_interface> iface;
    boost::asio::steady_timer filterTimer;

};
