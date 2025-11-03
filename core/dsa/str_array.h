//
#ifndef STRINGS_ARRAY_H
#define STRINGS_ARRAY_H

#include "defines.h"
#include "logger.h"
#include "str.h"

//the needs of a string are very specific,
//since they can be of varying size in the array
//so im giving it, its own array type

typedef struct String_Array
{
    char** data;
    u64* str_length;

    u64 count; // how many strings we gave
    u64 capacity; // the max amount of strings we are going to be allowed to store

    //TODO: allocator for the strings, for now just dynamically allocate
} String_Array;


String_Array* string_array_create(const u64 capacity)
{
    String_Array* s = (String_Array *) malloc(sizeof(String_Array));
    s->capacity = capacity;
    s->count = 0;
    s->data = malloc(sizeof(char *) * s->capacity);
    memset(s->data, 0, sizeof(char *) * s->capacity);
    s->str_length = malloc(sizeof(u64) * s->capacity);
    memset(s->str_length, 0, sizeof(u64 *) * s->capacity);


    return s;
};

void string_array_free(String_Array* s_arr)
{
    free(s_arr->data);
    free(s_arr->str_length);
    free(s_arr);
};

void string_array_resize(String_Array* s_arr, const u64 new_capacity)
{
    s_arr->data = realloc(s_arr->data, sizeof(char *) * new_capacity);
    s_arr->str_length = realloc(s_arr->str_length, sizeof(u64) * new_capacity);

    s_arr->capacity = new_capacity;
};

void string_array_reserve(String_Array* s, const u64 new_capacity)
{
    string_array_resize(s, new_capacity);
};


void string_array_emplace(String_Array* s_arr, const char* string, const u64 string_size)
{
    if (s_arr->count >= s_arr->capacity)
    {
        WARN("STRING ARR EMPLACE: COUNT EXCEEDS CAPACITY");
        return;

        //TODO: we can debug wrap if we want
        // INFO("STRING ARR EMPLACE: COUNT EXCEEDS CAPACITY, RESIZING");
        // string_array_resize(s_arr, s_arr->capacity * 2);
    }

    char* str_copy = malloc(string_size);
    memcpy(str_copy, string, string_size);
    s_arr->data[s_arr->count] = str_copy;
    s_arr->str_length[s_arr->count] = string_size;
    s_arr->count++;
};


void string_array_print_debug(const String_Array* s_arr)
{
    if (s_arr->count <= 0)
    {
        WARN("TRYING TO ITERATE OVER A 0 COUNT STRING ARRAY");
        return;
    }

    for (u64 i = 0; i < s_arr->count; i++)
    {
        DEBUG("[STRING ARRAY]: %s", s_arr->data[i]);
    }
};


#define STRING_ARRAY_EMPLACE(s, str) string_array_emplace(s, str, sizeof(str))


// #define STRING_BUILDER_CREATE(x, u)  string_builder_create(x, sizeof(x), u)

bool string_array_unit_test()
{
    String_Array* s = string_array_create(5);

    STRING_ARRAY_EMPLACE(s, "lalala");
    STRING_ARRAY_EMPLACE(s, "gibbirish");
    STRING_ARRAY_EMPLACE(s, "other");
    STRING_ARRAY_EMPLACE(s, "a thing");
    STRING_ARRAY_EMPLACE(s, "strange happenings");
    //this should trigger a resize
    STRING_ARRAY_EMPLACE(s, "resize");

    string_array_print_debug(s);

    string_array_free(s);

    return true;
}


#endif
