#ifndef C_STRING_H
#define C_STRING_H

// Returns the length of the given string.
MAPI u64 c_string_length(const char* str);

MAPI char* c_string_duplicate(const char* str);

MAPI const char* c_string_concat(const char* str1, const char* str2);


// Performs string formatting to dest given format string and parameters.
MAPI char* c_string_path_strip(const char* path, Arena* arena);


// Case-sensitive string comparison. True if the same, otherwise false.
MAPI bool c_strings_equal(const char* str0, const char* str1);

// Performs variadic string formatting to dest given format string and va_list.
MAPI i32 c_string_format_v(char* dest, const char* format, void* va_list);


// Performs string formatting to dest given format string and parameters.
MAPI i32 c_string_format(char* dest, const char* format, ...);





#endif //C_STRING_H
