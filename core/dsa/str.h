#ifndef STRINGS_H
#define STRINGS_H

#include "defines.h"

//TODO: refactor to ensure immutability, and ease of use with str_builder

//immutable string, not meant to be modified,
//all functions that need to make modifications will return you a new string, leaving the original untouched
//does not retain the null terminator, you can call string_to_c_string to get one with a null terminator
typedef struct String
{
    u64 length;
    char* chars;
} String;

typedef struct String_Tokenizer
{
    u64 number_of_strings;
    String** strings;
} String_Tokenizer;

//TODO: idk of i really want to implement this
typedef enum DELIMITER_BEHAVIOR
{
    DELIMITER_ISOLATE, // this should be default behavior, gives the delimiter its own string
    DELIMITER_INCLUDE, // will be the last char of the entire string
    DELIMITER_REMOVE, // will not be included
} DELIMITER_BEHAVIOR;


//NOTE: do not call this, just use STRING_CREATE(string) unless you specifically need to pass in the size for some reason
String* string_create(const char* word, const u64 length);
String* string_create_allocator(const char* word, const u64 length, Allocator* allocator);


String* string_create_internal(const String* s);
bool string_free(String* string);


//this gets created on the stack as a string literal, this also uses read only memory so it can crash if modified
#define STRING(string) ((String){.chars = (char*)(string), .length = sizeof(string)-1})
//will convert the string into the correct size, for some reason doesn't work after the string has been passed as a param
#define STRING_CREATE(string) string_create(string, sizeof(string))
//create a string from an already existing char[]/char* that excludes the null terminated string
#define STRING_CREATE_FROM_BUFFER(string) string_create(string, strlen(string))


//UTILITY
void string_print(const String* str);
void string_println(const String* str);


bool string_compare(const String* str1, const String* str2);



bool str_is_empty(const String* str);

String* string_duplicate(const String* str);

//creates a new string from the two strings
String* string_concat(const String* str1, const String* str2, Allocator_Interface allocator_interface);


String* string_strip_whitespace(const String* str);

/*C-STRING*/

const char* string_to_c_string(const String* s);
bool string_compare_c_string(const String* str1, const char* c_str);


/*STRING SLICE*/

String* string_slice_from(const String* s, const u64 slice_size);

String* string_slice_from_to(const String* s, const u64 slice_begin, const u64 slice_end);

String* string_strip_from_end(const String* str, const char stop_character);


//creates a copy of the string/words passed in
String_Tokenizer* string_tokenize_delimiter(const String* s, const char delimiter);

String_Tokenizer* string_tokenize_delimiter_array(const String* s, const String* delimiter_array,
                                                  bool ignore_whitespace);

void string_tokenizer_print(const String_Tokenizer* str_tokens);
//returns copy of the strings
#define STRING_TOKENIZE(s) string_tokenize_delimiter(s, ' ')


u32 string_hash_u32(String string);
u64 string_hash_u64(String string);


void string_test(void);




#endif
