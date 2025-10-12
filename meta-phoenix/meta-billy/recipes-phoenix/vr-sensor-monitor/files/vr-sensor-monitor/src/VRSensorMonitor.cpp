#include "VRSensorMonitor.hpp"
#include "Concrete/I2cOperation.hpp"
#include <iostream>
#include <utility>
#include <string>
#include <boost/asio.hpp>
#include <chrono>
#include <boost/asio/io_service.hpp>
#include <thread>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/message.hpp>
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
#include <variant>
#include <cstdio>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <systemd/sd-journal.h>
//#define DEBUG
const std::vector<uint8_t> addr_sensor = {0x60, 0x61, 0x62, 0x63};
std::string bus8 = std::to_string(8);
std::vector<SensorConfig> sensorConfigs;
std::map<std::string, uint8_t> lastValues;

std::map<std::string, std::deque<std::pair<uint8_t, std::chrono::system_clock::time_point>>> ringBuffers;
constexpr static size_t ringBufferSize = 600;

namespace fs = std::filesystem;

static constexpr const char* sensorMonitorService =
    "xyz.openbmc_project.PHX.VRSensor.Monitor";
static constexpr const char* sensorMonitorIface =
    "xyz.openbmc_project.PHX.VRSensor.Monitor";
static constexpr const char* sensorMonitorPath =
    "/xyz/openbmc_project/phx/vrsensor/monitor";


VRSensorMonitor::VRSensorMonitor(boost::asio::io_context& ioc,
                        std::shared_ptr<sdbusplus::asio::object_server> Server) :
    server(Server),
    filterTimer(ioc)
{
    registerDbusProperty();
    //CreateDefaultJson();
    reloadSensorConfig();
    setupRead();
}

void VRSensorMonitor::reloadSensorConfig()
{
    auto newConfigs = loadSensorConfig("/tmp/vr_controller.json");

    if (newConfigs.empty())
    {
        //std::cerr << "[WARN] No valid sensor configuration loaded.\n";
        return;
    }

    sensorConfigs = std::move(newConfigs);
    //std::cout << "[INFO] Reloaded " << sensorConfigs.size() << " sensors.\n";
}
void VRSensorMonitor::registerDbusProperty()
{
    auto iface = server->add_interface(sensorMonitorPath, sensorMonitorIface);
    
    iface->register_property_r(
        "VRValues",
        std::vector<std::string>{},
        sdbusplus::vtable::property_::emits_change,
        [this](const auto&) -> std::vector<std::string> {
            std::vector<std::string> result;
            
            for (const auto& [key, value] : lastValues)
            {
                int bus = 0, addr = 0, reg = 0;
                char c1, c2;
                std::istringstream iss(key);
                iss >> bus >> c1 >> addr >> c2 >> reg;
                
                std::ostringstream oss;
                oss << "vr_" << std::dec << bus << "_0x" 
                    << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << addr << "_0x"
                    << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << reg << "-0x"
                    << std::hex << std::uppercase << std::setw(2) << std::setfill('0') 
                    << static_cast<int>(value);
                
                result.push_back(oss.str());
            }
            
            return result;
        }
    );

    iface->register_method("LogReload", [this]() {
        reloadSensorConfig();
    });
    
    iface->initialize();
}

void VRSensorMonitor::setupRead()
{
    filterTimer.expires_after(std::chrono::seconds(1));
    filterTimer.async_wait([this](const boost::system::error_code& error) mutable {
        if (!error) {
            if (access("/tmp/GetVRLogToVar", F_OK) == 0)
            {
                DumpRingBufferToLog();
            }
            if (!sensorConfigs.empty())
            {
                for (auto& cfg : sensorConfigs)
                {
                    ReadState(cfg);
                }
                DumpRingBufferToLog();
            }
            setupRead();
        } else {
            std::cout << "Timer error: " << error.message() << std::endl;
        }
    });
}

std::vector<SensorConfig> VRSensorMonitor::loadSensorConfig(const std::string& filename)
{
    std::vector<SensorConfig> sensors;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        //std::cerr << "Failed to open " << filename << std::endl;
        return sensors;
    }

    json j;
    file >> j;

    for (auto& [name, arr] : j.items())
    {
        if (arr.size() != 4) continue;

        SensorConfig cfg;
        cfg.name = name;
        cfg.bus = std::stoi(arr[0].get<std::string>(), nullptr, 16);
        cfg.address = std::stoi(arr[1].get<std::string>(), nullptr, 16);
        cfg.reg = std::stoi(arr[2].get<std::string>(), nullptr, 16);
        cfg.type = arr[3].get<std::string>()[0];

        sensors.push_back(cfg);
    }

    return sensors;
}

void VRSensorMonitor::ReadState(const SensorConfig& cfg)
{
    size_t byteCount = 1;
    switch (cfg.type) {
        case 'b': byteCount = 1; break;
        case 'w': byteCount = 2; break;
        case 'd': byteCount = 4; break;
    }

    std::vector<uint8_t> wbuf = { cfg.reg };
    std::vector<uint8_t> rbuf(byteCount, 0);

#ifndef DEBUG
    int ret = i2cWriteRead("/dev/i2c-" + std::to_string(cfg.bus), cfg.address, wbuf, rbuf);
    if (ret != 0 || rbuf.empty()) return;
#else
    std::fill(rbuf.begin(), rbuf.end(), 0xAB); // debug value
#endif

    for (size_t i = 0; i < rbuf.size(); ++i)
    {
        uint8_t value = rbuf[i];

        std::string key = std::to_string(cfg.bus) + ":" +
                          std::to_string(cfg.address) + ":" +
                          std::to_string(cfg.reg);

        lastValues[key] = value;

        auto& buffer = ringBuffers[key];
        if (buffer.size() >= ringBufferSize)
            buffer.pop_front();
        buffer.push_back({value, std::chrono::system_clock::now()});
    }
}

void VRSensorMonitor::DumpRingBufferToLog()
{
    //std::ofstream ofs("/tmp/vr_controller.log", std::ios::app);
    std::ofstream ofs("/tmp/vr_controller.log", std::ios::trunc);
    if (!ofs)
    {
        //std::cerr << "Failed to open log file" << std::endl;
        return;
    }
    
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        int bus;
        int addr;
        int reg;
        uint8_t value;
    };
    
    std::vector<LogEntry> allEntries;
    
    for (const auto& [key, buffer] : ringBuffers)
    {
        size_t pos1 = key.find(':');
        size_t pos2 = key.rfind(':');
        std::string busStr = key.substr(0, pos1);
        std::string addrStr = key.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string regStr = key.substr(pos2 + 1);
        int bus = std::stoi(busStr);
        int addr = std::stoi(addrStr);
        int reg = std::stoi(regStr);
        
        for (const auto& sample : buffer)
        {
            allEntries.push_back({sample.second, bus, addr, reg, sample.first});
        }
    }
    
    std::sort(allEntries.begin(), allEntries.end(),
              [](const LogEntry& a, const LogEntry& b) {
                  auto a_sec = std::chrono::duration_cast<std::chrono::seconds>(
                      a.timestamp.time_since_epoch()).count();
                  auto b_sec = std::chrono::duration_cast<std::chrono::seconds>(
                      b.timestamp.time_since_epoch()).count();
                  
                  if (a_sec != b_sec)
                      return a_sec < b_sec;
                  
                  if (a.bus != b.bus)
                      return a.bus < b.bus;
                  if (a.addr != b.addr)
                      return a.addr < b.addr;
                  return a.reg < b.reg;
              });
    
    for (const auto& entry : allEntries)
    {
        auto ts = std::chrono::system_clock::to_time_t(entry.timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      entry.timestamp.time_since_epoch()) % 1000;
        std::tm tm;
        localtime_r(&ts, &tm);
        char timebuf[32];
        snprintf(timebuf, sizeof(timebuf),
                 "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
                 tm.tm_year + 1900,
                 tm.tm_mon + 1,
                 tm.tm_mday,
                 tm.tm_hour,
                 tm.tm_min,
                 tm.tm_sec,
                 static_cast<long>(ms.count()));
        
        ofs << timebuf
            << "-vr_sensor-0x" << std::hex << std::setw(2) << std::setfill('0') << entry.bus
            << "-0x" << std::setw(2) << entry.addr
            << "-0x" << std::setw(2) << entry.reg
            << "-0x" << std::setw(2) << static_cast<int>(entry.value)
            << std::dec << "\n";
    }
    
    ofs.close();
    std::remove("/tmp/GetVRLogToVar");
}
#if 0
void VRSensorMonitor::CreateDefaultJson()
{
    const std::string filePath = "/tmp/vr_controller.json";

        const std::string jsonContent = R"({
            "vr1": ["0x08", "0x60", "0x78", "b"],
            "vr2": ["0x08", "0x60", "0x79", "b"],
            "vr3": ["0x08", "0x60", "0x7a", "b"],
            "vr4": ["0x08", "0x60", "0x7b", "b"],
            "vr5": ["0x08", "0x61", "0x78", "b"],
            "vr6": ["0x08", "0x61", "0x79", "b"],
            "vr7": ["0x08", "0x61", "0x7a", "b"],
            "vr8": ["0x08", "0x61", "0x7b", "b"],
            "vr9": ["0x08", "0x62", "0x78", "b"],
            "vr10": ["0x08", "0x62", "0x79", "b"],
            "vr11": ["0x08", "0x62", "0x7a", "b"],
            "vr12": ["0x08", "0x62", "0x7b", "b"],
            "vr13": ["0x08", "0x63", "0x78", "b"],
            "vr14": ["0x08", "0x63", "0x79", "b"],
            "vr15": ["0x08", "0x63", "0x7a", "b"],
            "vr16": ["0x08", "0x63", "0x7b", "b"]
        })";

        std::ofstream ofs(filePath);
        if (!ofs)
        {
            std::cerr << "Failed to create file: " << filePath << std::endl;
            return;
        }

        ofs << jsonContent << std::endl;
        ofs.close();

        std::cout << "File created: " << filePath << std::endl;
}
#endif