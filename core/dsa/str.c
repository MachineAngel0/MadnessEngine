#include "str.h"

#include "allocator_malloc.h"
#include "allocator.h"
#include "c_string.h"


//immutable string, not meant to be modified,
//all functions that need to make modifications will return you a new string, leaving the original untouched
//does not retain the null terminator


//NOTE: do not call this, just use STRING_CREATE(string) unless you specifally need to pass in the size for some reason
String* string_create_allocator(const char* word, const u64 length, Allocator* allocator)
{
    //creates a string without the null terminator
    String* str = allocator_alloc(allocator, sizeof(String));
    //memset(str, 0, sizeof(MString));
    str->length = length;
    //important to note that we use -1 to not include the null terminated string
    str->chars = allocator_alloc(allocator, sizeof(char) * str->length);
    // memset(str->chars, 0, sizeof(char) * str->length);
    memcpy(str->chars, word, sizeof(char) * str->length);

    return str;
}

String* string_create_allocator_heap(const char* word, const u64 length, Heap_Allocator* allocator)
{
    //creates a string without the null terminator
    String* str = allocator_heap_alloc(allocator, sizeof(String));
    //memset(str, 0, sizeof(MString));
    str->length = length;
    //important to note that we use -1 to not include the null terminated string
    str->chars = allocator_heap_alloc(allocator, sizeof(char) * str->length);
    memset(str->chars, 0, sizeof(char) * str->length);
    memcpy(str->chars, word, sizeof(char) * str->length);

    return str;
}

bool string_free_allocator_heap(String* string, Heap_Allocator* allocator)
{
    MASSERT(string);
    if (!string)
    {
        WARN("INVALID STRING CANT FREE");
        return false;
    }
    if (!string->chars)
    {
        WARN("INVALID CHAR* INSIDE STRING CANT FREE");
        return false;
    }

    allocator_heap_free(allocator, string->chars);
    allocator_heap_free(allocator, string);

    return true;
}


//UTILITY
void string_print(const String* str)
{
    MASSERT(str);
    printf("%.*s", (int)str->length, str->chars);
}

void string_println(const String* str)
{
    MASSERT(str);
    printf("%.*s\n", (int)str->length, str->chars);
}


bool string_compare(const String* str1, const String* str2)
{
    MASSERT(str1 != NULL);
    MASSERT(str2 != NULL);
    if (str1->length != str2->length) return false;

    for (uint32_t i = 0; i < str1->length; i++)
    {
        if (str1->chars[i] != str2->chars[i])
        {
            // DEBUG("STRING COMPARE: %c, %c", str1->chars[i], str2->chars[i]);
            return false;
        }
    }

    return true;
}


bool str_is_empty(const String* str)
{
    MASSERT(str);
    return str->length == 0;
}

String* string_duplicate(const String* str)
{
    //plus one cause we don't have a null string terminator
    const String* s = str;
    String* out_str = malloc(sizeof(String));
    //important to note that we use -1 to not include the null terminated string
    out_str->chars = (char*)malloc(sizeof(char) * s->length);
    memcpy(out_str->chars, s->chars, sizeof(char) * s->length);
    out_str->length = s->length;
    return out_str;
}

String* string_duplicate_alloc(const String* str, Allocator* allocator)
{
    //plus one cause we don't have a null string terminator
    const String* s = str;
    String* out_str = allocator_alloc(allocator, sizeof(String));
    //important to note that we use -1 to not include the null terminated string
    out_str->chars = (char*)allocator_alloc(allocator,sizeof(char) * s->length);
    memcpy(out_str->chars, s->chars, sizeof(char) * s->length);
    out_str->length = s->length;
    return out_str;
}
String* string_duplicate_heap(const String* str, Heap_Allocator* allocator)
{
    //plus one cause we don't have a null string terminator
    const String* s = str;
    String* out_str = allocator_heap_alloc(allocator, sizeof(String));
    //important to note that we use -1 to not include the null terminated string
    out_str->chars = (char*)allocator_heap_alloc(allocator, sizeof(char) * s->length);
    memcpy(out_str->chars, s->chars, sizeof(char) * s->length);
    out_str->length = s->length;
    return out_str;
}


String* string_concat_malloc(const String* str1, const String* str2)
{
    String* out_str = allocator_malloc(sizeof(String));
    const u64 combined_length = str1->length + str2->length;
    out_str->chars = (char*)allocator_malloc(sizeof(char) * combined_length);
    out_str->length = combined_length;

    memcpy(out_str->chars, str1->chars, sizeof(char) * str1->length);
    memcpy(out_str->chars + str1->length, str2->chars, sizeof(char) * str2->length);

    return out_str;
}

String* string_concat(const String* str1, const String* str2, Allocator* allocator)
{
    String* out_str = allocator_alloc(allocator, sizeof(String));
    const u64 combined_length = str1->length + str2->length;
    out_str->chars = (char*)allocator_alloc(allocator, sizeof(char) * combined_length);
    out_str->length = combined_length;

    memcpy(out_str->chars, str1->chars, sizeof(char) * str1->length);
    memcpy(out_str->chars + str1->length, str2->chars, sizeof(char) * str2->length);

    return out_str;
}

String* string_concat_heap(const String* str1, const String* str2, Heap_Allocator* allocator)
{
    String* out_str = allocator_heap_alloc(allocator, sizeof(String));
    const u64 combined_length = str1->length + str2->length;
    out_str->chars = allocator_heap_alloc(allocator, sizeof(char) * combined_length);
    out_str->length = combined_length;

    memcpy(out_str->chars, str1->chars, sizeof(char) * str1->length);
    memcpy(out_str->chars + str1->length, str2->chars, sizeof(char) * str2->length);

    return out_str;
}


String* string_strip_whitespace(const String* str)
{
    String* out_string = string_duplicate(str);

    //two pointer
    u64 i = 0;
    u64 j = 0; // iterates forward
    while (j < str->length)
    {
        if (str->chars[j] != ' ')
        {
            out_string->chars[i] = str->chars[j];
            i++;
        }
        j++;
    }
    //where ever i ends up is the new length of the string
    out_string->length = i;
    return out_string;
}

/*C-STRING*/

const char* string_to_c_string(const String* s)
{
    //when you need to convert it to a valid directory, this function is nice
    char* c_string = malloc(sizeof(char) * (s->length + 1)); // +1 for the null terminated string
    memcpy(c_string, s->chars, s->length);
    c_string[s->length] = '\0';
    return c_string;
}

const char* string_to_c_string_allocator(const String* s, Allocator* allocator)
{
    //when you need to convert it to a valid directory, this function is nice
    char* c_string = allocator_alloc(allocator, sizeof(char) * (s->length + 1)); // +1 for the null terminated string
    memcpy(c_string, s->chars, s->length);
    c_string[s->length] = '\0';
    return c_string;
}

const char* string_to_c_string_alloc_heap(const String* s, Heap_Allocator* allocator)
{
    //when you need to convert it to a valid directory, this function is nice
    char* c_string = allocator_heap_alloc(allocator, sizeof(char) * (s->length + 1)); // +1 for the null terminated string
    memcpy(c_string, s->chars, s->length);
    c_string[s->length] = '\0';
    return c_string;
}



bool string_compare_c_string_length(const String* str1, const char* c_str, size_t c_string_length)
{
    MASSERT(str1 != NULL);
    MASSERT(c_str != NULL);

    if (c_string_length != str1->length) return false;


    for (uint32_t i = 0; i < c_string_length; i++)
    {
        if (str1->chars[i] != c_str[i]) return false;
    }

    return true;
}

bool string_compare_c_string(const String* str1, const char* c_str)
{
    MASSERT(str1 != NULL);
    MASSERT(c_str != NULL);

    size_t c_string_length = strlen(c_str);

    return string_compare_c_string_length(str1, c_str, c_string_length);
}



String* string_from_int(s32 value, Allocator* allocator)
{
    String* out_string = allocator_alloc(allocator, sizeof(String));

    out_string->length = snprintf(NULL, 0, "%d", value);
    out_string->chars = allocator_alloc(allocator, out_string->length + 1);
    snprintf(out_string->chars, out_string->length + 1, "%d", value);

    return out_string;
}

String* string_from_int_heap_allocator(s32 value, Heap_Allocator* allocator)
{
    String* out_string = allocator_heap_alloc(allocator, sizeof(String));

    out_string->length = snprintf(NULL, 0, "%d", value);
    out_string->chars = allocator_heap_alloc(allocator, out_string->length + 1);
    snprintf(out_string->chars, out_string->length + 1, "%d", value);

    return out_string;
}

String* string_from_float(float value, Allocator* allocator)
{
    String* out_string = allocator_alloc(allocator, sizeof(String));

    out_string->length = snprintf(NULL, 0, "%.2f", value);
    out_string->chars = allocator_alloc(allocator, out_string->length + 1);
    snprintf(out_string->chars, out_string->length + 1, "%.2f", value);

    return out_string;
}


String* string_format(Allocator* allocator, const char* format, ...)
{

    String* out_string = allocator_alloc(allocator, sizeof(String));

    va_list arg_ptr;
    va_start(arg_ptr, format);
    out_string->length = vsnprintf(NULL, 0, format, arg_ptr);
    out_string->chars = allocator_alloc(allocator, out_string->length + 1);

    vsnprintf(out_string->chars, out_string->length + 1, format, arg_ptr);
    va_end(arg_ptr);


    return out_string;
}



/*STRING SLICE*/

String_Slice* string_slice_from(String* s, const u64 slice_size, Allocator* allocator)
{
    if (slice_size > s->length)
    {
        WARN("STRING SLICE FROM: Passed in slice_size greater than string length")
        return NULL;
    }

    String_Slice* string_slice = allocator_alloc(allocator, sizeof(String_Slice));
    string_slice->offset = 0;
    string_slice->length = 0;
    string_slice->original_string = s;


    return string_slice;
}

String_Slice* string_slice_from_to(String* s, u64 slice_begin, u64 slice_end, Allocator* allocator)
{
    MASSERT(s);

    if (slice_begin > slice_end)
    {
        WARN("STRING FROM: Passed in slice begin greater than slice end")
        return NULL;
    }
    if (slice_end > s->length)
    {
        WARN("STRING SLICE FROM TO : Passed in slice_end greater than string length")
        return NULL;
    }

    String_Slice* string_slice = allocator_alloc(allocator, sizeof(String_Slice));
    string_slice->original_string = s;
    string_slice->offset = slice_begin;
    string_slice->length = slice_end - slice_begin;

    return string_slice;
}

String_Slice* string_strip_from_end(String* str, char stop_character, Allocator* allocator)
{
    //mostly used for path string, so that the end value will be removed
    //includes the stop character in the final result

    u64 i = str->length;
    for (; i > 0; i--)
    {
        if (str->chars[i] == stop_character)
        {
            break;
        }
    }

    //where ever i ends up is the new length of the string
    return string_slice_from(str, i + 1, allocator);
}

void string_slice_print(const String_Slice* slice)
{
    printf("%.*s", (int)slice->length, slice->original_string->chars + slice->offset);
}


bool string_serialize(String* string, FILE* fptr)
{
    MASSERT(string);
    MASSERT(string->chars);
    MASSERT(fptr);

    fwrite(&string->length, sizeof(string->length), 1, fptr);
    fwrite(string->chars, string->length, 1, fptr);
    return true;
}

bool string_deserialize(String* string, FILE* fptr, Frame_Allocator* allocator)
{
    MASSERT(fptr);
    MASSERT(allocator);

    fread(&string->length, sizeof(string->length), 1, fptr);
    // Allocate extra space for null terminator
    string->chars = allocator_alloc(allocator, string->length + 1);
    fread(string->chars, string->length, 1, fptr);
    // Null-terminate the string
    string->chars[string->length] = '\0';

    return true;
}


bool string_deserialize_heap(String* string, FILE* fptr, Heap_Allocator* allocator)
{
    MASSERT(string);
    MASSERT(fptr);
    MASSERT(allocator);

    fread(&string->length, sizeof(string->length), 1, fptr);
    // Allocate extra space for null terminator
    string->chars = allocator_heap_alloc(allocator, string->length + 1);
    fread(string->chars, string->length, 1, fptr);
    // Null-terminate the string
    string->chars[string->length] = '\0';

    return true;
}

u32 string_hash_u32(const String string)
{
    return generate_hash_key_32bit((u8*)string.chars, string.length);
}

u64 string_hash_u64(const String string)
{
    return generate_hash_key_64bit((u8*)string.chars, string.length);
}


//returns copy of the strings
#define STRING_TOKENIZE(s) string_tokenize_delimiter(s, ' ')

void string_test(void)
{
    TEST_START("STRING");

    Allocator* string_allocator = malloc(sizeof(Allocator));
    void* memory_block = malloc(1024);
    allocator_init(string_allocator, memory_block, 1024, "string test");

    String stack_string = STRING("I WAS BORN ON THE STACK, FREED BY IT");
    string_print(&stack_string);


    String* test1 = STRING_CREATE("testing something", string_allocator);
    string_print(test1);
    String* test2 = string_create_allocator("testing something", sizeof("testing something"), string_allocator);
    string_print(test1);
    string_print(test2);

    allocator_clear(string_allocator);

    String* str1 = string_create_allocator("hello string", sizeof("hello string"), string_allocator);
    string_print(str1);

    //String* test_string_two = string_create_no_length("hello string");

    String* str3 = string_create_allocator("string, hello", sizeof("string, hello"), string_allocator);
    string_print(str3);

    INFO("String Compare str1 and str1: %s", string_compare(str1, str1) ? "true" : "false");
    INFO("String Compare str1 and str3: %s", string_compare(str1, str3) ? "true" : "false");


    String* str4 = string_duplicate(str1);
    INFO("STRING 4");
    string_print(str4);

    allocator_clear(string_allocator);

    /***C_STRING STUFF***/
    char char_buffer_no_null[] = "lol";

    String* str_from_c_buffer = STRING_CREATE_FROM_BUFFER_ALLOCATOR(char_buffer_no_null, string_allocator);
    TEST_DEBUG(str_from_c_buffer->length == 3);
    string_print(str_from_c_buffer);

    char char_buffer_with_null[] = "lol\0";

    String* str_from_c_buffer_with_null = STRING_CREATE_FROM_BUFFER_ALLOCATOR(char_buffer_with_null, string_allocator);
    TEST_DEBUG(str_from_c_buffer_with_null->length == 3);
    string_print(str_from_c_buffer_with_null);


    const char* string_before_c = "String_convert_to_C_String";
    String* string_created_from_c_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(string_before_c, string_allocator);
    const char* c_string = string_to_c_string(string_created_from_c_string);
    printf("%s\n", c_string);
    TEST_DEBUG((strcmp(string_before_c, c_string) == 0));


    /***STRING SLICE***/

    String* slice_test = STRING_CREATE("Creating a String Slice", string_allocator);
    string_print(slice_test);
    String_Slice* slice4 = string_slice_from(slice_test, 8, string_allocator);
    string_slice_print(slice4);
    String_Slice* slice18_23 = string_slice_from_to(slice_test, 18, 23, string_allocator);
    string_slice_print(slice18_23);





    const String* str_concat1 = STRING_CREATE("First ", string_allocator);
    const String* str_concat2 = STRING_CREATE("Second", string_allocator);
    String* str_concat_final = string_concat(str_concat1, str_concat2, string_allocator);
    string_print(str_concat_final);


    String* with_white_space = STRING_CREATE("I HAVE A LOT OF WHITE SPACE", string_allocator);
    String* without_white_space = string_strip_whitespace(with_white_space);
    string_print(with_white_space);
    string_print(without_white_space);

    free(memory_block);
    free(string_allocator);

    TEST_REPORT("STRING");
}
