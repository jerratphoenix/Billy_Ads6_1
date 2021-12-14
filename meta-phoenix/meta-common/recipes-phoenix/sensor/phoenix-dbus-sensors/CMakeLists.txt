project (phoenix-dbus-sensors)

cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

#set (CMAKE_BUILD_TYPE DEBUG)
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

include_directories (${CMAKE_CURRENT_SOURCE_DIR} SensorAPIs)

set(SENSOR_API_SRCS
    SensorAPIs/sysfs.hpp
    SensorAPIs/sysfs.cpp
    SensorAPIs/adc.hpp
    SensorAPIs/adc.cpp
    SensorAPIs/gpio.hpp
    SensorAPIs/gpio.cpp
    SensorAPIs/sel.hpp
    SensorAPIs/sel.cpp
    SensorAPIs/bmc_state.hpp
    SensorAPIs/bmc_state.cpp
    SensorAPIs/chassis_state.hpp
    SensorAPIs/chassis_state.cpp
    SensorAPIs/host_cpu.hpp
    SensorAPIs/host_cpu.cpp
    SensorAPIs/bmc_update.hpp
    SensorAPIs/bmc_update.cpp
    SensorAPIs/bmc_factory_reset.hpp
    SensorAPIs/bmc_factory_reset.cpp
    SensorAPIs/watchdog2.hpp
    SensorAPIs/watchdog2.cpp
)

set(SRCS
    phoenix-dbus-sensors.cpp
    phoenix-dbus-sensors.hpp
    raw_reading.cpp
    raw_reading.hpp
    debug.hpp
    ipmisdr.hpp
    matchhandle.hpp
)

add_executable (phoenix-dbus-sensors
    ${SRCS}
    ${SENSOR_API_SRCS}
)

target_link_libraries (phoenix-dbus-sensors -lsystemd)
target_link_libraries (phoenix-dbus-sensors sdbusplus)
target_link_libraries (phoenix-dbus-sensors pthread)
target_link_libraries (phoenix-dbus-sensors stdc++fs)
target_link_libraries (phoenix-dbus-sensors bsd)
target_link_libraries (phoenix-dbus-sensors gpiodcxx)
target_link_libraries (phoenix-dbus-sensors i2c)
target_link_libraries (phoenix-dbus-sensors peci)
