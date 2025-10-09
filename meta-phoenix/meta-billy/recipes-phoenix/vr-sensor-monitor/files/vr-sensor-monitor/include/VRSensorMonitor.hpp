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

using json = nlohmann::ordered_json;

struct SensorConfig {
    std::string name;
    uint8_t bus;
    uint8_t address;
    uint8_t reg;
    char type;  // 'b' = 1 byte
};

class VRSensorMonitor
{
  public:
    explicit VRSensorMonitor(boost::asio::io_context& ioc,
      std::shared_ptr<sdbusplus::asio::object_server> Server);
    void setupRead();
    void Initialize();
    void ReadState(const SensorConfig& cfg);
    void CreateDefaultJson();
    void registerDbusProperty();
    std::vector<SensorConfig> loadSensorConfig(const std::string& filename);

  private:
    std::shared_ptr<sdbusplus::asio::object_server> server;
    std::shared_ptr<sdbusplus::asio::dbus_interface> iface;
    boost::asio::steady_timer filterTimer;

};
