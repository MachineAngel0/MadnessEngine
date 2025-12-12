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
    // char* chars;
    // u64 length;
    String string;
    //TODO: this absolutely should be using capacity, and also in not freeing the old data anywhere, also ideally use a pool arena
    // not using capacity rn, just cause we can just use an arena
    // u64 capacity;
}String_Builder;

//if you do not need to heap allocate, then do not heap allocate (String_Builder = {0})
String_Builder* string_builder_create()
{
    String_Builder* builder = malloc(sizeof(String_Builder));
    // builder->string = string_create_fr om_null_terminated(string, string_size);
    memset(&builder->string, 0, sizeof(String));
    return builder;
}
void string_builder_free(String_Builder* builder)
{
     free(builder);
}


void string_build(Arena* arena, String_Builder* builder, const char* string, const u64 length)
{
    u64 new_length = length + builder->string.length;
    char* data = (char*)arena_alloc(arena, new_length);

    //write old data into the buffer
    memcpy(data ,builder->string.chars, builder->string.length);
    //then write new data into the buffer
    memcpy(&data[builder->string.length], string, length);

    //replace the old string, and set the new size
    builder->string.chars = data;
    builder->string.length = new_length;


}


void string_build_string(Arena* arena, String_Builder* builder, String* str)
{
    u64 new_length = str->length + builder->string.length;
    char* data = (char*)arena_alloc(arena, new_length);

    //write old data into the buffer
    memcpy(data ,builder->string.chars, builder->string.length);
    //then write new data into the buffer
    memcpy(&data[builder->string.length], str->chars, str->length);

    //replace the old string, and set the new size
    builder->string.chars = data;
    builder->string.length = new_length;
}

#define STRING_BUILD(arena, builder, string) string_build(arena, builder, string, sizeof(string)-1)

u64 string_get_length(const String_Builder* builder)
{
    return builder->string.length;
}

void string_builder_print(String_Builder* builder)
{
    string_print(&builder->string);
}

void string_builder_append_str(String_Builder* builder, String* s)
{
    // if ((builder->string->length + s->length) > builder->capacity){
        // WARN("STRING BUILDER APPEND STRING: capacity overflow");
    // }
}

void string_builder_append_word(String_Builder* builder, char* word, u64 word_size)
{

}





void string_builder_test()
{
    TEST_START("STRING BUILDER");

    String_Builder builder = {0};

    Arena* arena_for_strings = arena_init_malloc(KB(1));

    STRING_BUILD(arena_for_strings, &builder, "ANOTHER WORD, ");
    string_builder_print(&builder);
    STRING_BUILD(arena_for_strings, &builder, "A SECOND WORD, ");

    // string_builder_print(&builder);
    string_builder_print(&builder);

    string_build_string(arena_for_strings, &builder, &(STRING("OH MY, A THIRD WORD")));
    string_builder_print(&builder);

    TEST_REPORT("STRING BUILDER");

    arena_free(arena_for_strings);

}



#endif

