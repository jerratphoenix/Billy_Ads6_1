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
#define DEBUG
const std::vector<uint8_t> addr_sensor = {0x60, 0x61, 0x62, 0x63};
std::string bus8 = std::to_string(8);
std::vector<SensorConfig> sensorConfigs;
std::unordered_map<std::string, uint8_t> lastValues;

VRSensorMonitor::VRSensorMonitor(boost::asio::io_context& ioc,
                        std::shared_ptr<sdbusplus::asio::object_server> Server) :
    server(Server),
    filterTimer(ioc)
{
    CreateDefaultJson();
    setupRead();
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
    size_t byteCount = 1;
    switch (cfg.type) {
        case 'b': byteCount = 1; break;
        case 'w': byteCount = 2; break;
        case 'd': byteCount = 4; break;
    }

    std::vector<uint8_t> wbuf = { cfg.reg };
    std::vector<uint8_t> rbuf(byteCount, 0);

    int ret = i2cWriteRead("/dev/i2c-" + std::to_string(cfg.bus), cfg.address, wbuf, rbuf);
    #ifdef DEBUG
    std::cout << "[DEBUG] bus=0x" << std::hex << int(cfg.bus)
              << " addr=0x" << int(cfg.address)
              << " reg=0x" << int(cfg.reg)
              << " type=" << cfg.type
              << " i2cWriteRead ret=" << std::dec << ret
              << " readBuf:";
    for (size_t i = 0; i < rbuf.size(); ++i)
    {
        std::cout << " 0x" << std::hex << std::setw(2) << std::setfill('0') << int(rbuf[i]);
    }
    std::cout << std::dec << std::endl;
    #endif
    if (ret != 0 || rbuf.empty()) return;

    for (size_t i = 0; i < rbuf.size(); ++i) {
        uint8_t value = rbuf[i];

        std::string key = std::to_string(cfg.bus) + ":" +
                          std::to_string(cfg.address) + ":" +
                          std::to_string(cfg.reg);

        if (lastValues[key] != value) {
            lastValues[key] = value;

            sd_journal_send(
                "VR_BUS=%02X", (unsigned)cfg.bus,
                "VR_ADDR=%02X", (unsigned)cfg.address,
                "VR_REG=%02X", (unsigned)cfg.reg,
                "VR_VALUE=%02X", (unsigned)value,
                NULL
            );
        }
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