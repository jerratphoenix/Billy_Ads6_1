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
    CreateDefaultJson();
    setupRead();
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
    
    iface->initialize();
}

void VRSensorMonitor::setupRead()
{
    filterTimer.expires_after(std::chrono::seconds(1));
    filterTimer.async_wait([this](const boost::system::error_code& error) mutable {
        if (!error) {
            sensorConfigs = loadSensorConfig("/tmp/vr_controller.json");
            for (auto& cfg : sensorConfigs) {
                ReadState(cfg);
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
        std::cerr << "Failed to open " << filename << std::endl;
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
    using namespace std::chrono;
    static auto lastRotate = steady_clock::now();

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
    std::fill(rbuf.begin(), rbuf.end(), 0x00);
#endif
    auto now = steady_clock::now();
    if (duration_cast<minutes>(now - lastRotate).count() > 10) {
        std::ofstream ofs("/tmp/vr_controller.log", std::ios::trunc);
        lastRotate = now;
    }

    std::ofstream ofs("/tmp/vr_controller.log", std::ios::app);
    if (!ofs) return;

    auto t = system_clock::now();
    std::time_t tt = system_clock::to_time_t(t);
    auto ms = duration_cast<milliseconds>(t.time_since_epoch()) % 1000;

    for (size_t i = 0; i < rbuf.size(); ++i) {
        uint8_t value = rbuf[i];
        std::string key = std::to_string(cfg.bus) + ":" +
                          std::to_string(cfg.address) + ":" +
                          std::to_string(cfg.reg);
        lastValues[key] = value;
        #ifdef DEBUG
        // --- Debug print ---
        std::cout << "[DEBUG] bus=0x" << std::hex << int(cfg.bus)
        << " addr=0x" << int(cfg.address)
        << " reg=0x" << int(cfg.reg)
        << " value=0x" << int(value) << std::endl;
        // --------------------
        #endif
        std::tm tm;
        localtime_r(&tt, &tm);
        ofs << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
            << "." << std::setw(3) << std::setfill('0') << ms.count()
            << "-vr_sensor-0x" << std::hex << std::setw(2) << std::setfill('0') << int(cfg.bus)
            << "-0x" << std::setw(2) << int(cfg.address)
            << "-0x" << std::setw(2) << int(cfg.reg)
            << "-0x" << std::setw(2) << int(value) << std::endl;
    }
}

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