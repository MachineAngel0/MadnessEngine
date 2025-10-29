

#ifndef STRINGS_H
#define STRINGS_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct String
{
    char* chars;
    uint32_t length;
} String;

String* string_create(const char* word, uint32_t length);
String* string_create_no_length(char* word);
bool string_free(String* string);



//UTILITY
void string_print(String* str);


bool string_compare(const String* str1, const String* str2);

//add the contents from str2 at the end of str1
void string_append(String* str1, String* str2);

bool string_is_empty(String* str);

//TODO: TEST
String* string_duplicate(String* str);

//TODO:
// char* string_tokenize_single(char* str, char* delimiter)
// char* string_tokenize(char* str, char* delimiter)


void string_test();


#endif //STRINGS_H
