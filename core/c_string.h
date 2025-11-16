#ifndef C_STRING_H
#define C_STRING_H

#include "defines.h"



// Returns the length of the given string.
MAPI u64 c_string_length(const char* str)
{
    return strlen(str);
}

MAPI char* c_string_duplicate(const char* str)
{
    u64 length = c_string_length(str);
    char* copy = malloc(length + 1);
    memcpy(copy, str, length + 1);
    return copy;
};

// Case-sensitive string comparison. True if the same, otherwise false.
MAPI bool c_strings_equal(const char* str0, const char* str1)
{
    return strcmp(str0, str1) == 0;
}

// Performs variadic string formatting to dest given format string and va_list.
MAPI i32 c_string_format_v(char* dest, const char* format, void* va_list)
{
    if (dest) {
        // Big, but can fit on the stack.
        char buffer[32000];
        i32 written = vsnprintf(buffer, 32000, format, va_list);
        buffer[written] = 0;
        memcpy(dest, buffer, written + 1);

        return written;
    }
    return -1;
}


// Performs string formatting to dest given format string and parameters.
MAPI i32 c_string_format(char* dest, const char* format, ...)
{
    if (dest) {
        va_list arg_ptr;
        va_start(arg_ptr, format);
        i32 written = c_string_format_v(dest, format, arg_ptr);
        va_end(arg_ptr);
        return written;
    }
    return -1;
}


#endif //C_STRING_H
