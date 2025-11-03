//
#ifndef STRINGS_BUILDER_H
#define STRINGS_BUILDER_H

#include "str.h"

// basically acts like a mutable string, where its size is bound by capacity
// also used in the case, we don't actually wish to create a new string from an existing one
// but modify in place
typedef struct String_Builder
{
    char* chars;
    uint32_t length;
    u64 capacity;
}String_Builder;


String_Builder* string_builder_create(const char* string, u64 string_size, u64 capacity)
{

}

String_Builder* string_builder_concat(const char* string, u64 string_size, u64 capacity)
{

}


#define STRING_BUILDER_CREATE(x, u)  string_builder_create(x, sizeof(x), u)






#endif

