#pragma once

#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1
//we need to define warning levels
// Disable debug and trace logging for release builds.
#if MRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif


typedef enum log_level
{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_DEBUG = 3,
    LOG_LEVEL_INFO = 4,
    LOG_LEVEL_TRACE = 5,

}log_level;


//log to a file
b8 initialize_logging();
void shutdown_logging();

//we want to be able to pass in a warning level, and a message, then extra arugments (variadic function)
MAPI void log_output(log_level level, const char* message, ...);




//we want to expand out the debug type for less typing
///the ## removes the comma after the variadic so (message, ..., )
#define MFATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)

#ifndef MERROR
#define MERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)
#endif

#if LOG_WARN_ENABLED == 1
#define MWARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#else
// Does nothing when LOG_WARN_ENABLED != 1
#define MWARN(message, ...)
#endif


#if LOG_DEBUG_ENABLED == 1
// Logs a debug-level message.
#define MDEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
// Does nothing when LOG_DEBUG_ENABLED != 1
#define MDEBUG(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
#define MINFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
// Does nothing when LOG_INFO_ENABLED != 1
#define MINFO(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
#define MTRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#else
// Does nothing when LOG_INFO_ENABLED != 1
#define MTRACE(message, ...)
#endif
