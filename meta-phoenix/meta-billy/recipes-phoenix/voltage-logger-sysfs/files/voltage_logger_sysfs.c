// =============================================================
// Phoenix Voltage Logger (AST2600 • delta-trigger, no-nominal)
// -------------------------------------------------------------
// • Input: /sys/bus/iio/devices/iio:device*/in_voltage*_raw + in_voltage_scale
// • scale unit: mV/LSB → Volt = raw * (scale/1000.0) / divider
// • Trigger: if ANY rail has |(Vn - Vn-1)/Vn-1| > 1% for TWO consecutive samples
// • Sampling: fast 0.5s (latched 3s, topped up) else 1s
// • Output: two decimals
// =============================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

#define MAX_SENSORS 16
#define MAX_LOGS 1200
#define LOG_PATH "/tmp/voltage_dump.log"

#define SCALE0_PATH "/sys/bus/iio/devices/iio:device0/in_voltage_scale"
#define SCALE1_PATH "/sys/bus/iio/devices/iio:device1/in_voltage_scale"
#define DEFAULT_SCALE 2.44140625 // mV/LSB fallback for AST2600

typedef struct {
    const char* name;
    const char* path;
    int   deviceId;     // 0 or 1
    double divider;     // voltage divider ratio
    double lastValue;   // last valid reading (V)
    int    deltaCount;  // consecutive |Δ|>1% count
    int    hasLast;     // whether lastValue is valid
} Sensor;

typedef struct {
    char line[1024];
} LogEntry;

// ---------- helpers ----------
static int read_int(const char* path, int* out)
{
    FILE* f = fopen(path, "r");
    if (!f) return -1;
    if (fscanf(f, "%d", out) != 1) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

static int read_double(const char* path, double* out)
{
    FILE* f = fopen(path, "r");
    if (!f) return -1;
    if (fscanf(f, "%lf", out) != 1) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

// Convert ADC raw to volts (AST2600): raw × (scale[mV]/1000) / divider
static double convert_to_volt(int raw, double scale_mV_per_lsb, double divider)
{
    if (scale_mV_per_lsb <= 0.0) scale_mV_per_lsb = DEFAULT_SCALE;
    if (divider <= 0.0) divider = 1.0;
    return raw * (scale_mV_per_lsb / 1000.0) / divider;
}

static double readSensor(const char* path, int deviceId,
                         double scale0, double scale1, double divider)
{
    int raw = 0;
    if (read_int(path, &raw) != 0) return -1.0;
    double scale = (deviceId == 0) ? scale0 : scale1;
    return convert_to_volt(raw, scale, divider);
}

// ---------- new helper for D-Bus ----------
static double read_dbus_voltage(const char* sensor_name)
{
    if (strcmp(sensor_name, "P3V_BAT") != 0)
        return -1.0;

    FILE* fp = popen("busctl get-property xyz.openbmc_project.ADCSensor "
                     "/xyz/openbmc_project/sensors/voltage/P3V_BAT "
                     "xyz.openbmc_project.Sensor.Value Value", "r");
    if (!fp) return -1.0;

    char buf[128] = {0};
    if (!fgets(buf, sizeof(buf), fp)) {
        pclose(fp);
        return -1.0;
    }
    pclose(fp);

    // 格式通常是： d 3.056
    double val = 0.0;
    if (sscanf(buf, "d %lf", &val) == 1) {
        return val;
    }
    return -1.0;
}

static void getPreciseTimestamp(char* buf, size_t len)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm* tm_info = localtime(&tv.tv_sec);
    int ms = (int)(tv.tv_usec / 1000);
    snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1,
             tm_info->tm_mday,
             tm_info->tm_hour,
             tm_info->tm_min,
             tm_info->tm_sec,
             ms);
}

// ---------- main ----------
int main(void)
{
    // paths/dividers = 你現場已驗證的對應
    Sensor sensors[MAX_SENSORS] = {
        {"PVCCFAFIVRA_CPU0", "/sys/bus/iio/devices/iio:device0/in_voltage0_raw", 0, 1.0,    0, 0, 0},
        {"P12V_AUX",         "/sys/bus/iio/devices/iio:device0/in_voltage1_raw", 0, 0.1572, 0, 0, 0},
        {"P3V3",             "/sys/bus/iio/devices/iio:device0/in_voltage2_raw", 0, 0.5714, 0, 0, 0},
        {"P5V",              "/sys/bus/iio/devices/iio:device0/in_voltage3_raw", 0, 0.3759, 0, 0, 0},
        {"PVNN_PCH_AUX",     "/sys/bus/iio/devices/iio:device0/in_voltage4_raw", 0, 1.0,    0, 0, 0},
        {"P1V05_PCH_AUX",    "/sys/bus/iio/devices/iio:device0/in_voltage5_raw", 0, 1.0,    0, 0, 0},
        {"P1V8_PCH_AUX",     "/sys/bus/iio/devices/iio:device0/in_voltage6_raw", 0, 1.0,    0, 0, 0},
        {"P3V_BAT",          "/sys/bus/iio/devices/iio:device0/in_voltage7_raw", 0, 0.5,    0, 0, 0},

        {"PVCCIN_CPU0",      "/sys/bus/iio/devices/iio:device1/in_voltage0_raw", 1, 1.0,    0, 0, 0},
        {"PVNN_MAIN_CPU0",   "/sys/bus/iio/devices/iio:device1/in_voltage1_raw", 1, 1.0,    0, 0, 0},
        {"PVCCINFAON_CPU0",  "/sys/bus/iio/devices/iio:device1/in_voltage2_raw", 1, 1.0,    0, 0, 0},
        {"PVPP_HBM_CPU0",    "/sys/bus/iio/devices/iio:device1/in_voltage3_raw", 1, 0.7463, 0, 0, 0},
        {"PVCCFA_CPU0",      "/sys/bus/iio/devices/iio:device1/in_voltage4_raw", 1, 1.0,    0, 0, 0},
        {"P3V3_AUX",         "/sys/bus/iio/devices/iio:device1/in_voltage5_raw", 1, 0.5714, 0, 0, 0},
        {"PVCCD_HV_CPU0",    "/sys/bus/iio/devices/iio:device1/in_voltage6_raw", 1, 1.0,    0, 0, 0},
        {"P5V_AUX",          "/sys/bus/iio/devices/iio:device1/in_voltage7_raw", 1, 0.3759, 0, 0, 0},
    };

    double scale0 = 0.0, scale1 = 0.0;
    if (read_double(SCALE0_PATH, &scale0) != 0) scale0 = DEFAULT_SCALE;
    if (read_double(SCALE1_PATH, &scale1) != 0) scale1 = DEFAULT_SCALE;

    printf("[Voltage Logger] scale0=%.6f mV/LSB, scale1=%.6f mV/LSB\n", scale0, scale1);

    LogEntry ring[MAX_LOGS];
    int head = 0, count = 0;

    // sampling control
    const double changeRatio = 0.01; // 1% vs previous sample
    const double slowInterval = 1.0; // seconds
    const double fastInterval = 0.5; // seconds
    const int fastHoldCyclesDefault = 6; // 6 * 0.5s = 3s

    int inFastMode = 0;
    int fastHoldCycles = 0;

    while (1)
    {
        char timestamp[64];
        getPreciseTimestamp(timestamp, sizeof(timestamp));

        char buf[1024] = {0};
        snprintf(buf, sizeof(buf), "%s ", timestamp);

        int wantFast = 0; // set if any rail hits second consecutive >1% change

        for (int i = 0; i < MAX_SENSORS; i++)
        {
            double value = readSensor(sensors[i].path, sensors[i].deviceId,
                                      scale0, scale1, sensors[i].divider);

            // --- special case: override P3V_BAT via D-Bus ---
            if (strcmp(sensors[i].name, "P3V_BAT") == 0) {
                double dbus_val = read_dbus_voltage("P3V_BAT");
                if (dbus_val > 0.0) {
                    value = dbus_val;
                }
            }

            if (value < 0) {
                // N/A: output and reset state so we don't carry a stale lastValue
                char tmp[64];
                snprintf(tmp, sizeof(tmp), "%s=N/A  ", sensors[i].name);
                strncat(buf, tmp, sizeof(buf) - strlen(buf) - 1);
                sensors[i].hasLast = 0;
                sensors[i].deltaCount = 0;
                continue;
            }

            // delta-based trigger vs previous valid sample
            if (sensors[i].hasLast && fabs(sensors[i].lastValue) > 1e-9) {
                double delta = fabs((value - sensors[i].lastValue) / sensors[i].lastValue);
                if (delta > changeRatio) {
                    if (sensors[i].deltaCount < 1000) sensors[i].deltaCount++;
                    if (sensors[i].deltaCount >= 2) wantFast = 1;
                } else {
                    sensors[i].deltaCount = 0;
                }
            } else {
                sensors[i].deltaCount = 0;
                sensors[i].hasLast = 1;
            }

            sensors[i].lastValue = value;

            // two-decimal output
            char tmp[64];
            snprintf(tmp, sizeof(tmp), "%s=%.2f  ", sensors[i].name, value);
            strncat(buf, tmp, sizeof(buf) - strlen(buf) - 1);
        }

        // ring buffer + atomic dump
        snprintf(ring[head].line, sizeof(ring[head].line), "%s\n", buf);
        head = (head + 1) % MAX_LOGS;
        if (count < MAX_LOGS) count++;

        const char* TMP_PATH = LOG_PATH ".tmp";
        FILE* f = fopen(TMP_PATH, "w");
        if (f) {
            int startIdx = (count == MAX_LOGS) ? head : 0;
            for (int i = 0; i < count; i++) {
                int idx = (startIdx + i) % MAX_LOGS;
                fputs(ring[idx].line, f);
            }
            fflush(f);
            fsync(fileno(f));
            fclose(f);
            rename(TMP_PATH, LOG_PATH);
        }

        // fast-mode state machine
        if (wantFast) {
            inFastMode = 1;
            fastHoldCycles = fastHoldCyclesDefault; // top up
        } else if (inFastMode) {
            if (fastHoldCycles > 0) fastHoldCycles--;
            if (fastHoldCycles == 0) inFastMode = 0;
        }

        double interval = inFastMode ? fastInterval : slowInterval;
        usleep((useconds_t)(interval * 1e6));
    }

    return 0;
}

