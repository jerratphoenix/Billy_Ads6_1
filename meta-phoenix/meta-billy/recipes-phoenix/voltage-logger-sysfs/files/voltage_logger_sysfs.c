// =============================================================
// Phoenix Voltage Logger (AST2600 Verified - Final)
// -------------------------------------------------------------
// ✅ Platform: AST2600
// ✅ Using /sys/bus/iio/devices/iio:device*/in_voltage*_raw + in_voltage_scale
// ✅ in_voltage_scale unit: mV/LSB  →  multiply by (scale / 1000.0)
// ✅ Formula: Volt = raw * (scale / 1000.0) / divider
// ✅ Output: 2 decimal places, matched to ipmitool SDR readings
// -------------------------------------------------------------
// Author : Phoenix BMC Dev
// Verified: 2025-10-13 (AST2600 real hardware)
// =============================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define MAX_SENSORS 16
#define MAX_LOGS 1200
#define LOG_PATH "/tmp/voltage_dump.log"

#define SCALE0_PATH "/sys/bus/iio/devices/iio:device0/in_voltage_scale"
#define SCALE1_PATH "/sys/bus/iio/devices/iio:device1/in_voltage_scale"
#define DEFAULT_SCALE 2.44140625 // mV/LSB (AST2600 default)

typedef struct {
    const char* name;
    const char* path;
    int deviceId;    // 0 or 1
    double divider;  // voltage divider ratio
    double nominal;  // nominal expected voltage
    double lastValue;
    int riseCount;
} Sensor;

typedef struct {
    char line[1024];
} LogEntry;

// ---------------------- Helper functions ----------------------

static int read_int(const char* path, int* out)
{
    FILE* f = fopen(path, "r");
    if (!f)
        return -1;
    if (fscanf(f, "%d", out) != 1)
    {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

static int read_double(const char* path, double* out)
{
    FILE* f = fopen(path, "r");
    if (!f)
        return -1;
    if (fscanf(f, "%lf", out) != 1)
    {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

// Convert ADC raw to voltage (AST2600)
static double readSensor(const char* path, int deviceId,
                         double scale0, double scale1, double divider)
{
    int raw = 0;
    if (read_int(path, &raw) != 0)
        return -1.0;

    double scale = (deviceId == 0) ? scale0 : scale1;
    if (scale <= 0.0)
        scale = DEFAULT_SCALE;
    if (divider <= 0.0)
        divider = 1.0;

    // ✅ AST2600 correct formula: raw × (scale/1000) / divider
    return raw * (scale / 1000.0) / divider;
}

// Millisecond timestamp
static void getPreciseTimestamp(char* buf, size_t len)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm* tm_info = localtime(&tv.tv_sec);
    int ms = tv.tv_usec / 1000;
    snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1,
             tm_info->tm_mday,
             tm_info->tm_hour,
             tm_info->tm_min,
             tm_info->tm_sec,
             ms);
}

// ---------------------- Main ----------------------

int main(void)
{
    Sensor sensors[MAX_SENSORS] = {
        {"PVCCFAFIVRA_CPU0", "/sys/bus/iio/devices/iio:device0/in_voltage0_raw", 0, 1.0,    1.80, 0, 0},
        {"P12V_AUX",         "/sys/bus/iio/devices/iio:device0/in_voltage1_raw", 0, 0.1572, 12.0, 0, 0},
        {"P3V3",             "/sys/bus/iio/devices/iio:device0/in_voltage2_raw", 0, 0.5714, 3.30, 0, 0},
        {"P5V",              "/sys/bus/iio/devices/iio:device0/in_voltage3_raw", 0, 0.3759, 5.00, 0, 0},
        {"PVNN_PCH_AUX",     "/sys/bus/iio/devices/iio:device0/in_voltage4_raw", 0, 1.0,    0.90, 0, 0},
        {"P1V05_PCH_AUX",    "/sys/bus/iio/devices/iio:device0/in_voltage5_raw", 0, 1.0,    1.05, 0, 0},
        {"P1V8_PCH_AUX",     "/sys/bus/iio/devices/iio:device0/in_voltage6_raw", 0, 1.0,    1.80, 0, 0},
        {"P3V_BAT",          "/sys/bus/iio/devices/iio:device0/in_voltage7_raw", 0, 0.5,    3.00, 0, 0},

        {"PVCCIN_CPU0",      "/sys/bus/iio/devices/iio:device1/in_voltage0_raw", 1, 1.0,    1.80, 0, 0},
        {"PVNN_MAIN_CPU0",   "/sys/bus/iio/devices/iio:device1/in_voltage1_raw", 1, 1.0,    1.00, 0, 0},
        {"PVCCINFAON_CPU0",  "/sys/bus/iio/devices/iio:device1/in_voltage2_raw", 1, 1.0,    1.15, 0, 0},
        {"PVPP_HBM_CPU0",    "/sys/bus/iio/devices/iio:device1/in_voltage3_raw", 1, 0.7463, 0.01, 0, 0},
        {"PVCCFA_CPU0",      "/sys/bus/iio/devices/iio:device1/in_voltage4_raw", 1, 1.0,    1.80, 0, 0},
        {"P3V3_AUX",         "/sys/bus/iio/devices/iio:device1/in_voltage5_raw", 1, 0.5714, 3.30, 0, 0},
        {"PVCCD_HV_CPU0",    "/sys/bus/iio/devices/iio:device1/in_voltage6_raw", 1, 1.0,    1.15, 0, 0},
        {"P5V_AUX",          "/sys/bus/iio/devices/iio:device1/in_voltage7_raw", 1, 0.3759, 5.00, 0, 0},
    };

    double scale0 = 0.0, scale1 = 0.0;
    if (read_double(SCALE0_PATH, &scale0) != 0)
        scale0 = DEFAULT_SCALE;
    if (read_double(SCALE1_PATH, &scale1) != 0)
        scale1 = DEFAULT_SCALE;

    printf("[Voltage Logger] scale0=%.6f, scale1=%.6f\n", scale0, scale1);

    LogEntry ring[MAX_LOGS];
    int head = 0, count = 0;
    const double thresholdRatio = 1.01;
    const double slowInterval = 1.0;
    const double fastInterval = 0.5;
    const int fastHoldCyclesDefault = 6;
    int inFastMode = 0, fastHoldCycles = 0;

    while (1)
    {
        char timestamp[64];
        getPreciseTimestamp(timestamp, sizeof(timestamp));
        char buf[1024] = {0};
        snprintf(buf, sizeof(buf), "%s ", timestamp);

        int wantFast = 0;

        for (int i = 0; i < MAX_SENSORS; i++)
        {
            double value = readSensor(sensors[i].path, sensors[i].deviceId,
                                      scale0, scale1, sensors[i].divider);
            if (value < 0)
            {
                char tmp[64];
                snprintf(tmp, sizeof(tmp), "%s=N/A  ", sensors[i].name);
                strncat(buf, tmp, sizeof(buf) - strlen(buf) - 1);
                continue;
            }

            if (sensors[i].lastValue == 0)
                sensors[i].lastValue = value;

            if (value > sensors[i].nominal * thresholdRatio)
            {
                if (sensors[i].riseCount < 1000)
                    sensors[i].riseCount++;
                if (sensors[i].riseCount >= 2)
                    wantFast = 1;
            }
            else
            {
                sensors[i].riseCount = 0;
            }

            sensors[i].lastValue = value;

            char tmp[64];
            snprintf(tmp, sizeof(tmp), "%s=%.2f  ", sensors[i].name, value);
            strncat(buf, tmp, sizeof(buf) - strlen(buf) - 1);
        }

        snprintf(ring[head].line, sizeof(ring[head].line), "%s\n", buf);
        head = (head + 1) % MAX_LOGS;
        if (count < MAX_LOGS)
            count++;

        const char* TMP_PATH = LOG_PATH ".tmp";
        FILE* f = fopen(TMP_PATH, "w");
        if (f)
        {
            int startIdx = (count == MAX_LOGS) ? head : 0;
            for (int i = 0; i < count; i++)
            {
                int idx = (startIdx + i) % MAX_LOGS;
                fputs(ring[idx].line, f);
            }
            fflush(f);
            fsync(fileno(f));
            fclose(f);
            rename(TMP_PATH, LOG_PATH);
        }

        if (wantFast)
        {
            inFastMode = 1;
            fastHoldCycles = fastHoldCyclesDefault;
        }
        else if (inFastMode)
        {
            if (fastHoldCycles > 0)
                fastHoldCycles--;
            if (fastHoldCycles == 0)
                inFastMode = 0;
        }

        double interval = inFastMode ? fastInterval : slowInterval;
        usleep((useconds_t)(interval * 1e6));
    }

    return 0;
}
