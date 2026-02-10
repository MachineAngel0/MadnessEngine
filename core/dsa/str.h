#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>
#include "defines.h"

//TODO: refactor to ensure immutability, and ease of use with str_builder

//immutable string, not meant to be modified,
//all functions that need to make modifications will return you a new string, leaving the original untouched
//does not retain the null terminator
typedef struct String
{
    char* chars;
    u64 length;
    // we never retain the null terminated char, but it might be that there is one based on the function used
} String;

typedef struct String_Tokenizer
{
    String** strings;
    u64 number_of_strings;
} String_Tokenizer;

//TODO: idk of i really want to implement this
typedef enum DELIMITER_BEHAVIOR
{
    DELIMITER_ISOLATE, // this should be default behavior, gives the delimiter its own string
    DELIMITER_INCLUDE, // will be the last char of the entire string
    DELIMITER_REMOVE, // will not be included
} DELIMITER_BEHAVIOR;


//NOTE: do not call this, just use STRING_CREATE(string) unless you specifally need to pass in the size for some reason
String* string_create(const char* word, const u64 length);


String* string_create_internal(const String* s);

bool string_free(String* string);


//this gets created on the stack as a string literal, this also uses read only memory so it can crash if modified
#define STRING(string) ((String){.chars = (char*)(string), .length = sizeof(string)-1})
//will convert the string into the correct size, for some reason doesn't work after the string has been passed
#define STRING_CREATE(string) string_create(string, sizeof(string))
//create a string from an already existing char[]/char* that excludes the null terminated string
#define STRING_CREATE_FROM_BUFFER(string) string_create(string, strlen(string))


//UTILITY
void string_print(const String* str);


bool string_compare(const String* str1, const String* str2);


bool str_is_empty(const String* str);

String* string_duplicate(const String* str);

//creates a new string from the two strings
String* string_concat(const String* str1, const String* str2);


String* string_strip_whitespace(String* str);

/*C-STRING*/

const char* string_convert_to_c_string(const String* s);


/*STRING SLICE*/

String* string_slice_from(const String* s, const u64 slice_size);

String* string_slice_from_to(const String* s, const u64 slice_begin, const u64 slice_end);

String* string_strip_from_end(String* str, const char stop_character);


//creates a copy of the string/words passed in
String_Tokenizer* string_tokenize_delimiter(const String* s, const char delimiter);

String_Tokenizer* string_tokenize_delimiter_array(const String* s, const String* delimiter_array,
                                                  bool ignore_whitespace);

void string_tokenizer_print(const String_Tokenizer* str_tokens);
//returns copy of the strings
#define STRING_TOKENIZE(s) string_tokenize_delimiter(s, ' ')

void string_test(void);


#endif
