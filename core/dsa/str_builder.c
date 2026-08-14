#include "str_builder.h"

#include <assert.h>


//if you do not need to heap allocate, then do not heap allocate (String_Builder = {0})
String_Builder* string_builder_create(const u64 capacity, Allocator* allocator)
{
    assert(capacity > 0);

    String_Builder* builder = allocator_alloc(allocator, sizeof(String_Builder));
    builder->str = allocator_alloc(allocator, capacity * sizeof(char));
    builder->current_length = 0;
    builder->capacity = capacity;

    builder->allocator = allocator;
    builder->heap_allocator = NULL;

    return builder;
}

String_Builder* string_builder_create_heap(const u64 capacity, Heap_Allocator* allocator)
{
    assert(capacity > 0);

    String_Builder* builder = allocator_heap_alloc(allocator, sizeof(String_Builder));
    builder->str = allocator_heap_alloc(allocator, capacity * sizeof(char));
    builder->current_length = 0;
    builder->capacity = capacity;

    builder->allocator = NULL;
    builder->heap_allocator = allocator;

    return builder;
}

void string_builder_free(String_Builder* builder)
{
    assert(builder);
    assert(builder->str);

    if (builder->heap_allocator)
    {

        allocator_heap_free(builder->heap_allocator, builder->str);
        allocator_heap_free(builder->heap_allocator, builder);
    }


}

/*
void string_build(Arena* arena, String_Builder* builder, const char* string, const u64 length)
{
    UNIMPLEMENTED();
}
*/


void string_builder_print(String_Builder* builder)
{
    printf("%.*s\n", (int)builder->current_length, builder->str);
}



void string_builder_append_string(String_Builder* str_builder, String* s)
{
    //check if we have enough space
    if (str_builder->current_length + s->length > str_builder->capacity)
    {
        u64 length_requested = str_builder->current_length + s->length;
        u64 new_capacity = str_builder->capacity * 2;
        //if we are still less than the requested length, then allocate to the size of the requested length
        // otherwise just double
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


void string_builder_append_builder(String_Builder* src, String_Builder* copy_str)
{
    //check if we have enough space
    if (src->current_length + copy_str->current_length > src->capacity)
    {
        u64 length_requested = src->current_length + copy_str->current_length;
        u64 new_capacity = src->capacity * 2;
        //if we are still less than the requested length, then allocate to the size of the requested length
        // otherwise just double
        if (new_capacity < length_requested)
        {
            src->str = realloc(src->str, length_requested);
            src->capacity = length_requested;
        }
        else
        {
            src->str = realloc(src->str, new_capacity);
            src->capacity = new_capacity;
        }
    };

    //copy the word into the string
    memcpy(src->str + src->current_length, copy_str->str, copy_str->current_length);
    src->current_length += copy_str->current_length;


}

void string_builder_append_c_string(String_Builder* str_builder, const char* word)
{
    //check if we have enough space
    u64 word_size = strlen(word);
    string_builder_append_c_string_length(str_builder, word, word_size);

}

void string_builder_append_c_string_length(String_Builder* str_builder, const char* word, const u64 length)
{
    //check if we have enough space
    if (str_builder->current_length + length > str_builder->capacity)
    {
        u64 length_requested = str_builder->current_length + length;
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
    memcpy(str_builder->str + str_builder->current_length, word, length);
    str_builder->current_length += length;
}

void string_builder_append_char(String_Builder* str_builder, const char character)
{
    //check if we have enough space
    if (str_builder->current_length + 1 > str_builder->capacity)
    {
        u64 length_requested = str_builder->current_length + 1;
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
    }

    //copy the word into the string
    memcpy(str_builder->str + str_builder->current_length, &character, 1);
    str_builder->current_length += 1;
}

void string_builder_append_u64(String_Builder* str_builder, const u64 val, Allocator* allocator)
{

    u32 val_length = snprintf(NULL, 0, "%llu", val);
    char* str = allocator_alloc(allocator, val_length + 1);
    snprintf(str, val_length + 1, "%llu", val);

    string_builder_append_c_string_length(str_builder, str, val_length);
}


void string_builder_decrement(String_Builder* str_builder)
{
    //check for valid pointer and that we can decrement a valid length of string
    MASSERT(str_builder)
    if (str_builder->current_length <= 0) return;

    str_builder->current_length--;
}


bool string_builder_strip_path_to_base_name(String_Builder* builder)
{
    //assuming something like this ../path/thing/other_thing/name.ext
    // we will make the string builder contain only name.ext

    size_t starting_length = builder->current_length;
    size_t new_length = builder->current_length;
    for (; new_length > 0; new_length--)
    {
        if (builder->str[new_length] == '/')
        {
            memcpy(builder->str, builder->str + new_length+1, starting_length - new_length+1);
            builder->current_length = starting_length - new_length;
            return true;
        }
    }

    //basically was not a valid path string
    MASSERT_MSG(false, "C_STRING_PATH_STRIP: PATH STRING DOES NOT CONTAIN /");
    return false;
}

bool string_builder_strip_extension(String_Builder* builder)
{
    MASSERT(builder);

    u64 new_index = builder->current_length;
    for (; new_index > 0; new_index--)
    {
        if (builder->str[new_index] == '.')
        {
            //once more to move past the extension
            builder->current_length = new_index;
            return true;
        }
    }

    //basically was not a valid path string
    MASSERT_MSG_FALSE("string_builder_strip_extension: PATH STRING DOES NOT CONTAIN /");
    return false;
}

bool string_builder_strip_path_from_beginning(String_Builder* builder)
{
    //assuming something like this ..base/path/thing/other_thing/name.ext
    //it will remove base, and incrementally moves forward


    size_t starting_length = builder->current_length;

    for (u32 i = 0; i < starting_length; i++)
    {
        if (builder->str[i] == '/')
        {
            //we add one to remove the '/'
            u64 new_length = starting_length - (i + 1);
            memcpy(builder->str, builder->str + (i + 1), new_length);
            builder->current_length = new_length;
            return true;
        }

    }


    //basically was not a valid path string
    MASSERT_MSG(false, "C_STRING_PATH_STRIP: PATH STRING DOES NOT CONTAIN /");
    return false;


}

bool string_builder_strip_path_from_end(String_Builder* builder)
{
    size_t starting_length = builder->current_length;

    for (u32 i = starting_length; i > 0; i--)
    {
        if (builder->str[i] == '/')
        {
            //we add one to remove the '/'
            builder->current_length = i;
            return true;
        }

    }


    //basically was not a valid path string
    MASSERT_MSG(false, "C_STRING_PATH_STRIP: PATH STRING DOES NOT CONTAIN /");
    return false;



}

String_Builder* string_builder_duplicate(const String_Builder* builder, Allocator* allocator)
{

    String_Builder* out_builder = string_builder_create(builder->capacity, allocator);
    memcpy(out_builder->str, builder->str, builder->current_length);


    return out_builder;
}

String* string_builder_to_string(const String_Builder* builder)
{
    return string_create(builder->str, builder->current_length);
}

String* string_builder_to_string_allocator(const String_Builder* builder, Allocator* allocator)
{
    return string_create_allocator(builder->str, builder->current_length, allocator);

}

String* string_builder_to_string_heap(const String_Builder* builder, Heap_Allocator* allocator)
{
    return string_create_allocator_heap(builder->str, builder->current_length, allocator);
}

String string_builder_to_string_non_pointer(const String_Builder* builder)
{
    return (String){.chars = builder->str, .length = builder->current_length};
}

char* string_builder_to_c_string(const String_Builder* builder)
{
    char* out_string = malloc(builder->current_length + 1);
    out_string = memcpy(out_string, builder->str, builder->current_length);
    out_string[builder->current_length] = '\0';
    return out_string;
}


u64 string_builder_to_number(const String_Builder* builder)
{
    u64 out_value = 0;
    for (u64 i = 0; i < builder->current_length; i++)
    {
        u64 val = builder->str[i] - '0';
        // ex: 10 + 3 = 103 (Wrong), 10 * 10 = 100 + 3 = 103 (Correct)
        out_value = out_value * 10 + val;
    }

    return out_value;
}

void string_builder_clear(String_Builder* builder)
{
    builder->current_length = 0;
}

bool string_builder_compare_with_char(String_Builder* builder, const char* word, u64 word_size)
{
    if (builder->current_length != word_size)
    {
        return false;
    }

    for (u64 i = 0; i < word_size; i++)
    {
        if (builder->str[i] != word[i])
        {
            return false;
        }
    }

    return true;
}

u64 string_builder_hash_u64(const String_Builder* builder)
{
    return generate_hash_key_64bit((u8*)builder->str, builder->current_length);
}

#define STRING_BUILDER_APPEND_CHAR(builder, string) string_builder_append_c_string(builder, string, sizeof(string)-1)


void string_builder_test(void)
{
    TEST_START(STRING BUILDER);

    Allocator allocator;
    u64 mem_size = MB(1);
    void* backing_memory = malloc(mem_size);
    allocator_init(&allocator, backing_memory, mem_size);

    const char* HI = "HI";

    String_Builder* str1 = string_builder_create(100, &allocator);

    String* other_str1 = STRING_CREATE("HI");

    string_builder_append_string(str1, other_str1);

    for (size_t i = 0; i < strlen(HI); i++)
    {
        TEST_DEBUG(other_str1->chars[i] == HI[i]);
    }


    string_builder_free(str1);

    TEST_DEBUG(str1 == NULL);


    TEST_END(STRING BUILDER);
}
