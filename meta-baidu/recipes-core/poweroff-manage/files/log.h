#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

// 日志级别
#define LOG_LEVEL_INFO    1
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_ERROR   3
#define LOG_LEVEL_DEBUG   4

static int LOG_LEVEL = 4;

// 日志宏定义
#define LOG_INFO(...)  do { if (LOG_LEVEL >= LOG_LEVEL_INFO) log_message(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__); } while (0)
#define LOG_WARN(...)  do { if (LOG_LEVEL >= LOG_LEVEL_WARN) log_message(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__); } while (0)
#define LOG_ERROR(...) do { if (LOG_LEVEL >= LOG_LEVEL_ERROR) log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__); } while (0)
#define LOG_DEBUG(...) do { if (LOG_LEVEL >= LOG_LEVEL_DEBUG) log_message(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__); } while (0)

static inline void log_message(int level, const char *file, int line, const char *format, ...) {
    va_list args;
    struct tm *tm_info;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    tm_info = localtime(&tv.tv_sec);

    char* log_tag = "DEBUG";
    switch (level) {
        case LOG_LEVEL_INFO:
            log_tag = "INFO";
            break;
        case LOG_LEVEL_WARN:
            log_tag = "WARN";
            break;
        case LOG_LEVEL_ERROR:
            log_tag = "ERROR";
            break;
        case LOG_LEVEL_DEBUG:
            log_tag = "DEBUG";
            break;
        default:
            log_tag = "UNKNOWN";
            break;
    }

    printf("[%04d-%02d-%02d %02d:%02d:%02d.%06ld] [%-5s] ",
           tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
           tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, tv.tv_usec, log_tag);

    printf("%s:%-3d - ", file, line);

    size_t len = strlen(format);
    char new_format[len + 2];
    strcpy(new_format, format);
    strcat(new_format, "\n");

    va_start(args, format);
    vprintf(new_format, args);
    va_end(args);
}

#endif