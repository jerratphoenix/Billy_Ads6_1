/*
// Copyright (c) 2021 Phoenix Technologies Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#pragma once

#include <boost/algorithm/string.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/server.hpp>

constexpr const char* phoenix_sensor_service =
    "xyz.openbmc_project.phoenix-dbus-sensors";
constexpr const char* operational_status_interface_name =
    "xyz.openbmc_project.State.Decorator.OperationalStatus";
constexpr const char* property_interface_name =
    "org.freedesktop.DBus.Properties";
constexpr const char* association_interface_name =
    "xyz.openbmc_project.Association.Definitions";
constexpr const char* available_interface_name =
    "xyz.openbmc_project.State.Decorator.Availability";
constexpr const char* operational_interface_name =
    "xyz.openbmc_project.State.Decorator.OperationalStatus";
constexpr const char* item_board_intferface_name =
    "xyz.openbmc_project.Inventory.Item.Board";

enum SENSOR_STATUS
{
    NORMAL,
    NORMAL_AND_EVENT_HANDLED,
    ABSENT,
    UNAVAILABLE,
    ERROR,
};

enum LEVEL
{
    WARNING,
    CRITICAL
};

enum DIRECTION
{
    HIGH,
    LOW
};

enum DISCRETE_OFFSET
{
    OFFSET_0 = 0,
    OFFSET_1,
    OFFSET_2,
    OFFSET_3,
    OFFSET_4,
    OFFSET_5,
    OFFSET_6,
    OFFSET_7,
    OFFSET_8,
    OFFSET_9,
    OFFSET_10,
    OFFSET_11,
    OFFSET_12,
    OFFSET_13,
    OFFSET_14,
    OFFSET_15,
};

// Unit type code from IPMI spec
enum UNIT_TYPE_CODE
{
    UNSPECIFIED = 0,
    DEGREES_C = 1,
    DEFREES_F = 2,
    DEFREES_K = 3,
    VOLTS = 4,
    AMPS = 5,
    WATTS = 6,
    RPM = 18,
};

enum STATUS_CODE
{
    STATUS_OK = 0,
    STATUS_SDR_FILE_FAILED,
    STATUS_SEMPHORE_ERROR,
    STATUS_FILE_NAME_NULL,
    STATUS_FILE_NOT_EXIST,
    STATUS_FILE_OPEN_ERROR,
    STATUS_ALLOCATE_ERROR,
    STATUS_POINT_NULL,
    STATUS_ERROR,
};

#define BIT(n) (1 << n)

struct sensor_var_t
{
    std::shared_ptr<sdbusplus::asio::dbus_interface> value_intf;
    std::shared_ptr<sdbusplus::asio::dbus_interface> warn_intf;
    std::shared_ptr<sdbusplus::asio::dbus_interface> crit_intf;
    std::shared_ptr<sdbusplus::asio::dbus_interface> discrete_offset_intf;
    std::shared_ptr<sdbusplus::asio::dbus_interface> available_intf;

    char name[32];
    uint8_t reading_state;
    uint8_t is_discrete_sensor;

    bool have_warn_low;
    bool alarm_warn_low;
    double warn_low;
    int32_t have_warn_low_retry;
    int32_t warn_low_retry_count;

    bool have_warn_high;
    bool alarm_warn_high;
    double warn_high;
    int32_t have_warn_high_retry;
    int32_t warn_high_retry_count;

    bool have_crit_low;
    bool alarm_crit_low;
    double crit_low;
    int32_t have_crit_low_retry;
    int32_t crit_low_retry_count;

    bool have_crit_high;
    bool alarm_crit_high;
    double crit_high;
    int32_t have_crit_high_retry;
    int32_t crit_high_retry_count;

    double max_value;
    double min_value;

    double scale;

    bool have_assert_discrete_offset[16];
    bool have_deassert_discrete_offset[16];
    bool alarm_discrete_offset[16];
    int32_t have_assert_discrete_offset_retry[16];
    int32_t assert_discrete_offset_retry_count[16];

    struct timespec last_scan_ts;

    long int scan_period;     //ms
    long int power_on_delay;  //ms
    long int power_off_delay; //ms

    char obj_path[128];
    char unit[128];
    int32_t (*read_func)(double*);
    double value;
    struct sdr_rec_map* sdr_mapping;
};

struct VariantToStrVisitor
{
    template <typename T>
    std::string operator()(const T& t) const
    {
        return static_cast<std::string>(t);
    }
};

int add_ipmi_std_sel_entry(std::string ipmiSELAddMessage,
                           std::string sensorPath,
                           std::vector<uint8_t> eventData,
                           bool assert,
                           uint16_t generatorID);

std::shared_ptr<sdbusplus::asio::connection> get_bus_connection(void);
void restart_io(void);

std::string get_processing_sensor_path(void);
