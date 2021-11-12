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

#include <iostream>
#include <string.h>
#include <time.h>

#include <boost/algorithm/string.hpp>
#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/server.hpp>

#include "debug.hpp"

constexpr const char* phoenix_sensor_service =
    "xyz.openbmc_project.phoenix-dbus-sensors";

constexpr const char* specific_sensor_path_prefix =
    "/xyz/openbmc_project/sensors/specific/";
constexpr const char* temperature_sensor_path_prefix =
    "/xyz/openbmc_project/sensors/temperature/";
constexpr const char* fan_tach_sensor_path_prefix =
    "/xyz/openbmc_project/sensors/fan_tach/";
constexpr const char* pwm_sensor_path_prefix =
    "/xyz/openbmc_project/sensors/fan_pwm/";
constexpr const char* voltage_sensor_path_prefix =
    "/xyz/openbmc_project/sensors/voltage/";
constexpr const char* current_sensor_path_prefix =
    "/xyz/openbmc_project/sensors/current/";
constexpr const char* power_sensor_path_prefix =
    "/xyz/openbmc_project/sensors/power/";
constexpr const char* base_board_path_prefix =
    "/xyz/openbmc_project/inventory/system/board/Baseboard";

constexpr const char* sensor_value_interface_name =
    "xyz.openbmc_project.Sensor.Value";
constexpr const char* sensor_warning_interface_name =
    "xyz.openbmc_project.Sensor.Threshold.Warning";
constexpr const char* sensor_critical_interface_name =
    "xyz.openbmc_project.Sensor.Threshold.Critical";

//Follow Gagarin's discrete interface
constexpr const char* sensor_discrete_interface_name =
    "xyz.openbmc_project.Sensor.Discrete.Event";

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

constexpr const char* sensor_unit_degreesc =
    "xyz.openbmc_project.Sensor.Value.Unit.DegreesC";
constexpr const char* sensor_unit_rpms =
    "xyz.openbmc_project.Sensor.Value.Unit.RPMS";
constexpr const char* sensor_unit_percents =
    "xyz.openbmc_project.Sensor.Value.Unit.Percents";
constexpr const char* sensor_unit_volts =
    "xyz.openbmc_project.Sensor.Value.Unit.Volts";
constexpr const char* sensor_unit_meters =
    "xyz.openbmc_project.Sensor.Value.Unit.Meters";
constexpr const char* sensor_unit_ampers =
    "xyz.openbmc_project.Sensor.Value.Unit.Amperes";
constexpr const char* sensor_unit_watts =
    "xyz.openbmc_project.Sensor.Value.Unit.Watts";
constexpr const char* sensor_unit_joules =
    "xyz.openbmc_project.Sensor.Value.Unit.Joules";
constexpr const char* sensor_unit_none =
    "xyz.openbmc_project.Sensor.Value.Unit.None";

using Association = std::tuple<std::string, std::string, std::string>;

enum SENSOR_STATUS {
    NORMAL,
    NORMAL_AND_EVENT_HANDLED,
    ABSENT,
    UNAVAILABLE,
    ERROR,
};

enum LEVEL {
    WARNING,
    CRITICAL
};

enum DIRECTION {
    HIGH,
    LOW
};

enum DISCRETE_OFFSET {
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
enum UNIT_TYPE_CODE {
    UNSPECIFIED = 0,
    DEGREES_C   = 1,
    DEFREES_F   = 2,
    DEFREES_K   = 3,
    VOLTS       = 4,
    AMPS        = 5,
    WATTS       = 6,
    RPM         = 18,
};

enum STATUS_CODE {
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

struct sdr_rec_map {
    long offset;
    uint16_t record_id;
    uint8_t length;
    uint8_t sdrType;
    uint16_t sensor_number;
    char sensor_name[32];
} __attribute__ ((packed));

#define BIT(n)    (1 << n)

struct sensor_var_t {

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

    long int scan_period; //ms
    long int power_on_delay; //ms
    long int power_off_delay; //ms

    char obj_path[128];
    char unit[128];
    int32_t (*read_func) (double*);
    double value;
    struct sdr_rec_map *sdr_mapping;
//} __attribute__ ((packed));
};

#define SDR_RECORD_TYPE_FULL_SENSOR             0x01
#define SDR_RECORD_TYPE_COMPACT_SENSOR          0x02
#define SDR_RECORD_TYPE_EVENTONLY_SENSOR        0x03
#define SDR_RECORD_TYPE_ENTITY_ASSOC            0x08
#define SDR_RECORD_TYPE_DEVICE_ENTITY_ASSOC     0x09
#define SDR_RECORD_TYPE_GENERIC_DEVICE_LOCATOR  0x10
#define SDR_RECORD_TYPE_FRU_DEVICE_LOCATOR      0x11
#define SDR_RECORD_TYPE_MC_DEVICE_LOCATOR       0x12
#define SDR_RECORD_TYPE_MC_CONFIRMATION         0x13
#define SDR_RECORD_TYPE_BMC_MSG_CHANNEL_INFO    0x14
#define SDR_RECORD_TYPE_OEM                     0xc0

struct sdr_rec_header {
    uint16_t record_id;
    uint8_t ver;
    uint8_t type;
    uint8_t length;
} __attribute__ ((packed));

#define tos32(val, bits)    ((val & ((1<<((bits)-1)))) ? (-((val) & (1<<((bits)-1))) | (val)) : (val))
# define BSWAP_16(x) ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))
# define BSWAP_32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) |\
                     (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))

# define __TO_TOL(mtol)     (uint16_t)(BSWAP_16(mtol) & 0x3f)
# define __TO_M(mtol)       (int16_t)(tos32((((BSWAP_16(mtol) & 0xff00) >> 8) | ((BSWAP_16(mtol) & 0xc0) << 2)), 10))
# define __TO_B(bacc)       (int32_t)(tos32((((BSWAP_32(bacc) & 0xff000000) >> 24) | \
                            ((BSWAP_32(bacc) & 0xc00000) >> 14)), 10))
# define __TO_ACC(bacc)     (uint32_t)(((BSWAP_32(bacc) & 0x3f0000) >> 16) | ((BSWAP_32(bacc) & 0xf000) >> 6))
# define __TO_ACC_EXP(bacc) (uint32_t)((BSWAP_32(bacc) & 0xc00) >> 10)
# define __TO_R_EXP(bacc)   (int32_t)(tos32(((BSWAP_32(bacc) & 0xf0) >> 4), 4))
# define __TO_B_EXP(bacc)   (int32_t)(tos32((BSWAP_32(bacc) & 0xf), 4))

struct sdr_record_mask {
    union {
        struct {
            uint16_t assert_event;	/* assertion event mask */
            uint16_t deassert_event;	/* de-assertion event mask */
            uint16_t read;	/* discrete reading mask */
        } __attribute__ ((packed)) discrete;

        struct {
            uint16_t assert_lnc_low:1;
            uint16_t assert_lnc_high:1;
            uint16_t assert_lcr_low:1;
            uint16_t assert_lcr_high:1;
            uint16_t assert_lnr_low:1;
            uint16_t assert_lnr_high:1;
            uint16_t assert_unc_low:1;
            uint16_t assert_unc_high:1;
            uint16_t assert_ucr_low:1;
            uint16_t assert_ucr_high:1;
            uint16_t assert_unr_low:1;
            uint16_t assert_unr_high:1;
            uint16_t status_lnc:1;
            uint16_t status_lcr:1;
            uint16_t status_lnr:1;
            uint16_t reserved:1;

            uint16_t deassert_lnc_low:1;
            uint16_t deassert_lnc_high:1;
            uint16_t deassert_lcr_low:1;
            uint16_t deassert_lcr_high:1;
            uint16_t deassert_lnr_low:1;
            uint16_t deassert_lnr_high:1;
            uint16_t deassert_unc_low:1;
            uint16_t deassert_unc_high:1;
            uint16_t deassert_ucr_low:1;
            uint16_t deassert_ucr_high:1;
            uint16_t deassert_unr_low:1;
            uint16_t deassert_unr_high:1;
            uint16_t status_unc:1;
            uint16_t status_ucr:1;
            uint16_t status_unr:1;
            uint16_t reserved_2:1;

            union {
                struct {
                    uint16_t r_lnc:1;
                    uint16_t r_lcr:1;
                    uint16_t r_lnr:1;
                    uint16_t r_unc:1;
                    uint16_t r_ucr:1;
                    uint16_t r_unr:1;
                    uint16_t reserved_1:2;
                    uint16_t s_lnc:1;
                    uint16_t s_lcr:1;
                    uint16_t s_lnr:1;
                    uint16_t s_unc:1;
                    uint16_t s_ucr:1;
                    uint16_t s_unr:1;
                    uint16_t reserved_2:2;
                } __attribute__ ((packed)) init; /* Threshold Mask Init */

                struct {
                    uint16_t readable:8;
                    uint16_t lnc:1;
                    uint16_t lcr:1;
                    uint16_t lnr:1;
                    uint16_t unc:1;
                    uint16_t ucr:1;
                    uint16_t unr:1;
                    uint16_t reserved:2;
                } __attribute__ ((packed)) set; /* Settable Threshold Mask (for threshold-base sensors) */

                struct {
                    uint16_t lnc:1;
                    uint16_t lcr:1;
                    uint16_t lnr:1;
                    uint16_t unc:1;
                    uint16_t ucr:1;
                    uint16_t unr:1;
                    uint16_t reserved:2;
                    uint16_t settable:8;
                } __attribute__ ((packed)) read;	/* Readable Threshold Mask (for threshold-base sensors) */
            } __attribute__ ((packed)) rw;
        } __attribute__ ((packed)) threshold;
    } __attribute__ ((packed)) type;
} __attribute__ ((packed));


struct entity_id {
    uint8_t	id;			/* physical entity id */
    uint8_t	instance    : 7;	/* instance number */
    uint8_t	logical     : 1;	/* physical/logical */
} __attribute__ ((packed));

/* SDR type 01h, Full Sensor Record */
struct sdr_record_full_sensor {
    struct sdr_rec_header  header;

    struct {
        uint8_t owner_id;

        uint8_t lun:2;	/* sensor owner lun */
        uint8_t __reserved:2;
        uint8_t channel:4;	/* channel number */

        uint8_t sensor_num;	/* unique sensor number */
    } __attribute__ ((packed)) keys;

    struct entity_id entity;

    struct {
        struct {
            uint8_t sensor_scan:1;
            uint8_t event_gen:1;
            uint8_t type:1;
            uint8_t hysteresis:1;
            uint8_t thresholds:1;
            uint8_t events:1;
            uint8_t scanning:1;
            uint8_t settable:1; // in new spec
        } __attribute__ ((packed)) init;
        struct {
            uint8_t event_msg:2;
            uint8_t threshold:2;
            uint8_t hysteresis:2;
            uint8_t rearm:1;
            uint8_t ignore:1;
        } __attribute__ ((packed)) capabilities;
        uint8_t type;
    } __attribute__ ((packed)) sensor;

    uint8_t event_type;	/* event/reading type code */

    struct sdr_record_mask mask;

    struct {
        uint8_t pct:1;
        uint8_t modifier:2;
        uint8_t rate:3;
        uint8_t analog:2;

        struct {
            uint8_t base;
            uint8_t modifier;
        } __attribute__ ((packed)) type;
    } __attribute__ ((packed)) unit;

#define SDR_SENSOR_L_LINEAR     0x00
#define SDR_SENSOR_L_LN         0x01
#define SDR_SENSOR_L_LOG10      0x02
#define SDR_SENSOR_L_LOG2       0x03
#define SDR_SENSOR_L_E          0x04
#define SDR_SENSOR_L_EXP10      0x05
#define SDR_SENSOR_L_EXP2       0x06
#define SDR_SENSOR_L_1_X        0x07
#define SDR_SENSOR_L_SQR        0x08
#define SDR_SENSOR_L_CUBE       0x09
#define SDR_SENSOR_L_SQRT       0x0a
#define SDR_SENSOR_L_CUBERT     0x0b
#define SDR_SENSOR_L_NONLINEAR  0x70

    uint8_t linearization;	/* 70h=non linear, 71h-7Fh=non linear, OEM */
    uint16_t mtol;		/* M, tolerance */
    uint32_t bacc;		/* accuracy, B, Bexp, Rexp */

    struct {
        uint8_t nominal_read:1;	/* nominal reading field specified */
        uint8_t normal_max:1;	/* normal max field specified */
        uint8_t normal_min:1;	/* normal min field specified */
        uint8_t __reserved:5;
    } __attribute__ ((packed)) analog_flag;

    uint8_t nominal_read;	/* nominal reading, raw value */
    uint8_t normal_max;	/* normal maximum, raw value */
    uint8_t normal_min;	/* normal minimum, raw value */
    uint8_t sensor_max;	/* sensor maximum, raw value */
    uint8_t sensor_min;	/* sensor minimum, raw value */

    struct {
        struct {
            uint8_t non_recover;
            uint8_t critical;
            uint8_t non_critical;
        } __attribute__ ((packed)) upper;
        struct {
            uint8_t non_recover;
            uint8_t critical;
            uint8_t non_critical;
        } __attribute__ ((packed)) lower;
        struct {
            uint8_t positive;
            uint8_t negative;
        } __attribute__ ((packed)) hysteresis;
    } __attribute__ ((packed)) threshold;
    uint8_t __reserved[2];
    uint8_t oem;		/* reserved for OEM use */
    struct {
        uint8_t len:5;
        uint8_t __reserved:1;
#define IPMI_TYPE_LENGTH_UNICODE    0x00
#define IPMI_TYPE_LENGTH_BCD_PLUS   0x01
#define IPMI_TYPE_LENGTH_6BIT_ASCII 0x02
#define IPMI_TYPE_LENGTH_8BIT_ASCII 0x03
        uint8_t type:2;
    } __attribute__ ((packed)) id_code; /* sensor ID string type/length code */
    uint8_t id_string[16];	/* sensor ID string bytes, only if id_code != 0 */
} __attribute__ ((packed));


/* SDR type 02h, Compact Sensor Record */
struct sdr_record_compact_sensor {
    struct sdr_rec_header  header;

    struct {
        uint8_t owner_id;
        uint8_t lun:2;	        /* sensor owner lun */
        uint8_t fru_lun:2;      /* FRU Inventory Device Owner LUN */
        uint8_t channel:4;	/* channel number */

        uint8_t sensor_num;	/* unique sensor number (Init) */
    } __attribute__ ((packed)) keys;

    struct entity_id entity;

    struct {
        struct {
            uint8_t sensor_scan:1;
            uint8_t event_gen:1;
            uint8_t type:1;
            uint8_t hysteresis:1;
            uint8_t thresholds:1;
            uint8_t events:1;
            uint8_t scanning:1;
            uint8_t settable:1;
        } __attribute__ ((packed)) init;		         /* initialization  */

        struct {
            uint8_t event_msg:2;
            uint8_t threshold:2;
            uint8_t hysteresis:2;
            uint8_t rearm:1;
            uint8_t ignore:1;
        } __attribute__ ((packed)) capabilities;	         /* capabilities */

        uint8_t type;	         /* sensor type */
    } __attribute__ ((packed)) sensor;

    uint8_t event_type;	         /* event/reading type code */

    struct sdr_record_mask mask;

    struct {
        uint8_t pct:1;
        uint8_t modifier:2;
        uint8_t rate:3;
        uint8_t __reserved:2;
        struct {
            uint8_t base;
            uint8_t modifier;
        } __attribute__ ((packed)) type;
    } __attribute__ ((packed)) unit;

    struct {
        uint8_t count:4;
        uint8_t mod_type:2;
        uint8_t __reserved:2;
        uint8_t mod_offset:7;
        uint8_t entity_inst:1;
    } __attribute__ ((packed)) share;

    struct {
        struct {
            uint8_t positive;
            uint8_t negative;
        } __attribute__ ((packed)) hysteresis;
    } __attribute__ ((packed)) threshold;

    uint8_t __reserved[3];
    uint8_t oem;		/* reserved for OEM use */
    struct {
        uint8_t len:5;
        uint8_t __reserved:1;
        uint8_t type:2;
    } __attribute__ ((packed)) id_code; /* sensor ID string type/length code */

    uint8_t id_string[16];	/* sensor ID string bytes, only if id_code != 0 */
} __attribute__ ((packed));


// SDR type x03
struct sdr_record_eventonly_sensor {

    struct sdr_rec_header  header;

    struct {
        uint8_t owner_id;

        uint8_t lun:2;	/* sensor owner lun */
        uint8_t fru_owner:2;	/* fru device owner lun */
        uint8_t channel:4;	/* channel number */

        uint8_t sensor_num;	/* unique sensor number */
    } __attribute__ ((packed)) keys;

    struct entity_id entity;

    uint8_t sensor_type;	/* sensor type */
    uint8_t event_type;	/* event/reading type code */

    struct {
        uint8_t count:4;
        uint8_t mod_type:2;
        uint8_t __reserved:2;

        uint8_t mod_offset:7;
        uint8_t entity_inst:1;
    } __attribute__ ((packed)) share;

    uint8_t __reserved;
    uint8_t oem;		/* reserved for OEM use */
    struct {
        uint8_t len:5;
        uint8_t __reserved:1;
        uint8_t type:2;
    } __attribute__ ((packed)) id_code; /* sensor ID string type/length code */
    uint8_t id_string[16];	/* sensor ID string bytes, only if id_code != 0 */

} __attribute__ ((packed));

struct VariantToStrVisitor {
    template <typename T> std::string operator()(const T& t) const
    {
        return static_cast<std::string>(t);
    }
};

int add_ipmi_std_sel_entry(std::string ipmiSELAddMessage,
                           std::string sensorPath,
                           std::vector<uint8_t> eventData,
                           bool assert,
                           uint16_t generatorID);

