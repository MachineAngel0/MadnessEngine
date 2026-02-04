//
#ifndef STRINGS_BUILDER_H
#define STRINGS_BUILDER_H

#include "str.h"
#include "defines.h"


// basically acts like a mutable string, where its size is bound by capacity or the backing buffer
// also used in the case, we don't actually wish to create a new string from an existing one
// but modify in place
typedef struct String_Builder
{
    char* str;
    u64 current_length;
    //TODO: add an arena or pool just for strings
    u64 capacity; // here if we want to realloc the data
} String_Builder;

//if you do not need to heap allocate, then do not heap allocate (String_Builder = {0})
String_Builder* string_builder_create(const u64 capacity)
{
    assert(capacity > 0);

    //TODO: replace malloc
    String_Builder* builder = malloc(sizeof(String_Builder));
    memset(builder, 0, sizeof(String_Builder));
    builder->str = malloc(capacity * sizeof(char));

    builder->current_length = 0;
    builder->capacity = capacity;
    // builder->string = string_create_fr om_null_terminated(string, string_size);
    return builder;
}

void string_builder_free(String_Builder* builder)
{
    assert(builder);
    assert(builder->str);

    free(builder->str);
    free(builder);

    builder = NULL;
}

/*
void string_build(Arena* arena, String_Builder* builder, const char* string, const u64 length)
{
    UNIMPLEMENTED();
}
*/


void string_builder_print(String_Builder* builder)
{
    printf("%.*s", (int)builder->current_length, builder->str);
}

void string_builder_append_string(String_Builder* str_builder, String* s)
{
    //check if we have enough space
    if (str_builder->current_length + s->length > str_builder->capacity)
    {
        u64 length_requested = str_builder->current_length + s->length;
        u64 new_capacity = str_builder->capacity * 2;
        if (new_capacity < length_requested)
        {
            str_builder->str = realloc(str_builder->str, length_requested);
            str_builder->capacity = length_requested;
        }
        else
        {
            str_builder->str = realloc(str_builder->str, new_capacity);
            str_builder->capacity = new_capacity;
        }
    };

    //copy the word into the string
    memcpy(str_builder->str + str_builder->current_length, s->chars, s->length);
    str_builder->current_length += s->length;
}

void string_builder_append_char(String_Builder* str_builder, const char* word, const u64 word_size)
{
    //check if we have enough space
    if (str_builder->current_length + word_size > str_builder->capacity)
    {
        u64 length_requested = str_builder->current_length + word_size;
        u64 new_capacity = str_builder->capacity * 2;
        if (new_capacity < length_requested)
        {
            str_builder->str = realloc(str_builder->str, length_requested);
            str_builder->capacity = length_requested;
        }
        else
        {
            str_builder->str = realloc(str_builder->str, new_capacity);
            str_builder->capacity = new_capacity;
        }
    };

    //copy the word into the string
    memcpy(str_builder->str + str_builder->current_length, word, word_size);
    str_builder->current_length += word_size;
}

String* string_builder_to_string(const String_Builder* builder)
{
    return string_create(builder->str, builder->current_length);
}
String* string_builder_to_c_string(const String_Builder* builder)
{
    UNIMPLEMENTED();
    return NULL;
}

#define STRING_BUILDER_APPEND_CHAR(builder, string) string_builder_append_char(builder, string, sizeof(string)-1)


void string_builder_test()
{
    TEST_START("STRING BUILDER");

    const char* HI = "HI";

    String_Builder* str1 = string_builder_create(100);

    String* other_str1 = STRING_CREATE("HI");

    string_builder_append_string(str1, other_str1);

    for (size_t i = 0; i < strlen(HI); i++)
    {
        TEST_DEBUG(other_str1->chars[i] == HI[i]);
    }



    string_builder_free(str1);

    TEST_DEBUG(str1 == NULL);



    TEST_END("STRING BUILDER");
}


#endif
