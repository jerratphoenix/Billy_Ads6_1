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

int main()
{
    boost::asio::io_context io;
    auto bus = std::make_shared<sdbusplus::asio::connection>(io);
    auto server = std::make_shared<sdbusplus::asio::object_server>(bus, true);
    SensorManager app(io, bus, server);

    io.run();
    return 0;
}
