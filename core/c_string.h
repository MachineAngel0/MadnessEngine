#ifndef C_STRING_H
#define C_STRING_H

#include "defines.h"



// Returns the length of the given string.
MAPI u64 c_string_length(const char* str)
{
    //does not count the null terminated string
    return strlen(str);
}

MAPI char* c_string_duplicate(const char* str)
{
    u64 length = c_string_length(str);
    char* copy = malloc(length + 1);
    memcpy(copy, str, length + 1);
    return copy;
};


MAPI const char* c_string_concat(const char* str1, const char* str2)
{
    u64 str1_length = c_string_length(str1);
    u64 str2_length = c_string_length(str2);


    //+1 for the null char
    char* out_str = malloc(str1_length + str2_length + 1);
    memcpy(out_str, str1, str1_length);
    memcpy(out_str, str2, str1_length + str2_length);
    out_str[str1_length + str2_length + 1] = '\0';

    return out_str;
};



// Performs string formatting to dest given format string and parameters.
MAPI char* c_string_path_strip(const char* path, Arena* arena)
{
    MASSERT(path);

    char* new_path = NULL;
    int i = strlen(path);
    for (; i > 0; i--)
    {
        if (path[i] == '/')
        {
            new_path = arena_alloc(arena, i + 2);
            memcpy(new_path, path, i + 1);
            new_path[i + 1] = '\0';
            return new_path;

        }
    }

    //basically was not a valid path string
    MASSERT_MSG(new_path, "C_STRING_PATH_STRIP: PATH STRING DOES NOT CONTAIN /");
    return "";
}


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
