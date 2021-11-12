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

#include <variant>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <semaphore.h>
#include <type_traits>
#include <mutex>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <bsd/sys/time.h>
#include <unistd.h>
#include <execinfo.h>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/server.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "debug.hpp"
#include "phoenix-dbus-sensors.hpp"
#include "raw_reading.hpp"

sem_t semService;

#define MAX_SDR_MAP_ENTRIES 512
#define MAX_SDR_RECORD_SIZE (int)(sizeof(struct sdr_rec_header) + 255)

#define MAX_SENSOR_COUNT MAX_SDR_MAP_ENTRIES

struct sensor_var_t sensor_table[MAX_SENSOR_COUNT];

static int32_t sensor_count = sizeof(sensor_callback_map) / sizeof (struct callback_map_t);

static struct sdr_rec_map *sdr_map_entry;

// All SDR raw bytes point
static uint8_t *sdr_raw_cache_ptr;
constexpr static const uint32_t sdr_raw_cache_ptr_size = MAX_SDR_RECORD_SIZE *
        MAX_SDR_MAP_ENTRIES;
static int sdr_total_record_count = 0;

#define ACTIVE_SDR_FILE "/usr/share/phoenix-sensor-config/SDR.active"

constexpr const int default_scan_reading_state = READING_STATE::BOTH;
constexpr const int defaultThresholdRetry = 3;
constexpr const int defaultDiscreteRetry = 0;
constexpr const int defaultScanPeriod = 3000; //ms
constexpr const int defaultPowerOnDelay = 0; //ms
constexpr const int defaultPowerOffDelay = 0; //ms

extern bool is_power_on(void);

static bool last_power_good;
static struct timespec last_poweron_ts;
static struct timespec last_poweroff_ts;

struct timespec get_last_poweron_ts(void)
{
    return last_poweron_ts;
}

struct timespec get_last_poweroff_ts(void)
{
    return last_poweroff_ts;
}

struct VariantToBoolVisitor {
    template <typename T> bool operator()(const T& t) const
    {
        return static_cast<bool>(t);
    }
};

struct VariantToDoubleVisitor {
    template <typename T> double operator()(const T& t) const
    {
        return static_cast<double>(t);
    }
};

int add_ipmi_std_sel_entry(std::string ipmiSELAddMessage,
                           std::string sensorPath,
                           std::vector<uint8_t> eventData,
                           bool assert,
                           uint16_t generatorID)
{
    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);

    auto method = conn->new_method_call(
                      "xyz.openbmc_project.Logging.IPMI",
                      "/xyz/openbmc_project/Logging/IPMI",
                      "xyz.openbmc_project.Logging.IPMI",
                      "IpmiSelAdd");

    method.append(ipmiSELAddMessage, 
                    sensorPath, 
                    eventData,
                    assert,
                    generatorID);

    try {
        conn->call(method);
    }

    catch (sdbusplus::exception::SdBusError& e) {
        DPRINT ( "add_ipmi_std_sel_entry error\n");
        return -1;
    }
    
    return 0;
}

/**
 * @brief   Display backtrace dump
 *
 * @param   signal_num      Signal
 */
__inline__ void segv_backtrace(int signal_num) 
{ 
    void *array[256];
    size_t size, i;
    char **strings;

    DPRINT("Catch %s signal!! (%d)\n", strsignal(signal_num), signal_num);  
    DPRINT("\nBacktrace:\n");
    
    size = backtrace(array, 256);

    strings = backtrace_symbols(array, size);

    for (i = 0; i < size; i++) {
        DPRINT("%d: %s\n", i, strings[i]);
    }
    
    free(strings);

    exit(1);
}

/**
 * @brief   Retrieve the system time of the specified clock
 *
 * @param   ts      Timespec structures point
 *
 * @return  ret     Status code
 */
int get_system_timespec(struct timespec *ts)
{
    int ret;

    FUNC_START();

    ret = clock_gettime (CLOCK_MONOTONIC, ts);
    if (ret) {
        DPRINT ("clock_gettime error %d\n", ret);
    }

    FUNC_END();
    
    return ret;
}

void update_last_power_change_ts(void)
{
    bool power_good = is_power_on();

    if (last_power_good == power_good) {
        return;
    }

    last_power_good = power_good;

    if (power_good == true) {
        get_system_timespec(&last_poweron_ts);
    } else {
        get_system_timespec(&last_poweroff_ts);
    }
}

/**
 * @brief   Trigger assert/deassert threshold sensor event
 *
 * @param   sensor       Sensor point
 * @param   assert_value Reading value at the time of the trigger event
 * @param   level        Threshold warning/critical
 * @param   direction    Threshold high/low
 * @param   assert       Assert or deassert, true is assert
 */
void assert_thresholds(struct sensor_var_t *sensor, double assert_value,
                       LEVEL level, DIRECTION direction,
                       bool assert)
{
    std::string property;
    std::shared_ptr<sdbusplus::asio::dbus_interface> interface;

    FUNC_START();

    if (sensor == NULL) {
        DPRINT ("ERROR: sensor point is null\n");
        FUNC_END();
        return;
    }

    if (level == LEVEL::WARNING &&
        direction == DIRECTION::HIGH) {
        property = "WarningAlarmHigh";
        interface = sensor->warn_intf;
    } else if (level == LEVEL::WARNING &&
               direction == DIRECTION::LOW) {
        property = "WarningAlarmLow";
        interface = sensor->warn_intf;
    } else if (level == LEVEL::CRITICAL &&
               direction == DIRECTION::HIGH) {
        property = "CriticalAlarmHigh";
        interface = sensor->crit_intf;
    } else if (level == LEVEL::CRITICAL &&
               direction == DIRECTION::LOW) {
        property = "CriticalAlarmLow";
        interface = sensor->crit_intf;
    } else {
        DPRINT ("ERROR: Unknown threshold level or direction\n");
        FUNC_END();
        return;
    }
    if (!interface) {
        DPRINT ("ERROR: uninitialized interface\n");
        FUNC_END();
        return;
    }

    DPRINT("assert_thresholds signal\n");

    try {
        sdbusplus::message::message msg =
            interface->new_signal("ThresholdAsserted");

        msg.append(sensor->name, interface->get_interface_name(), property,
                   assert, assert_value);
        msg.signal_send();
    } catch (const sdbusplus::exception::exception& e) {
        DPRINT("Failed to send thresholdAsserted signal with assert_value\n");
    }

    /*
        try {
            interface->set_property(property, assert);
        } catch (const sdbusplus::exception::exception& e) {
            DPRINT("Failed to assertThreshold\n");
        }
    */

    FUNC_END();
}

/**
 * @brief   Trigger assert/deassert discrete sensor event
 *
 * @param   sensor       Sensor point
 * @param   offset       Event offset
 * @param   assert       Assert or deassert, true is assert
 */
void assert_discrete_offsets(struct sensor_var_t *sensor, unsigned int offset,
                             bool assert)
{
    std::string property;
    std::shared_ptr<sdbusplus::asio::dbus_interface> interface;

    FUNC_START();

    if (sensor == NULL) {
        DPRINT ("ERROR: sensor point is null\n");
        FUNC_END();
        return;
    }

    if (assert == true) {
        property = "AssertionAlarmOffset";
    } else {
        property = "DeassertionAlarmOffset";
    }

    interface = sensor->discrete_offset_intf;

    if (!interface) {
        DPRINT ("ERROR: uninitialized interface\n");
        FUNC_END();
        return;
    }

    DPRINT("assert_discrete_offsets %s %d\n", property.c_str(), offset);
    try {
        interface->set_property(property, offset);
        interface->set_property(property, (unsigned int) 255);
    } catch (const sdbusplus::exception::exception& e) {
        DPRINT("Failed to assert_discrete_offsets\n");
    }

    FUNC_END();
}

/**
 * @brief   Check for triggering discrete sensor events
 *
 * @param   sensor       Sensor point
 * @param   reading      Sensor reading value that used to check if need to generate event 
 */
void check_discrete_offsets(struct sensor_var_t *sensor, double reading)
{
    int offset;
    int64_t discrete;

    FUNC_START();

    discrete = static_cast<int64_t>(reading);

    for (offset = DISCRETE_OFFSET::OFFSET_0; offset <= DISCRETE_OFFSET::OFFSET_15; offset++) {
        bool alarm = ((discrete & BIT(offset)) > 0) ? true:false;

        if (sensor->alarm_discrete_offset[offset] == alarm) {
            continue;
        }

        sensor->alarm_discrete_offset[offset] = alarm;

        if (sensor->have_assert_discrete_offset[offset] == true) {
            if (alarm == true) {
                assert_discrete_offsets(sensor, offset, alarm);
            }
        }

        if (sensor->have_deassert_discrete_offset[offset] == true) {
            if (alarm == false) {
                assert_discrete_offsets(sensor, offset, alarm);
            }
        }
    }

    FUNC_END();
}

/**
 * @brief   Check if discrete sensor offsets need to be re-try
 *
 * @param   sensor       Sensor point
 * @param   reading      Sensor reading value that used to check if need to re-try
 *
 * @return  status       True is needed to re-try
 *                       False
 */
bool check_discrete_offset_retry(struct sensor_var_t *sensor, double reading)
{
    bool status = false;
    int offset;
    int64_t discrete;

    FUNC_START();

    discrete = static_cast<int64_t>(reading);

    for (offset = DISCRETE_OFFSET::OFFSET_0; offset <= DISCRETE_OFFSET::OFFSET_15; offset++) {
        bool alarm = ((discrete & BIT(offset)) > 0) ? true:false;

        if (sensor->alarm_discrete_offset[offset] == alarm) {
            continue;
        }

        if (sensor->have_assert_discrete_offset[offset] == true) {
            if (alarm == true) {
                if (sensor->assert_discrete_offset_retry_count[offset]++ < sensor->have_assert_discrete_offset_retry[offset]) {
                    DPRINT("%s offset %d assert retry count = %d\n", sensor->name, offset, sensor->assert_discrete_offset_retry_count[offset]);
                    status = true;
                } else {
                    sensor->assert_discrete_offset_retry_count[offset] = 0;
                }
            }
        }

        if (sensor->have_deassert_discrete_offset[offset] == true) {
            if (alarm == false) {
                sensor->assert_discrete_offset_retry_count[offset] = 0;
            }
        }
    }

    FUNC_END();

    return status;
}

/**
 * @brief   Check for triggering threshold sensor events
 *
 * @param   sensor       Sensor point
 * @param   reading      Sensor reading value that used to check if need to generate event 
 */
void check_thresholds(struct sensor_var_t *sensor, double reading)
{
    bool alarm;

    FUNC_START();

    //TODO: SDR Hysteresis value

    if (sensor->have_crit_low) {
        if (reading < sensor->crit_low) {
            alarm = true;
        } else {
            alarm = false;
        }

        if (sensor->alarm_crit_low != alarm) {
            DPRINT("alarm_crit_low %d\n", alarm );
            sensor->alarm_crit_low = alarm;

            assert_thresholds(sensor, reading, LEVEL::CRITICAL, DIRECTION::LOW, alarm);
        }
    }

    if (sensor->have_crit_high) {
        if (reading >= sensor->crit_high) {
            alarm = true;
        } else {
            alarm = false;
        }
        if (sensor->alarm_crit_high != alarm) {
            DPRINT("alarm_crit_high %d\n", alarm );
            sensor->alarm_crit_high = alarm;

            assert_thresholds(sensor, reading, LEVEL::CRITICAL, DIRECTION::HIGH, alarm);
        }
    }

    if (sensor->have_warn_low) {
        if (reading < sensor->warn_low) {
            alarm = true;
        } else {
            alarm = false;
        }
        if (sensor->alarm_warn_low != alarm) {
            DPRINT("alarm_warn_low %d\n", alarm );
            sensor->alarm_warn_low = alarm;

            assert_thresholds(sensor, reading, LEVEL::WARNING, DIRECTION::LOW, alarm);
        }
    }

    if (sensor->have_warn_high) {
        if (reading >= sensor->warn_high) {
            alarm = true;
        } else {
            alarm = false;
        }
        if (sensor->alarm_warn_high != alarm) {
            DPRINT("alarm_warn_high %d\n", alarm );
            sensor->alarm_warn_high = alarm;

            assert_thresholds(sensor, reading, LEVEL::WARNING, DIRECTION::HIGH, alarm);
        }
    }

    FUNC_END();
}

/**
 * @brief   Check if threshold sensor event need to be re-try
 *
 * @param   sensor       Sensor point
 * @param   reading      Sensor reading value that used to check if need to re-try
 *
 * @return  status       True is needed to re-try
 *                       False
 */
bool check_thresholds_retry(struct sensor_var_t *sensor, double reading)
{
    bool alarm;
    bool status = false;

    FUNC_START();

    if (sensor->have_warn_low) {
        if (reading < sensor->warn_low) {
            alarm = true;
        } else {
            alarm = false;
        }
        if (sensor->alarm_warn_low != alarm) {
            if (alarm == true &&
                sensor->warn_low_retry_count++ < sensor->have_warn_low_retry) {
                DPRINT("%s = %lf, warn low retry count = %d\n", sensor->name, reading, sensor->warn_low_retry_count);
                status = true;
            } else {
                sensor->warn_low_retry_count = 0;
            }
        }
    }

    if (sensor->have_warn_high) {
        if (reading >= sensor->warn_high) {
            alarm = true;
        } else {
            alarm = false;
        }
        if (sensor->alarm_warn_high != alarm) {
            if (alarm == true &&
                sensor->warn_high_retry_count++ < sensor->have_warn_high_retry) {
                DPRINT("%s = %lf, warn high retry count = %d\n", sensor->name, reading, sensor->warn_high_retry_count);
                status = true;
            } else {
                sensor->warn_high_retry_count = 0;
            }
        }
    }

    if (sensor->have_crit_low) {
        if (reading < sensor->crit_low) {
            alarm = true;
        } else {
            alarm = false;
        }

        if (sensor->alarm_crit_low != alarm) {
            if (alarm == true &&
                sensor->crit_low_retry_count++ < sensor->have_crit_low_retry) {
                DPRINT("%s = %lf, crit low retry count = %d\n", sensor->name, reading, sensor->crit_low_retry_count);
                status = true;
            } else {
                sensor->crit_low_retry_count = 0;
            }
        }
    }

    if (sensor->have_crit_high) {
        if (reading >= sensor->crit_high) {
            alarm = true;
        } else {
            alarm = false;
        }
        if (sensor->alarm_crit_high != alarm) {
            if (alarm == true &&
                sensor->crit_high_retry_count++ < sensor->have_crit_high_retry) {
                DPRINT("%s = %lf, crit high retry count = %d\n", sensor->name, reading, sensor->crit_high_retry_count);
                status = true;
            } else {
                sensor->crit_high_retry_count = 0;
            }
        }
    }

    FUNC_END();

    return status;
}

/**
 * @brief   Calculate the difference tick between two time specs
 *
 * @param   end_ts       Subtracts end_ts from start_ts
 * @param   start_ts     Subtracts end_ts from start_ts
 *
 * @return  tick         milliseconds
 */
unsigned long int diff_timespec_ms(struct timespec end_ts, struct timespec start_ts)
{
    struct timespec diff_ts;
    unsigned long int tick;

    // already handle overflow in timespecsub
    timespecsub(&end_ts, &start_ts, &diff_ts);

    tick = diff_ts.tv_sec * 1000;
    tick += diff_ts.tv_nsec / 1000000;

    return tick;
}

/**
 * @brief   Check if the sensor is in power on delay period
 *
 * @param   sensor       Sensor point
 *
 * @return  true/false
 */
bool is_sensor_power_on_delay(struct sensor_var_t *sensor)
{
    FUNC_START();

    if (sensor->power_on_delay == 0) {
        FUNC_END();
        return false;
    }

    struct timespec current_ts;

    get_system_timespec(&current_ts);

    if (diff_timespec_ms(current_ts, get_last_poweron_ts()) < sensor->power_on_delay) {
        DPRINT("%s is under power on delay\n", sensor->name);
        FUNC_END();
        return true;
    }

    FUNC_END();

    return false;
}

/**
 * @brief   Check if the sensor is in power off delay period
 *
 * @param   sensor       Sensor point
 *
 * @return  true/false
 */
bool is_sensor_power_off_delay(struct sensor_var_t *sensor)
{
    FUNC_START();

    if (sensor->power_off_delay == 0) {
        FUNC_END();
        return false;
    }

    struct timespec current_ts;

    get_system_timespec(&current_ts);

    if (diff_timespec_ms(current_ts, get_last_poweroff_ts()) < sensor->power_off_delay) {
        DPRINT("%s is under power off delay\n", sensor->name);
        FUNC_END();
        return true;
    }

    return false;
}

/**
 * @brief   Check if the sensor is allowed to scan
 *
 * @param   sensor       Sensor point
 *
 * @return  true/false
 */
bool is_sensor_scan_enable(struct sensor_var_t *sensor)
{
    bool functional = true;

    FUNC_START();

    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);
    auto method = conn->new_method_call(
                      phoenix_sensor_service,
                      sensor->obj_path,
                      property_interface_name,
                      "GetAll");
    method.append(operational_status_interface_name);
    boost::container::flat_map<std::string, std::variant<bool>> operational_status_variant;
    try {
        sdbusplus::message::message get_functional_resp = conn->call(method);
        get_functional_resp.read(operational_status_variant);
    } catch (sdbusplus::exception::SdBusError& e) {
        DPRINT ("Get operational status error, %s scan disabled\n", sensor->name);
        FUNC_END();
        return false;
    }

    auto findVal = operational_status_variant.find("Functional");
    if (findVal != operational_status_variant.end()) {
        functional = std::visit(VariantToBoolVisitor(), findVal->second);
    }

    FUNC_END();

    return functional;
}

/**
 * @brief   Get sensor value from dbus
 *
 * @param   sensor       Sensor point
 * @param   reading      Sensor value point
 *
 * @return  status code
 */
int get_dbus_sensor_value(struct sensor_var_t *sensor, double *reading)
{  
    FUNC_START();

    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);
    auto method = conn->new_method_call(
                      phoenix_sensor_service,
                      sensor->obj_path,
                      property_interface_name,
                      "GetAll");
    method.append(sensor_value_interface_name);

    boost::container::flat_map<std::string, std::variant<double>> sensor_value_variant;
    try {
        sdbusplus::message::message get_value_resp = conn->call(method);
        get_value_resp.read(sensor_value_variant);
    } catch (sdbusplus::exception::SdBusError& e) {
        DPRINT ("Get operational status error, %s scan disabled\n", sensor->name);
        FUNC_END();
        return STATUS_ERROR;
    }

    auto findVal = sensor_value_variant.find("Value");
    if (findVal != sensor_value_variant.end()) {
        *reading = std::visit(VariantToDoubleVisitor(), findVal->second);
    }

    FUNC_END();
    return STATUS_OK;
}

/**
 * @brief   Handle sensor reading functions
 *
 * @param   sensor       Sensor point
 *
 * @return  status
 */
int sensor_value_callback(struct sensor_var_t *sensor)
{
    int32_t (*get_reading) (double*);
    double reading_cache = 0;
    int32_t status;
    bool powergood;

    FUNC_START();

    if (sensor == NULL) {
        FUNC_END();
        return STATUS_POINT_NULL;
    }

    if (sensor->read_func == NULL) {
        FUNC_END();
        return STATUS_POINT_NULL;
    }

    get_reading = sensor->read_func;

    powergood = is_power_on();

    if ((powergood == false && sensor->reading_state == READING_STATE::POWERON) ||
        (powergood == true && sensor->reading_state == READING_STATE::POWEROFF)) {
        status = SENSOR_STATUS::UNAVAILABLE;
    } else if ((powergood == true && is_sensor_power_on_delay(sensor) == true) ||
               (powergood == false && is_sensor_power_off_delay(sensor) == true)) {
        status = SENSOR_STATUS::UNAVAILABLE;
    } else {
        if (DEBUG_FUNC_START_END == true) {
            DPRINT("%s get_reading Start\n", sensor->name);
        }
        // Get last sensor value, for some sensor that is possible to update value from outside
        get_dbus_sensor_value(sensor, &reading_cache);
        if (reading_cache != sensor->value) {
            DPRINT("%s get manual sensor value\n", sensor->name);
            //TODO: In some specific modes, we allow override sensor value 
        }
        
        // Get Sensor reading_cache
        status = get_reading (&reading_cache);

        if (DEBUG_FUNC_START_END == true) {
            DPRINT("%s get_reading End\n", sensor->name);
        }
    }

    if ((status == SENSOR_STATUS::ABSENT) ||
        (status == SENSOR_STATUS::UNAVAILABLE) ||
        (status == SENSOR_STATUS::ERROR)) {
        //DPRINT ( "%s value is unavailable\n", sensor->name);
        sensor->available_intf->set_property("Available", false);

        FUNC_END();
        return STATUS_OK;
    }

    // Check retry and avoid sensor value update
    if (sensor->is_discrete_sensor == true) {
        if (check_discrete_offset_retry(sensor, reading_cache) == true) {
            FUNC_END();
            return STATUS_OK;
        }
    } else {
        if (check_thresholds_retry(sensor, reading_cache) == true) {
            FUNC_END();
            return STATUS_OK;
        }
    }

    // set availability
    sensor->available_intf->set_property("Available", true);

    // set value
    sensor->value_intf->set_property("Value", reading_cache);
    sensor->value = reading_cache;

    if (status == SENSOR_STATUS::NORMAL_AND_EVENT_HANDLED) {
        if (DEBUG_ALL_SENSOR_VALUE == true) {
            DPRINT ( "%s doesn't need to handle events\n", sensor->name);
        }
        FUNC_END();
        return STATUS_OK;
    }

    if (sensor->is_discrete_sensor == true) {
        if (DEBUG_ALL_SENSOR_VALUE == true) {
            int64_t discrete = static_cast<int64_t>(reading_cache);
            DPRINT ( "%s reading_cache 0x%llx\n", sensor->name, discrete);
        }
        check_discrete_offsets(sensor, reading_cache);

    } else {
        if (DEBUG_ALL_SENSOR_VALUE == true) {
            DPRINT ( "%s reading_cache %lf\n", sensor->name, reading_cache);
        }
        check_thresholds(sensor, reading_cache);
    }

    FUNC_END();
    return STATUS_OK;
}

/**
 * @brief   Handle sensor scan
 *
 * @return  status
 */
int sensor_scan_service(void)
{
    sem_wait(&semService);

    FUNC_START();

    struct timespec sensor_scan_ts;

    while (true) {
        get_system_timespec(&sensor_scan_ts);

        for (int i = 0; i < sensor_count; i++) {
            struct sensor_var_t *sensor = &sensor_table[i];

            if (sensor == NULL) {
                continue;
            }

            // No read function
            if (sensor->read_func == NULL) {
                continue;
            }
            
            // Check scan period
            if (diff_timespec_ms(sensor_scan_ts, sensor->last_scan_ts) < sensor->scan_period) {
                continue;
            }
            
            // Scan disable
            if (is_sensor_scan_enable(sensor) == false) {
                //DPRINT ("%s scan disable\n", sensor->name);
                continue;
            }

            // Update Sensor Reading Value
            sensor_value_callback(sensor);

            // Update Sensor last scan time spec for scan period
            get_system_timespec(&sensor->last_scan_ts);

            // Update last Power On/Off time spec for power on/off scan delay
            update_last_power_change_ts();
        
            usleep(30000);
        }
        sleep(1);
    }

    FUNC_END();
}

/**
 * @brief   Get sensor unit path
 *
 * @param   percentage       Sensor unit is percentage or not, true is percentage
 * @param   sensor_type      Sensor Type code from IPMI SDR
 * @param   unit             Sensor Unit code from IPMI SDR
 *
 * @return  sensor unit path
 */
const char *get_sensor_unit_path(bool percentage, uint8_t sensor_type,  uint8_t unit)
{
    const char *path;

    if (unit == UNIT_TYPE_CODE::DEGREES_C ||
        unit == UNIT_TYPE_CODE::DEFREES_F ||
        unit == UNIT_TYPE_CODE::DEFREES_K) {
        path = sensor_unit_degreesc;
    } else if (unit == UNIT_TYPE_CODE::VOLTS) {
        path = sensor_unit_volts;
    } else if (unit == UNIT_TYPE_CODE::AMPS) {
        path = sensor_unit_ampers;
    } else if (unit == UNIT_TYPE_CODE::WATTS) {
        path = sensor_unit_watts;
    } else if (unit == UNIT_TYPE_CODE::RPM) {
        path = sensor_unit_rpms;
    } else if ((sensor_type == 0x04 && unit == UNIT_TYPE_CODE::UNSPECIFIED) || 
                percentage == true) {
        path = sensor_unit_percents;
    } else {
        path = sensor_unit_none;
    }

    return path;
}

/**
 * @brief   Get sensor object path
 *
 * @param   unit       Sensor unit path
 *
 * @return  sensor object path
 */
const char *get_sensor_obj_path(char *unit)
{
    const char *path;

    if (!strcmp(sensor_unit_degreesc, unit)) {
        path = temperature_sensor_path_prefix;
    } else if (!strcmp(sensor_unit_rpms, unit)) {
        path = fan_tach_sensor_path_prefix;
    } else if (!strcmp(sensor_unit_percents, unit)) {
        path = pwm_sensor_path_prefix;
    } else if (!strcmp(sensor_unit_volts, unit)) {
        path = voltage_sensor_path_prefix;
    } else if (!strcmp(sensor_unit_ampers, unit)) {
        path = current_sensor_path_prefix;
    } else if (!strcmp(sensor_unit_watts, unit)) {
        path = power_sensor_path_prefix;
    } else {
        path = specific_sensor_path_prefix;
    }

    return path;
}

/**
 * @brief   Convert SDR raw value to real world value
 *
 * @param   sensor       Sensor point
 * @param   val          Value to be converted
 *
 * @return  real world value
 */
double sdr_convert_value(struct sdr_record_full_sensor *sensor, uint8_t val)
{
    int m, b, k1, k2;
    double result;

    m = __TO_M(sensor->mtol);
    b = __TO_B(sensor->bacc);
    k1 = __TO_B_EXP(sensor->bacc);
    k2 = __TO_R_EXP(sensor->bacc);

    //DPRINT ("val=%d  m=%d  b=%d  k1=%d  k2=%d\n",val,m,b,k1,k2);

    switch (sensor->unit.analog) {
    case 0:
        result = (double) (((m * val) +
                            (b * std::pow(10, k1))) * std::pow(10, k2));
        break;
    case 1:
        if (val & 0x80)
            val++;
    /* fall through */
    case 2:
        result = (double) (((m * (int8_t) val) +
                            (b * std::pow(10, k1))) * std::pow(10, k2));
        break;
    default:
        /* Oops! This isn't an analog sensor. */
        return 0.0;
    }

#if 1
    switch (sensor->linearization & 0x7f) {
    case SDR_SENSOR_L_LN:
        result = log(result);
        break;
    case SDR_SENSOR_L_LOG10:
        result = log10(result);
        break;
    case SDR_SENSOR_L_LOG2:
        result = (double) (log(result) / log(2.0));
        break;
    case SDR_SENSOR_L_E:
        result = exp(result);
        break;
    case SDR_SENSOR_L_EXP10:
        result = pow(10.0, result);
        break;
    case SDR_SENSOR_L_EXP2:
        result = pow(2.0, result);
        break;
    case SDR_SENSOR_L_1_X:
        result = pow(result, -1.0);	/*1/x w/o exception */
        break;
    case SDR_SENSOR_L_SQR:
        result = pow(result, 2.0);
        break;
    case SDR_SENSOR_L_CUBE:
        result = pow(result, 3.0);
        break;
    case SDR_SENSOR_L_SQRT:
        result = sqrt(result);
        break;
    case SDR_SENSOR_L_CUBERT:
        result = cbrt(result);
        break;
    case SDR_SENSOR_L_LINEAR:
    default:
        break;
    }
#endif
    //DPRINT ("result = %f\n",result);

    return result;
}

/**
 * @brief   Find board path
 *
 * @param   conn        Dbus connection
 * @param   board_path  Board path string
 * @param   retry       Retry
 *
 * @return  status code
 */
int find_board_path(std::shared_ptr<sdbusplus::asio::connection> conn,
                    std::string *board_path,
                    int retry)
{
    auto method = conn->new_method_call("xyz.openbmc_project.ObjectMapper",
                                        "/xyz/openbmc_project/object_mapper",
                                        "xyz.openbmc_project.ObjectMapper", "GetSubTree");

    FUNC_START();

    method.append("/", 0, std::array<const char*, 1> {item_board_intferface_name});

    std::vector<std::pair<std::string,
        std::vector<std::pair<std::string, std::vector<std::string>>>>> resp;

    do {
        try {
            auto reply = conn->call(method);
            reply.read(resp);
        } catch (sdbusplus::exception_t&) {
            DPRINT ("can't find board object\n");
        };

        // FIXME: Although this is started after entity-manager, but still have to wait for board object to be created.
        if (resp.size() == 0 && retry > 0) {
            DPRINT ("No board object, retry %d\n", retry);
            sleep(1);
        }
    } while (resp.size() == 0 && retry-- > 0);

    if (resp.size() == 0) {
        DPRINT ("board object count is 0\n");
        FUNC_END();
        return -1;
    }

    for (const auto& object : resp) {
        std::string path_name = object.first;
        DPRINT ("board path_name %s\n", path_name.c_str());
        *board_path = path_name;
        /*
        for (const auto& serviceIface : object.second) {
            std::string serviceName = serviceIface.first;
            DPRINT ("board serviceName %s\n", serviceName.c_str());
            break;
        }
        */
        break;
    }

    FUNC_END();

    return STATUS_OK;
}

/**
 * @brief   Display sensor information
 *
 * @param   sensor      Sensor point
 */
static void display_sensor_info(struct sensor_var_t *sensor)
{
    FUNC_START();

    DPRINT ("name = %s\n", sensor->name);
    DPRINT ("read_func = %p\n", sensor->read_func);

    DPRINT ("unit = %s\n", sensor->unit);
    DPRINT ("obj_path = %s\n", sensor->obj_path);

    DPRINT ("is_discrete_sensor = %s\n",
            (sensor->is_discrete_sensor == true) ? "true":"false");

    DPRINT ("scan_period = %ld\n", sensor->scan_period);
    DPRINT ("power_on_delay = %ld\n", sensor->power_on_delay);
    DPRINT ("power_off_delay = %ld\n", sensor->power_off_delay);

    if (sensor->is_discrete_sensor == false) {
        DPRINT ("scale = %lld\n", sensor->scale);
        DPRINT ("min_value = %lf\n", sensor->min_value);
        DPRINT ("max_value = %lf\n", sensor->max_value);

        DPRINT ("have_warn_high = %s\n",
                (sensor->have_warn_high == true) ? "true":"false");
        DPRINT ("have_warn_low = %s\n",
                (sensor->have_warn_low == true) ? "true":"false");
        DPRINT ("have_crit_high = %s\n",
                (sensor->have_crit_high == true) ? "true":"false");
        DPRINT ("have_crit_low = %s\n",
                (sensor->have_crit_low == true) ? "true":"false");

        DPRINT ("crit_low = %lf\n", sensor->crit_low);
        DPRINT ("crit_high = %lf\n", sensor->crit_high);
        DPRINT ("warn_low = %lf\n", sensor->warn_low);
        DPRINT ("warn_high = %lf\n", sensor->warn_high);

        DPRINT ("have_crit_low_retry = %d\n", sensor->have_crit_low_retry);
        DPRINT ("have_crit_high_retry = %d\n", sensor->have_crit_high_retry);
        DPRINT ("have_warn_low_retry = %d\n", sensor->have_warn_low_retry);
        DPRINT ("have_warn_high_retry = %d\n", sensor->have_warn_high_retry);
        
    } else {
        for (int i = 0; i < 16; i++) {
            if (sensor->have_assert_discrete_offset[i] == true) {
                DPRINT( "have_assert_discrete_offset[%d] = %s\n", i,
                        (sensor->have_assert_discrete_offset[i] == true) ? "true":"false");
            }
            if (sensor->have_deassert_discrete_offset[i] == true) {
                DPRINT( "have_deassert_discrete_offset[%d] = %s\n", i,
                        (sensor->have_deassert_discrete_offset[i] == true) ? "true":"false");
            }
        }
    }
    DPRINT ("\n");

    FUNC_END();
}

/**
 * @brief   Initial all sensor information
 *
 * @return  status code
 */
int initial_sensor_capability_info(void)
{
    FUNC_START();

    printf ("total sensors %d\n", sensor_count);

    //TODO: mutex to protect sensor_table

    //init sensor_table
    memset(sensor_table, 0, sizeof(struct sensor_var_t)*MAX_SENSOR_COUNT);

    for (int index = 0; index < sensor_count; index++) {
        bool is_discrete_sensor = false;

        std::string sensor_name_nospace;
        sensor_name_nospace.assign(sensor_callback_map[index].name, strlen(sensor_callback_map[index].name));
        sensor_name_nospace = boost::replace_all_copy(sensor_name_nospace, " ", "_");

        strcat (sensor_table[index].name, sensor_name_nospace.c_str());

        sensor_table[index].read_func = sensor_callback_map[index].read_func;

        sensor_table[index].reading_state = sensor_callback_map[index].reading_state;
        if (sensor_table[index].reading_state == READING_STATE::NOT_DEFINE) {
            sensor_table[index].reading_state = default_scan_reading_state; 
        }

        sensor_table[index].scan_period = sensor_callback_map[index].scan_period;
        if (sensor_table[index].scan_period < 0) {
            sensor_table[index].scan_period = defaultScanPeriod;
        }

        sensor_table[index].power_on_delay = sensor_callback_map[index].power_on_delay;
        if (sensor_table[index].power_on_delay < 0) {
            sensor_table[index].power_on_delay = defaultPowerOnDelay;
        }

        sensor_table[index].power_off_delay = sensor_callback_map[index].power_off_delay;
        if (sensor_table[index].power_off_delay < 0) {
            sensor_table[index].power_off_delay = defaultPowerOffDelay;
        }

        sensor_table[index].have_crit_high_retry = sensor_callback_map[index].crit_high_retry;
        if (sensor_table[index].have_crit_high_retry < 0) {
            sensor_table[index].have_crit_high_retry = defaultThresholdRetry;
        }

        sensor_table[index].have_crit_low_retry = sensor_callback_map[index].crit_low_retry;
        if (sensor_table[index].have_crit_low_retry < 0) {
            sensor_table[index].have_crit_low_retry = defaultThresholdRetry;
        }

        sensor_table[index].have_warn_high_retry = sensor_callback_map[index].warn_high_retry;
        if (sensor_table[index].have_warn_high_retry < 0) {
            sensor_table[index].have_warn_high_retry = defaultThresholdRetry;
        }

        sensor_table[index].have_warn_low_retry = sensor_callback_map[index].warn_low_retry;
        if (sensor_table[index].have_warn_low_retry < 0) {
            sensor_table[index].have_warn_low_retry = defaultThresholdRetry;
        }

        sensor_table[index].have_assert_discrete_offset_retry[0] = sensor_callback_map[index].offset0_retry;
        sensor_table[index].have_assert_discrete_offset_retry[1] = sensor_callback_map[index].offset1_retry;
        sensor_table[index].have_assert_discrete_offset_retry[2] = sensor_callback_map[index].offset2_retry;
        sensor_table[index].have_assert_discrete_offset_retry[3] = sensor_callback_map[index].offset3_retry;
        sensor_table[index].have_assert_discrete_offset_retry[4] = sensor_callback_map[index].offset4_retry;
        sensor_table[index].have_assert_discrete_offset_retry[5] = sensor_callback_map[index].offset5_retry;
        sensor_table[index].have_assert_discrete_offset_retry[6] = sensor_callback_map[index].offset6_retry;
        sensor_table[index].have_assert_discrete_offset_retry[7] = sensor_callback_map[index].offset7_retry;
        sensor_table[index].have_assert_discrete_offset_retry[8] = sensor_callback_map[index].offset8_retry;
        sensor_table[index].have_assert_discrete_offset_retry[9] = sensor_callback_map[index].offset9_retry;
        sensor_table[index].have_assert_discrete_offset_retry[10] = sensor_callback_map[index].offset10_retry;
        sensor_table[index].have_assert_discrete_offset_retry[11] = sensor_callback_map[index].offset11_retry;
        sensor_table[index].have_assert_discrete_offset_retry[12] = sensor_callback_map[index].offset12_retry;
        sensor_table[index].have_assert_discrete_offset_retry[13] = sensor_callback_map[index].offset13_retry;
        sensor_table[index].have_assert_discrete_offset_retry[14] = sensor_callback_map[index].offset14_retry;
        sensor_table[index].have_assert_discrete_offset_retry[15] = sensor_callback_map[index].offset15_retry;

        for (int offset = 0; offset < 16; offset++) {
            if (sensor_table[index].have_assert_discrete_offset_retry[offset] < 0) {
                sensor_table[index].have_assert_discrete_offset_retry[offset] = defaultDiscreteRetry;
            }
        }

        // Find SDR Entry
        bool findSDR = false;
        for (int i = 0; i < sdr_total_record_count; i++) {
            if (strlen(sensor_callback_map[index].name) != strlen(sdr_map_entry[i].sensor_name))
                continue;

            if (strcmp(sensor_callback_map[index].name, sdr_map_entry[i].sensor_name) == 0) {
                sensor_table[index].sdr_mapping = &sdr_map_entry[i];
                //DPRINT ("Find SDR %d, %p \n", i, sensor_table[index].sdrMapping);
                findSDR = true;
                break;
            }
        }

        if (findSDR == false) {
            DPRINT ("Can't Find SDR for %s\n", sensor_callback_map[index].name);
            continue;
        }

        uint8_t sdrType = sensor_table[index].sdr_mapping->sdrType;

        if (sdrType == SDR_RECORD_TYPE_FULL_SENSOR) {
            struct sdr_record_full_sensor sdrData;
            memcpy(&sdrData,
                   sdr_raw_cache_ptr + sensor_table[index].sdr_mapping->offset,
                   sensor_table[index].sdr_mapping->length);

            // set unit string from sdr
            strcpy(sensor_table[index].unit,
                   get_sensor_unit_path(sdrData.unit.pct,
                                        sdrData.sensor.type,
                                        sdrData.unit.type.base));

            // set object path
            strcpy(sensor_table[index].obj_path,
                   get_sensor_obj_path(sensor_table[index].unit));
            strcat(sensor_table[index].obj_path,
                   sensor_name_nospace.c_str());

            if (sdrData.event_type == 0x01) {
                is_discrete_sensor = false;

                sensor_table[index].have_crit_low = sdrData.mask.type.threshold.assert_lcr_low;
                sensor_table[index].have_crit_high = sdrData.mask.type.threshold.assert_ucr_high;

                sensor_table[index].have_warn_low = sdrData.mask.type.threshold.assert_lnc_low;
                sensor_table[index].have_warn_high = sdrData.mask.type.threshold.assert_unc_high;

                if (sensor_table[index].have_crit_low == true) {
                    sensor_table[index].crit_low = //static_cast<double>
                        (sdr_convert_value(&sdrData, sdrData.threshold.lower.critical));
                }

                if (sensor_table[index].have_crit_high == true) {
                    sensor_table[index].crit_high = //static_cast<double>
                        (sdr_convert_value(&sdrData, sdrData.threshold.upper.critical));
                }

                if (sensor_table[index].have_warn_low == true) {
                    sensor_table[index].warn_low = //static_cast<double>
                        (sdr_convert_value(&sdrData, sdrData.threshold.lower.non_critical));
                }

                if (sensor_table[index].have_warn_high == true) {
                    sensor_table[index].warn_high = //static_cast<double>
                        (sdr_convert_value(&sdrData, sdrData.threshold.upper.non_critical));
                }

                // If need to setup scale, that usually is k2
                // But we already get real world reading value, no need to set scale.
                //sensor_table[index].scale = __TO_R_EXP(sdrData.bacc);

                // If SDR doesn't have min and max, it needs to be set automatically, because other services don't read the formula from SDR.
                // they use min max to calculate ...
                // TODO: PSU related sensors needs to add min and max
                if (sdrData.sensor_min == 0 && sdrData.sensor_max == 0) {
                    if (strcmp(sensor_table[index].unit, sensor_unit_degreesc) == 0) {
                        sensor_table[index].max_value = 127;
                        sensor_table[index].min_value = -128;
                    } else if (strcmp(sensor_table[index].unit, sensor_unit_rpms) == 0) {
                        sensor_table[index].max_value = 25000;
                        sensor_table[index].min_value = 0;
                    } else if (strcmp(sensor_table[index].unit, sensor_unit_percents) == 0) {
                        sensor_table[index].max_value = 100;
                        sensor_table[index].min_value = 0;
                    } else if (strcmp(sensor_table[index].unit, sensor_unit_volts) == 0) {
                        sensor_table[index].max_value = 20;
                        sensor_table[index].min_value = 0;
                    } else if (strcmp(sensor_table[index].unit, sensor_unit_ampers) == 0) {
                        sensor_table[index].max_value = 255;
                        sensor_table[index].min_value = 0;
                    } else if (strcmp(sensor_table[index].unit, sensor_unit_watts) == 0) {
                        sensor_table[index].max_value = 3000;
                        sensor_table[index].min_value = 0;
                    } else {
                        sensor_table[index].max_value = 255;//FIXME
                        sensor_table[index].min_value = 0;//FIXME
                    }
                } else {
                    sensor_table[index].min_value = (sdr_convert_value(&sdrData, sdrData.sensor_min));
                    sensor_table[index].max_value = (sdr_convert_value(&sdrData, sdrData.sensor_max));
                }
            } else {
                is_discrete_sensor = true;

                for (int i = 0; i < 16; i++) {
                    sensor_table[index].have_assert_discrete_offset[i] =
                        ((sdrData.mask.type.discrete.assert_event & (1 << i)) > 0) ? true : false;
                    sensor_table[index].have_deassert_discrete_offset[i] =
                        ((sdrData.mask.type.discrete.deassert_event & (1 << i)) > 0) ? true : false;
                }
            }
        } else if (sdrType == SDR_RECORD_TYPE_COMPACT_SENSOR) {
            struct sdr_record_compact_sensor sdrData;
            memcpy(&sdrData,
                   sdr_raw_cache_ptr + sensor_table[index].sdr_mapping->offset,
                   sensor_table[index].sdr_mapping->length);

            // set unit string from sdr
            strcpy(sensor_table[index].unit,
                   get_sensor_unit_path(sdrData.unit.pct,
                                        sdrData.sensor.type,
                                        sdrData.unit.type.base));

            // set object path
            strcpy (sensor_table[index].obj_path,
                    get_sensor_obj_path(sensor_table[index].unit));
            strcat (sensor_table[index].obj_path,
                    sensor_name_nospace.c_str());

            if (sdrData.event_type == 0x01) {
                is_discrete_sensor = false;

                sensor_table[index].have_crit_low = false;
                sensor_table[index].have_crit_high = false;
                sensor_table[index].have_warn_low = false;
                sensor_table[index].have_warn_high = false;
                // Compact SDR no threshold value can be set in IPMI spec 2.0.
            } else {
                is_discrete_sensor = true;

                for (int i = 0; i < 16; i++) {
                    sensor_table[index].have_assert_discrete_offset[i] =
                        ((sdrData.mask.type.discrete.assert_event & (1 << i)) > 0) ? true : false;
                    sensor_table[index].have_deassert_discrete_offset[i] =
                        ((sdrData.mask.type.discrete.deassert_event & (1 << i)) > 0) ? true : false;
                    sensor_table[index].have_assert_discrete_offset_retry[i] =
                        defaultDiscreteRetry;
                }
            }
        } else {
            is_discrete_sensor = true;
            strcpy (sensor_table[index].unit, sensor_unit_none);

            // set object path
            strcpy (sensor_table[index].obj_path,
                    specific_sensor_path_prefix);
            strcat (sensor_table[index].obj_path,
                    sensor_name_nospace.c_str());

            for (int i = 0; i < 16; i++) {
                sensor_table[index].have_assert_discrete_offset[i] = true;
                sensor_table[index].have_deassert_discrete_offset[i] = true;
                sensor_table[index].have_assert_discrete_offset_retry[i] =
                    defaultDiscreteRetry;
            }
        }

        sensor_table[index].is_discrete_sensor = is_discrete_sensor;

        if (DEBUG_SENSOR_INFO == true) {
            display_sensor_info(&sensor_table[index]);
        }
    }

    FUNC_END();

    return STATUS_OK;
}

/**
 * @brief   Initial sensor dbus
 *
 */
void sensor_dbus_service(void)
{
    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);
    std::string board_path;

    // Init Service and Object
    DPRINT ("Register Service\n");
    conn->request_name(phoenix_sensor_service);
    auto server = sdbusplus::asio::object_server(conn);

    // Try to find Board Path for Association Interface
    // TODO: only retry when entity-manager exist
    if (find_board_path(conn, &board_path, 30) != 0) {
        std::shared_ptr<sdbusplus::asio::dbus_interface> item_board_interface = server.add_interface(
                    base_board_path_prefix,
                    item_board_intferface_name);

        item_board_interface->register_property("Name",
                                                (const char*)"BaseBoard",
                                                sdbusplus::asio::PropertyPermission::readWrite);
        item_board_interface->register_property("Type",
                                                (const char*)"Board",
                                                sdbusplus::asio::PropertyPermission::readWrite);
        item_board_interface->initialize();

        board_path = base_board_path_prefix;
    }

    for (int index = 0; index < sensor_count; index++) {
        // No read function, no need to create interfaces
        if (sensor_table[index].read_func == NULL) {
            continue;
        }

        if (sensor_table[index].obj_path == NULL) {
            continue;
        }

        sensor_table[index].available_intf = server.add_interface(
                sensor_table[index].obj_path,
                available_interface_name);
        sensor_table[index].available_intf->register_property("Available",
                (bool)false,
                sdbusplus::asio::PropertyPermission::readWrite);
        sensor_table[index].available_intf->initialize();


        sensor_table[index].value_intf = server.add_interface(
                                             sensor_table[index].obj_path,
                                             sensor_value_interface_name);

        sensor_table[index].value_intf->register_property("Value",
                (double)0,
                sdbusplus::asio::PropertyPermission::readWrite);

        sensor_table[index].value_intf->register_property("Unit",
                (const char*)sensor_table[index].unit,
                sdbusplus::asio::PropertyPermission::readWrite);
        /*
                sensor_table[index].value_intf->register_property("Scale",
                        (double)sensor_table[index].scale,
                        sdbusplus::asio::PropertyPermission::readWrite);
        */
        sensor_table[index].value_intf->register_property("MaxValue",
                (double)sensor_table[index].max_value,
                sdbusplus::asio::PropertyPermission::readWrite);

        sensor_table[index].value_intf->register_property("MinValue",
                (double)sensor_table[index].min_value,
                sdbusplus::asio::PropertyPermission::readWrite);

        // TODO: to implement Verbose property
        sensor_table[index].value_intf->register_property("Verbose",
                (int32_t)0,
                sdbusplus::asio::PropertyPermission::readWrite);
        sensor_table[index].value_intf->initialize();

        // Threshold interface
        if (sensor_table[index].is_discrete_sensor == false) {
            if (sensor_table[index].have_warn_high == true ||
                sensor_table[index].have_warn_low == true) {
                sensor_table[index].warn_intf = server.add_interface(
                                                    sensor_table[index].obj_path,
                                                    sensor_warning_interface_name);
                if (sensor_table[index].have_warn_high == true) {
                    sensor_table[index].warn_intf->register_property("WarningHigh",
                            sensor_table[index].warn_high,
                            sdbusplus::asio::PropertyPermission::readWrite);
                    sensor_table[index].warn_intf->register_property("WarningAlarmHigh",
                            false,
                            sdbusplus::asio::PropertyPermission::readWrite);
                }
                if (sensor_table[index].have_warn_low == true) {
                    sensor_table[index].warn_intf->register_property("WarningLow",
                            sensor_table[index].warn_low,
                            sdbusplus::asio::PropertyPermission::readWrite);
                    sensor_table[index].warn_intf->register_property("WarningAlarmLow",
                            false,
                            sdbusplus::asio::PropertyPermission::readWrite);
                }
                sensor_table[index].warn_intf->initialize();
            }

            if (sensor_table[index].have_crit_high == true ||
                sensor_table[index].have_crit_low == true) {
                sensor_table[index].crit_intf = server.add_interface(
                                                    sensor_table[index].obj_path,
                                                    sensor_critical_interface_name);
                if (sensor_table[index].have_crit_high == true) {
                    sensor_table[index].crit_intf->register_property("CriticalHigh",
                            sensor_table[index].crit_high,
                            sdbusplus::asio::PropertyPermission::readWrite);
                    sensor_table[index].crit_intf->register_property("CriticalAlarmHigh",
                            false,
                            sdbusplus::asio::PropertyPermission::readWrite);
                }
                if (sensor_table[index].have_crit_low == true) {
                    sensor_table[index].crit_intf->register_property("CriticalLow",
                            sensor_table[index].crit_low,
                            sdbusplus::asio::PropertyPermission::readWrite);

                    sensor_table[index].crit_intf->register_property("CriticalAlarmLow",
                            false,
                            sdbusplus::asio::PropertyPermission::readWrite);
                }
                sensor_table[index].crit_intf->initialize();
            }
        }

        // Discrete interface (follow Gagarin)
        if (sensor_table[index].is_discrete_sensor == true) {
            sensor_table[index].discrete_offset_intf = server.add_interface(
                        sensor_table[index].obj_path,
                        sensor_discrete_interface_name);
            sensor_table[index].discrete_offset_intf->register_property("AssertionAlarmOffset",
                    (unsigned int)255,
                    sdbusplus::asio::PropertyPermission::readWrite);
            sensor_table[index].discrete_offset_intf->register_property("DeassertionAlarmOffset",
                    (unsigned int)255,
                    sdbusplus::asio::PropertyPermission::readWrite);

            sensor_table[index].discrete_offset_intf->initialize();
        }

        // Association interface
        std::shared_ptr<sdbusplus::asio::dbus_interface> association_interface = server.add_interface(
                    sensor_table[index].obj_path,
                    association_interface_name);
        if (board_path.length() > 0) {
            std::vector<Association> associations;
            associations.push_back(
                Association("chassis", "all_sensors", board_path.c_str()));
            association_interface->register_property("Associations", associations);
            association_interface->initialize();
        }

        // Operational interface
        std::shared_ptr<sdbusplus::asio::dbus_interface> operational_interface = server.add_interface(
                    sensor_table[index].obj_path,
                    operational_status_interface_name);
        operational_interface->register_property("Functional",
                (bool)true,
                sdbusplus::asio::PropertyPermission::readWrite);
        operational_interface->initialize();
    }

    sem_post(&semService);

    io.run();
}

/**
 * @brief   Get file size
 *
 * @param   file_name        File name
 * @param   file_size        File size point to save size
 *
 * @return  status code
 */
static int get_file_size(const char *file_name, long *file_size)
{
    int fd;

    FUNC_START();

    if (file_name == NULL) {
        FUNC_END();
        return STATUS_FILE_NAME_NULL;
    }

    fd = open(file_name, O_RDONLY | O_SYNC);

    if (fd < 0) {
        FUNC_END();
        return STATUS_FILE_OPEN_ERROR;
    }

    *file_size = lseek(fd, 0, SEEK_END);

    close(fd);

    FUNC_END();

    return STATUS_OK;
}

/**
 * @brief   Mapping SDR
 *
 * @param   sdr_repository_size        SDR repository size
 * @param   sdr_repository_ptr         SDR repository point
 *
 * @return  status code
 */
static int mapping_sdr_rec(int sdr_repository_size,
                           uint8_t *sdr_repository_ptr)
{
    long offset = 0;
    int sdrIndex = 0;
    struct sdr_rec_header sdr_header;
    static constexpr uint8_t ipmi_sdr_version = 0x51;

    FUNC_START();

    while (offset < sdr_repository_size) {
        // Load SDR header only
        memcpy(&sdr_header, sdr_repository_ptr + offset,
               sizeof(struct sdr_rec_header));

        // Check Header invaild
        if (sdr_header.ver != ipmi_sdr_version ||
            sdr_header.type == 0x00) {
            continue;
        }

        // Save mapping information
        sdr_map_entry[sdrIndex].record_id = sdr_header.record_id;
        sdr_map_entry[sdrIndex].length = sizeof(struct sdr_rec_header)
                                         + sdr_header.length;
        sdr_map_entry[sdrIndex].sdrType = sdr_header.type;

        int nameLength = 0;
        switch (sdr_header.type) {
        case SDR_RECORD_TYPE_FULL_SENSOR:
            nameLength = *(sdr_repository_ptr + offset + 47) & 0x1f; //TODO: create struct, not hard code offset
            sdr_map_entry[sdrIndex].sensor_number = *(sdr_repository_ptr + offset + 7);
            memcpy(sdr_map_entry[sdrIndex].sensor_name, sdr_repository_ptr + offset + 48, nameLength);
            break;

        case SDR_RECORD_TYPE_COMPACT_SENSOR:
            nameLength = *(sdr_repository_ptr + offset + 31) & 0x1f;
            sdr_map_entry[sdrIndex].sensor_number = *(sdr_repository_ptr + offset + 7);
            memcpy(sdr_map_entry[sdrIndex].sensor_name, sdr_repository_ptr + offset + 32, nameLength);
            break;

        case SDR_RECORD_TYPE_EVENTONLY_SENSOR:
            nameLength = *(sdr_repository_ptr + offset + 16) & 0x1f;
            sdr_map_entry[sdrIndex].sensor_number = *(sdr_repository_ptr + offset + 7);
            memcpy(sdr_map_entry[sdrIndex].sensor_name, sdr_repository_ptr + offset + 17, nameLength);
            break;
        }

        // Reocrd offset on sdr_repository_ptr
        sdr_map_entry[sdrIndex].offset = offset;

        sdrIndex++;

        // Go throuth offset to next record
        offset += sizeof(struct sdr_rec_header) + sdr_header.length;

        // Adjust offset for next record
        if ((offset % 16) != 0)
            offset = (offset / 16 + 1) * 16;
    }

    sdr_total_record_count = sdrIndex;

    FUNC_END();

    return STATUS_OK;
}

/**
 * @brief   Load SDR repository
 *
 * @return  status code
 */
static int sdr_repository_load(void)
{
    int status;
    int fd;
    long file_size;
    auto sdr_file_name = ACTIVE_SDR_FILE;

    FUNC_START();

    // Check SDR filesize.
    status = get_file_size(sdr_file_name, &file_size);
    if (status != 0 || file_size <= 1) {
        FUNC_END();
        return status;
    }

    // Allocate memory
    sdr_map_entry = (struct sdr_rec_map *)
                    malloc (sizeof(struct sdr_rec_map) * MAX_SDR_MAP_ENTRIES);

    // Can't allocate memory for sdr_map_entry.
    if (sdr_map_entry == NULL) {
        FUNC_END();
        return STATUS_ALLOCATE_ERROR;
    }

    // Initial sdr_map_entry
    memset(sdr_map_entry, 0, sizeof(struct sdr_rec_map) * MAX_SDR_MAP_ENTRIES);

    sdr_raw_cache_ptr = (uint8_t *) malloc(sizeof(uint8_t) * sdr_raw_cache_ptr_size);

    // Can't allocate memory for sdr_raw_cache_ptr.
    if (sdr_raw_cache_ptr == NULL) {
        free (sdr_map_entry);
        FUNC_END();
        return STATUS_ALLOCATE_ERROR;
    }

    // Initial SDR cache
    memset(sdr_raw_cache_ptr, 0, sdr_raw_cache_ptr_size);

    fd = open(sdr_file_name, O_RDONLY | O_SYNC);
    if (fd < 0) {
        free (sdr_map_entry);
        free (sdr_raw_cache_ptr);
        FUNC_END();
        return STATUS_FILE_OPEN_ERROR;
    }

    if (lseek(fd, 0, SEEK_SET) >= 0) {
        DPRINT ("Read %s to Cache\n", sdr_file_name);
        // Read full SDR file to Cache
        read(fd, sdr_raw_cache_ptr, file_size);
    }

    close(fd);

    // Mapping SDR Record
    mapping_sdr_rec(file_size, sdr_raw_cache_ptr);

    FUNC_END();

    return STATUS_OK;
}

/**
 * @brief   Main function
 *
 * @param   argc     Number of arguments in argv   
 * @param   argv     Arguments
 *
 * @return  status code
 */
int main(int argc, char *argv[])
{
    FUNC_START();

    // Register all signal action
    for (int i = 1; i < NSIG; i++) {
        signal(i, segv_backtrace);
    }
/*
    signal(SIGHUP, segv_backtrace);
    signal(SIGQUIT, segv_backtrace);
    signal(SIGILL, segv_backtrace);
    signal(SIGTRAP, segv_backtrace);
    signal(SIGABRT, segv_backtrace);    
    signal(SIGBUS, segv_backtrace);
    signal(SIGFPE, segv_backtrace);
    signal(SIGUSR1, segv_backtrace);
    signal(SIGSEGV, segv_backtrace);
    signal(SIGUSR2, segv_backtrace);
    signal(SIGPIPE, segv_backtrace);
    signal(SIGALRM, segv_backtrace);
    signal(SIGCHLD, segv_backtrace);
    signal(SIGCONT, segv_backtrace);
    signal(SIGTSTP, segv_backtrace);
    signal(SIGTTIN, segv_backtrace);
    signal(SIGTTOU, segv_backtrace);
    signal(SIGURG, segv_backtrace);
    signal(SIGXCPU, segv_backtrace);
    signal(SIGXFSZ, segv_backtrace);
    signal(SIGVTALRM, segv_backtrace);
    signal(SIGPROF, segv_backtrace);
    signal(SIGWINCH, segv_backtrace);
    signal(SIGIO, segv_backtrace);
    signal(SIGPWR, segv_backtrace);
    signal(SIGTERM, segv_backtrace);
*/
    if (sdr_repository_load() != STATUS_OK) {
        DPRINT ("sdr_repository_load error\n");
        FUNC_END();
        return STATUS_SDR_FILE_FAILED;
    }

    initial_sensor_capability_info();

    int semRet;
    semRet = sem_init(&semService, 0, 0);

    if( semRet ) {
        DPRINT ("semaphore init error\n");
        FUNC_END();
        return STATUS_SEMPHORE_ERROR;
    }

    std::thread phoenix_sensor_dbus_service(sensor_dbus_service);
    std::thread phoenix_sensor_scan_service(sensor_scan_service);

    phoenix_sensor_dbus_service.join();
    phoenix_sensor_scan_service.join();

    FUNC_END();

    return STATUS_OK;
}


