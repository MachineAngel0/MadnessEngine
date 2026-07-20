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

    Allocator* allocator;
    Heap_Allocator* heap_allocator;
} String_Builder;





//if you do not need to heap allocate, then do not heap allocate (String_Builder = {0})
String_Builder* string_builder_create(const u64 capacity, Allocator* allocator);

void string_builder_free(String_Builder* builder);
/*
void string_build(Arena* arena, String_Builder* builder, const char* string, const u64 length)
{
    UNIMPLEMENTED();
}
*/

void string_builder_print(String_Builder* builder);

void string_builder_append_string(String_Builder* str_builder, String* s);
void string_builder_append_string_format(String_Builder* str_builder, const char* format, ...);

void string_builder_append_builder(String_Builder* src, String_Builder* copy_str);


void string_builder_append_c_string(String_Builder* str_builder, const char* word);
void string_builder_append_char(String_Builder* str_builder, char character);

void string_builder_decrement(String_Builder* str_builder);


bool string_builder_strip_path(String_Builder* builder);
bool string_builder_strip_extension(String_Builder* builder);



String* string_builder_to_string(const String_Builder* builder);
String string_builder_to_string_non_pointer(const String_Builder* builder);
char* string_builder_to_c_string(const String_Builder* builder);

void string_builder_clear(String_Builder* builder);


//NOTE: this assume all values are digits
u64 string_builder_to_number(const String_Builder* builder);

bool string_builder_compare_with_char(String_Builder* builder, const char* word, u64 word_size);
#define STRING_BUILDER_COMPARE_WITH_CHAR(builder, string) string_builder_compare_with_char(builder, string, sizeof(string)-1)


void string_builder_test(void);


#endif //STRINGS_BUILDER_H
