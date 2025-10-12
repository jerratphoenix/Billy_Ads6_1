// Phoenix Voltage Logger (IIO-based, ms precision, calibrated, nominal trigger, latched fast mode)
// - 1s sampling (default), 0.5s fast mode when any rail > +1% nominal for 2 consecutive samples
// - Fast mode is latched with a hold window and topped-up while condition persists
// - Millisecond timestamps, 10 minutes ring buffer (1200), atomic write to /tmp/voltage_dump.log
// - Reads 16 ADC rails via /sys/bus/iio/devices/iio:device{0,1}/in_voltage*_raw

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

typedef struct {
    const char* name;
    const char* path;
    double scaleFactor;
    double nominal;
    double lastValue;
    int riseCount;
} Sensor;

typedef struct {
    char line[512];
} LogEntry;

// read raw ADC value and convert to volts (scale=1 assumed)
static double readSensor(const char* path, double scaleFactor)
{
    FILE* f = fopen(path, "r");
    if (!f)
        return -1.0;
    int raw = 0;
    if (fscanf(f, "%d", &raw) != 1)
    {
        fclose(f);
        return -1.0;
    }
    fclose(f);
    return raw * scaleFactor / 1000.0; // convert mV (scale≈1, raw~1000 per volt)
}

// get timestamp with millisecond precision
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

int main(void)
{
    Sensor sensors[MAX_SENSORS] = {
        {"PVCCFAFIVRA_CPU0", "/sys/bus/iio/devices/iio:device0/in_voltage0_raw", 1.0, 1.80, 0, 0},
        {"P12V_AUX",         "/sys/bus/iio/devices/iio:device0/in_voltage1_raw", 0.1572, 12.0, 0, 0},
        {"P3V3",             "/sys/bus/iio/devices/iio:device0/in_voltage2_raw", 0.5714, 3.30, 0, 0},
        {"P5V",              "/sys/bus/iio/devices/iio:device0/in_voltage3_raw", 0.3759, 5.00, 0, 0},
        {"PVNN_PCH_AUX",     "/sys/bus/iio/devices/iio:device0/in_voltage4_raw", 1.0, 0.90, 0, 0},
        {"P1V05_PCH_AUX",    "/sys/bus/iio/devices/iio:device0/in_voltage5_raw", 1.0, 1.05, 0, 0},
        {"P1V8_PCH_AUX",     "/sys/bus/iio/devices/iio:device0/in_voltage6_raw", 1.0, 1.80, 0, 0},
        {"P3V_BAT",          "/sys/bus/iio/devices/iio:device0/in_voltage7_raw", 0.5, 3.00, 0, 0},
        {"PVCCIN_CPU0",      "/sys/bus/iio/devices/iio:device1/in_voltage0_raw", 1.0, 1.80, 0, 0},
        {"PVNN_MAIN_CPU0",   "/sys/bus/iio/devices/iio:device1/in_voltage1_raw", 1.0, 1.00, 0, 0},
        {"PVCCINFAON_CPU0",  "/sys/bus/iio/devices/iio:device1/in_voltage2_raw", 1.0, 1.15, 0, 0},
        {"PVPP_HBM_CPU0",    "/sys/bus/iio/devices/iio:device1/in_voltage3_raw", 0.7463, 0.01, 0, 0},
        {"PVCCFA_CPU0",      "/sys/bus/iio/devices/iio:device1/in_voltage4_raw", 1.0, 1.80, 0, 0},
        {"P3V3_AUX",         "/sys/bus/iio/devices/iio:device1/in_voltage5_raw", 0.5714, 3.30, 0, 0},
        {"PVCCD_HV_CPU0",    "/sys/bus/iio/devices/iio:device1/in_voltage6_raw", 1.0, 1.15, 0, 0},
        {"P5V_AUX",          "/sys/bus/iio/devices/iio:device1/in_voltage7_raw", 0.3759, 5.00, 0, 0},
    };

    LogEntry ring[MAX_LOGS];
    int head = 0, count = 0;
    const double thresholdRatio = 1.01; // +1%
    const double slowInterval = 1.0;
    const double fastInterval = 0.5;
    const int fastHoldCyclesDefault = 6;

    int inFastMode = 0;
    int fastHoldCycles = 0;

    while (1)
    {
        char timestamp[64];
        getPreciseTimestamp(timestamp, sizeof(timestamp));
        char buf[1024] = {0};
        snprintf(buf, sizeof(buf), "%s ", timestamp);

        int wantFast = 0;

        for (int i = 0; i < MAX_SENSORS; i++)
        {
            double value = readSensor(sensors[i].path, sensors[i].scaleFactor);
            if (value < 0)
            {
                char tmp[64];
                snprintf(tmp, sizeof(tmp), "%s=N/A  ", sensors[i].name);
                strcat(buf, tmp);
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
            snprintf(tmp, sizeof(tmp), "%s=%.3f  ", sensors[i].name, value);
            strcat(buf, tmp);
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
                fprintf(f, "%s", ring[idx].line);
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
