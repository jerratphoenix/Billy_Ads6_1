#include "HostStateMonitor.hpp"
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

static constexpr const char* sensorMonitorService =
    "xyz.openbmc_project.PHX.HostState.Monitor";
static constexpr const char* sensorMonitorIface =
    "xyz.openbmc_project.PHX.HostState.Monitor";
static constexpr const char* sensorMonitorPath =
    "/xyz/openbmc_project/phx/hoststate/monitor";

using namespace std;
namespace fs = std::filesystem;
using namespace std::chrono;
bool firstTime = true;
bool lastpowerState = false;

HostStateMonitor::HostStateMonitor(boost::asio::io_context& ioc,
                        std::shared_ptr<sdbusplus::asio::object_server> Server) :
    server(Server),
    filterTimer(ioc)
{
    registerDbusMethod();
    setupRead();
}

void HostStateMonitor::registerDbusMethod()
{
    iface = server->add_interface(sensorMonitorPath, sensorMonitorIface);
    iface->register_method("CrashdumpTrigger", [this]() {
        //std::cout << "[DBus] TriggerAction called!" << std::endl;
        if (access("/tmp/crashdumpFlag", F_OK) == 0)
        {
            eventTrigger();
        }
    });
    iface->register_method("LogDump", [this]() {
        //std::cout << "[DBus] TriggerAction called!" << std::endl;
        if (access("/tmp/voltage_dump.log", F_OK) == 0)
        {
            try
            {
                fs::copy_file("/tmp/voltage_dump.log",
                    "/var/log/voltage_dump.log",
                    fs::copy_options::overwrite_existing);
                //std::cout << "Copied /tmp/voltage_dump.log -> /var/log/voltage_dump.log\n";
            }
            catch (const fs::filesystem_error& e)
            {
                std::cerr << "Copy failed: " << e.what() << "\n";
            }
        }
        if (access("/tmp/vr_controller.log", F_OK) == 0)
        {
            try
            {
                fs::copy_file("/tmp/vr_controller.log",
                    "/var/log/vr_controller.log",
                    fs::copy_options::overwrite_existing);
                //std::cout << "Copied /tmp/vr_controller.log -> /var/log/vr_controller.log\n";
            }
            catch (const fs::filesystem_error& e)
            {
                std::cerr << "Copy failed: " << e.what() << "\n";
            }
        }
    });
    iface->initialize();
}

void HostStateMonitor::eventTrigger()
{
    
    if (!isLogging.exchange(true)) 
    { 
        isLogging = true; 
        std::thread([this]() {
            if (access("/tmp/crashdumpFlag", F_OK) == 0)
            {
                /* code */
                int ret = system("/usr/bin/ParserLog.sh crashdump"); 
                if (ret != 0) 
                { 
                    std::cerr << "Failed to execute log script, ret=" << ret << std::endl; 
                }
                std::remove("/tmp/crashdumpFlag");
                isLogging = false;
            }
            else
            {
                int ret = system("/usr/bin/ParserLog.sh poweroff"); 
                if (ret != 0) 
                { 
                    std::cerr << "Failed to execute log script, ret=" << ret << std::endl; 
                }
                isLogging = false;
            }
        }).detach();
    } 
    
}

bool HostStateMonitor::isPowerOn()
{
    auto bus = sdbusplus::bus::new_default();

        auto method = bus.new_method_call(
            "xyz.openbmc_project.State.Host0",                 // bus name
            "/xyz/openbmc_project/state/host0",               // object path
            "org.freedesktop.DBus.Properties",                // interface
            "Get");                                           // method

        method.append("xyz.openbmc_project.State.Host");      // interface name
        method.append("CurrentHostState");                   // property name

        std::variant<std::string> value;
        try
        {
            auto reply = bus.call(method);
            reply.read(value);
        }
        catch (const sdbusplus::exception::SdBusError& e)
        {
            //std::cerr << "D-Bus call failed: " << e.what() << std::endl;
            return 1;
        }

        std::string hostState = std::get<std::string>(value);
        std::cout << "CurrentHostState: " << hostState << std::endl;

        if (hostState == "xyz.openbmc_project.State.Host.HostState.Running")
        {
            //std::cout << "Host is running!" << std::endl;
            return true;
        }
        else
        {
            /* code */
            //std::cout << "Host is not running." << std::endl;
            return false;
        }
}

void HostStateMonitor::setupRead()
{
    filterTimer.expires_after(std::chrono::seconds(1));
    filterTimer.async_wait([this](const boost::system::error_code& error) mutable {
        if (!error) {
            bool pState = isPowerOn();
            if (pState && firstTime)
            {
                firstTime = false;
                lastpowerState = pState;
            }
            else if (!pState && !firstTime)
            {
                if (pState)
                {
                    if(lastpowerState != pState)
                    {
                        lastpowerState = pState;
                    }
                }
                else
                {
                    if(lastpowerState != pState)
                    {
                        lastpowerState = pState;
                        if (access("/tmp/crashdumpFlag", F_OK) != 0)
                        {
                            eventTrigger();
                        }
                    }
                }
                
            }
            setupRead();
        } else {
            std::cout << "Timer error: " << error.message() << std::endl;
        }
    });
}