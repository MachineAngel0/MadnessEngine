//
// Created by Adams Humbert on 7/31/2025.
//

#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "src/platform/platform.h"

b8 initialize_logging()
{
    //TODO:
    return TRUE;
}

void shutdown_logging()
{
}

void log_output(log_level level, const char* message, ...)
{

    //get errors that are below 2 or below warn
    b8 is_error = level < LOG_LEVEL_WARN;

    //create a string buffer of 32k size,
    //imposing a string limit, but realistically we should never go that high
    i32 message_length = 32000;
    char out_message[message_length]; //automatic memory allocation on the stack
    //zero out the memory
    memset(out_message, 0, message_length);

    // variadic function
    //create the variable arugment list - va_list *var name*
    //initialize the arugment list - va_start(valist, message(...))

    //retrieve arugments va_arg() //NOTE: we dont need this

    //int vsnprintf(char *s, size_t n, const char *format, va_list ap);
    //used for formatted output, similar to snprintf,
    //but designed to work with a variable argument list (va_list) instead of a fixed number of arguments.
    //Its primary purpose is to provide a safe way to format strings into a fixed-size buffer,
    //preventing buffer overflows by limiting the number of characters written.

    //Clean Up with va_end()

    va_list args;
    va_start(args, message);
    vsnprintf(out_message, message_length, message, args);
    va_end(args);



    //get level string for printing
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};

    char out_message2[message_length];
    //we specifally need an out buffer for this
    //combines the level string and the already formatted varaidic function into one
    sprintf(out_message2, "%s%s\n", level_strings[level], out_message);


    if (is_error)
    {
        platform_console_write_error(out_message2, level);
    }else
    {
        platform_console_write(out_message2, level);
    }


}

MAPI void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line)
{
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}
