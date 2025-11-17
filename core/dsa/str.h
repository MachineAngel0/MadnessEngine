#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


//TODO: refactor to ensure immutability, and ease of use with str_builder

//immutable string, not meant to be modified
typedef struct String
{
    char* chars;
    u64 length;
} String;

//NOTE: do not call this, just use STRING_CREATE(string) unless you specifally need to pass in the size for some reason
String* string_create_from_null_terminated(const char* word, const u64 length)
{
    String* str = malloc(sizeof(String));
    //memset(str, 0, sizeof(MString));

    //important to note that we use -1 to not include the null terminated string
    str->chars = (char *) malloc(sizeof(char) * length - 1);
    memset(str->chars, 0, sizeof(char) * length - 1);

    str->length = length - 1;


    memcpy(str->chars, word, sizeof(char) * length-1);
    // for (uint32_t i = 0; i < str->length; i++)
    // {
    //     str->chars[i] = word[i];
    // }

    return str;
};




String* string_create_without_null_terminated(const char* word, const u64 length)
{
    String* str = malloc(sizeof(String));
    //memset(str, 0, sizeof(MString));

    //important to note that we use -1 to not include the null terminated string
    str->chars = (char *) malloc(sizeof(char) * length);
    memset(str->chars, 0, sizeof(char) * length);

    str->length = length;


    memcpy(str->chars, word, sizeof(char) * length);
    // for (uint32_t i = 0; i < str->length; i++)
    // {
    //     str->chars[i] = word[i];
    // }

    return str;
};


String* string_create_internal(const String* s)
{
    String* out_str = malloc(sizeof(String));
    //memset(str, 0, sizeof(MString));

    //important to note that we use -1 to not include the null terminated string
    out_str->chars = (char *) malloc(sizeof(char) * s->length);
    memcpy(out_str->chars, s->chars, sizeof(char) * s->length);

    out_str->length = s->length;


    return out_str;
};

bool string_free(String* string)
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

    free(string->chars);
    //if (string->chars != NULL){ return false;}
    free(string);
    //if (string != NULL){ return false;}

    return true;
}


//this gets created on the stack as a string literal, this also uses read only memory so it can crash if modified
#define STRING(string) ((String){.chars = (char*)(string), .length = sizeof(string)-1})
//will convert the string into the correct size, for some reason doesn't work after the string has been passed
#define STRING_CREATE(string) string_create_from_null_terminated(string, sizeof(string))
#define STRING_CREATE_FROM_BUFFER(string) string_create_without_null_terminated(string, sizeof(string))


//UTILITY
void string_print(const String* str)
{
    MASSERT(str);

    for (uint32_t i = 0; i < str->length; i++)
    {
        printf("%c", str->chars[i]);
    }
    printf("\n");
}


bool string_compare(const String* str1, const String* str2)
{
    MASSERT(str1 != NULL);
    MASSERT(str2 != NULL);
    if (str1->length != str2->length) return false;

    for (uint32_t i = 0; i < str1->length; i++)
    {
        if (str1->chars[i] != str2->chars[i]) return false;
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
    return string_create_internal(str);
}

//creates a new string from the two strings
String* string_concat(const String* str1, const String* str2)
{
    String* out_str = malloc(sizeof(String));
    u64 combined_length = str1->length + str2->length;
    out_str->chars = (char *) malloc(sizeof(char) * combined_length);
    memset(out_str->chars, 0, sizeof(char) * combined_length);

    out_str->length = combined_length;

    memcpy(out_str->chars, str1->chars, sizeof(char) * str1->length);
    memcpy(&out_str->chars[str1->length], str2->chars, sizeof(char) * str2->length);

    return out_str;
}

String* string_strip_whitespace(String* str)
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


/*STRING SLICE*/

String* string_slice_from(const String* s, const u64 slice_size)
{
    if (slice_size > s->length)
    {
        WARN("STRING SLICE FROM: Passed in slice_size greater than string length")
        return NULL;
    }

    return string_create_without_null_terminated(s->chars, slice_size);
}

String* string_slice_from_to(const String* s, const u64 slice_begin, const u64 slice_end)
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

    String* new_string_str = malloc(sizeof(String));
    const u64 new_length = slice_end - slice_begin;

    new_string_str->chars = (char *) malloc(sizeof(char) * new_length);
    memset(new_string_str->chars, 0, sizeof(char) * new_length);

    new_string_str->length = new_length;

    for (u64 i = 0; i < new_length; i++)
    {
        //copy from the passed in, to the new string
        new_string_str->chars[i] = s->chars[slice_begin + i];
    }

    return new_string_str;
}


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

//creates a copy of the string/words passed in
String_Tokenizer* string_tokenize_delimiter(const String* s, const char delimiter)
{
    String_Tokenizer* str_tokens = malloc(sizeof(String_Tokenizer));
    //theoretically the longest it can be
    str_tokens->strings = malloc(sizeof(String *) * s->length);
    str_tokens->number_of_strings = 0;


    u64 prev_index = 0;
    u64 index = 0;
    while (index < s->length)
    {


        if (s->chars[index] == delimiter)
        {
            String* temp = string_slice_from_to(s, prev_index, index + 1);
            str_tokens->strings[str_tokens->number_of_strings] = temp;
            str_tokens->number_of_strings++;
            prev_index = index + 1;
            index++;
        }
        index++;
    }
    //in this implementation I will be returning the last string
    String* temp = string_slice_from_to(s, prev_index, index);
    str_tokens->strings[str_tokens->number_of_strings] = temp;
    str_tokens->number_of_strings++;
    return str_tokens;
}

String_Tokenizer* string_tokenize_delimiter_array(const String* s, const String* delimiter_array, bool ignore_whitespace)
{
    //so the behavior should be something like this: delimeter = "<>" before<token> ->  before,<, token, >

    MASSERT(s);
    MASSERT(delimiter_array);

    String_Tokenizer* str_tokens = malloc(sizeof(String_Tokenizer));
    //theoretically the longest it can be
    str_tokens->strings = malloc(sizeof(String *) * s->length);
    str_tokens->number_of_strings = 0;


    u64 prev_index = 0;
    u64 index = 0;
    while (index < s->length)
    {
        if (ignore_whitespace)
        {
            if (s->chars[index] == ' ')
            {
                prev_index = index + 1;
                index++;
                continue;
            }
        }

        for (int i = 0; i < delimiter_array->length; i++)
        {

            if (s->chars[index] == delimiter_array->chars[i])
            {



                //take everything before the delimiter,
                if (index - prev_index > 0){
                    String* temp = string_slice_from_to(s, prev_index, index);
                    str_tokens->strings[str_tokens->number_of_strings] = temp;
                    str_tokens->number_of_strings++;
                }

                //we want to isolate the delimiter
                String* delimiter = string_slice_from_to(s, index, index+1);
                str_tokens->strings[str_tokens->number_of_strings] = delimiter;
                str_tokens->number_of_strings++;


                prev_index = index + 1;
                // index++;
                break;
            }
        }

        index++;
    }
    //in this implementation I will be returning the last string
    String* temp = string_slice_from_to(s, prev_index, index);
    str_tokens->strings[str_tokens->number_of_strings] = temp;
    str_tokens->number_of_strings++;
    return str_tokens;
}



//returns copy of the strings
#define STRING_TOKENIZE(s) string_tokenize_delimiter(s, ' ')

void string_test()
{
    TEST_START("STRING");

    String stack_string = STRING("I WAS BORN ON THE STACK, FREED BY IT");
    string_print(&stack_string);



    String* test1 = STRING_CREATE("testing something");
    string_print(test1);
    String* test2 = string_create_from_null_terminated("testing something", sizeof("testing something"));
    string_print(test1);
    string_print(test2);

    string_free(test1);
    string_free(test2);

    String* str1 = string_create_from_null_terminated("hello string", sizeof("hello string"));
    string_print(str1);

    //String* test_string_two = string_create_no_length("hello string");

    String* str3 = string_create_from_null_terminated("string, hello", sizeof("string, hello"));
    string_print(str3);

    INFO("String Compare str1 and str1: %s", string_compare(str1, str1) ? "true" : "false");
    INFO("String Compare str1 and str3: %s", string_compare(str1, str3) ? "true" : "false");


    String* str4 = string_duplicate(str1);
    INFO("STRING 4");
    string_print(str4);

    string_free(str1);
    // string_free(str2);

    /***C_STRING STUFF***/
    char char_buffer[3];
    char_buffer[0] = 'l';
    char_buffer[1] = 'o';
    char_buffer[2] = 'l';

    String* str_from_c_buffer = STRING_CREATE_FROM_BUFFER(char_buffer);
    string_print(str_from_c_buffer);


    String* slice_test = STRING_CREATE("Creating a String Slice");
    string_print(slice_test);
    String* slice4 = string_slice_from(slice_test, 8);
    string_print(slice4);
    String* slice18_23 = string_slice_from_to(slice_test, 18, 23);
    string_print(slice18_23);

    free(slice_test);
    free(slice18_23);

    String* string_for_token = STRING_CREATE("Creating a token of strings");
    String_Tokenizer* string_tokens = STRING_TOKENIZE(string_for_token);
    // TEST_DEBUG(string_tokens->number_of_strings == 5);
    for (int i = 0; i < string_tokens->number_of_strings; i++)
    {
        string_print(string_tokens->strings[i]);
    }

    String* token_the_array = STRING_CREATE("<Token>; of; a bunch; of ; dumb shit ");
    String_Tokenizer* super_token = string_tokenize_delimiter_array(token_the_array, &(STRING(";<> ")), true);
    for (int i = 0; i < super_token->number_of_strings; i++)
    {
        string_print(super_token->strings[i]);
    }



    const String* str_concat1 = STRING_CREATE("First ");
    const String* str_concat2 = STRING_CREATE("Second");
    String* str_concat_final = string_concat(str_concat1, str_concat2);
    string_print(str_concat_final);


    String* with_white_space = STRING_CREATE("I HAVE A LOT OF WHITE SPACE");
    String* without_white_space = string_strip_whitespace(with_white_space);
    string_print(with_white_space);
    string_print(without_white_space);




    TEST_REPORT("STRING");
}


#endif //STRINGS_H
