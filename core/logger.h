#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "defines.h"
#include "asserts.h"

//fatal and error will always be logged
#define M_ERROR_ENABLED 1
#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1


#if defined(RELEASE_BUILD)
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level
{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5,
} log_level;

bool logger_init(void);
void logger_shutdown(void);

//log string lookup table
static const char* log_level_string[] = {
    "[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "
};

static const char* colour_strings[] = {
    "0;30;41", "1;31", "1;33", "1;32", "1;34", "1;30"
};

//variadic argument
void log_output(log_level level, const char* message, ...);


#define FATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#if M_ERROR_ENABLED == 1
#define M_ERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
#define WARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define INFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
#define DEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define DEBUG(message, ...)
#endif


#if LOG_TRACE_ENABLED == 1
#define TRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define TRACE(message, ...)
#endif

void report_assertion_failure(const char* expression, const char* message, const char* file, int line);
void report_unimplemented(const char* file, int32_t line);
void report_unreachable(const char* file, int32_t line);


void log_test(void);


#endif //LOGGER_H
