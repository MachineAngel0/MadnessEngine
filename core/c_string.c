#include "c_string.h"
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
}


MAPI const char* c_string_concat(const char* str1, const char* str2)
{
    u64 str1_length = c_string_length(str1);
    u64 str2_length = c_string_length(str2);


    //+1 for the null char
    char* out_str = malloc(str1_length + str2_length + 1);
    memcpy(out_str, str1, str1_length);
    memcpy(out_str + str1_length, str2, str2_length);
    out_str[str1_length + str2_length + 1] = '\0';

    return out_str;
};


// Performs string formatting to dest given format string and parameters.
MAPI char* c_string_path_strip(const char* path, Arena* arena)
{
    MASSERT(path);

    char* new_path = NULL;
    size_t i = strlen(path);
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

char* c_string_path_get_extension(const char* path, Arena* arena)
{
    //NOTE: does return the dot  ex: .gltf / .txt and not gltf/txt
    MASSERT(path);
    MASSERT(arena);


    char* out_string_extension = NULL;

    size_t string_size = strlen(path);
    MASSERT(string_size > 0);

    for (size_t i = string_size; i > 0; i--)
    {
        if (path[i] == '.')
        {
            size_t diff = string_size - i; // difference from where we started
            out_string_extension = arena_alloc(arena, diff + 1);

            size_t new_string_idx = 0;
            for (size_t j = i; j < string_size; j++)
            {
                out_string_extension[new_string_idx] = path[j];
                new_string_idx++;
            }
            out_string_extension[diff] = '\0';

            break;
        }
    }


    return out_string_extension;
}

bool c_string_path_is_extension(const char* path, const char* extensions_name)
{
    //this is a simpler version of the one below
    size_t path_len = strlen(path);
    size_t extension_len = strlen(extensions_name);
    size_t start_idx = path_len - extension_len;
    if (strcmp(path + start_idx, extensions_name) == 0)
    {
        return true;
    }

    return false;
    /*
    size_t path_len = strlen(path);
    size_t extension_len = strlen(extensions_name);

    size_t start_idx = path_len - extension_len;
    size_t ext_idx = 0;
    for (; start_idx < path_len; start_idx++)
    {
        printf("%c %c\n", path[start_idx], extensions_name[ext_idx]);
        if (path[start_idx] != extensions_name[ext_idx])
        {
            return false;
        }
         ext_idx++;
    }

    return true;
    */
}


// Case-sensitive string comparison. True if the same, otherwise false.
MAPI bool c_strings_equal(const char* str0, const char* str1)
{
    return strcmp(str0, str1) == 0;
}

// Performs variadic string formatting to dest given format string and va_list.
MAPI i32 c_string_format_v(char* dest, const char* format, void* va_list)
{
    if (dest)
    {
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
    if (dest)
    {
        va_list arg_ptr;
        va_start(arg_ptr, format);
        i32 written = c_string_format_v(dest, format, arg_ptr);
        va_end(arg_ptr);
        return written;
    }
    return -1;
}
