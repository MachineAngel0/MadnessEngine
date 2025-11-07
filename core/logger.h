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
}log_level;

bool logger_init()
{
    //TODO: LOG FILE
    return true;
}
void logger_shutdown()
{

}

//log string lookup table
static const char* log_level_string[] = {
    "[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "
};

static const char* colour_strings[] = {
    "0;30;41", "1;31", "1;33", "1;32", "1;34", "1;30"};

//variadic argument
void log_output(log_level level, const char *message, ...)
{

    //stack allocation
    //32k string length limitation (wtf are you writing that's more than that)
    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));

    //create our formated variadic argument string
    // __builtin_va_list args_ptr; // apparently you should just use va_list
    va_list args_ptr;
    va_start(args_ptr, message);
    vsnprintf(out_message, 32000, message, args_ptr);
    va_end(args_ptr);

    char out_message2[32000];
    //takes a buffer, message format, then the remaining strings
    sprintf(out_message2, "%s%s\n", log_level_string[level], out_message);


    b8 is_error = (level == LOG_LEVEL_ERROR || level == LOG_LEVEL_FATAL);
    FILE* console_handle = is_error ? stderr : stdout;

    //print message
    // printf("%s", out_message2);
    fprintf(console_handle, "\033[%sm%s\033[0m", colour_strings[level], out_message2);


}

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

void report_assertion_failure(const char* expression, const char* message, const char* file, int line)
{
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}



void log_test()
{
    FATAL("FATAL TEST MESSAGE %d, %f, %c, %s", 3, 10.0f, 'C', "STRING");
    M_ERROR("ERROR TEST MESSAGE %d, %f, %c, %s", 3, 10.0f, 'C', "STRING");
    WARN("WARN TEST MESSAGE %d, %f, %c, %s", 3, 10.0f, 'C', "STRING");
    INFO("INFO TEST MESSAGE %d, %f, %c, %s", 3, 10.0f, 'C', "STRING");
    DEBUG("DEBUG TEST MESSAGE %d, %f, %c, %s", 3, 10.0f, 'C', "STRING");
    TRACE("TRACE TEST MESSAGE %d, %f, %c, %s", 3, 10.0f, 'C', "STRING");

    // MASSERT( 1== 2);
    // MASSERT_MSG( 1== 2, "OH NO HOW COULD THIS HAVE FAILED");

}



#endif //LOGGER_H
