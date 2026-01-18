#ifndef String_C_H
#define String_C_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"


// contains the null terminated String_C
// used mainly for handling paths
typedef struct String_C
{
    char* chars;
    u64 length; // does not include the size for the null terminated string
} String_C;

//NOTE: do not call this, just use String_C_CREATE(String_C) unless you specifically need to pass in the size for some reason
String_C* string_c_create(const char* word, const u64 length)
{
    String_C* str = malloc(sizeof(String_C));
    MASSERT_MSG(str, "C-STRING MALLOC FAILED");
    str->length = length;
    str->chars = (char*)malloc((str->length + 1)); // +1 for the null char
    MASSERT_MSG(str->chars, "C-STRING CHARS MALLOC FAILED");

    memcpy(str->chars, word, (str->length));
    str->chars[length] = '\0';


    return str;
};


bool string_c_free(String_C* str_c)
{
    MASSERT(str_c);
    if (!str_c)
    {
        WARN("INVALID String_C CANT FREE");
        return false;
    }
    if (!str_c->chars)
    {
        WARN("INVALID CHAR* INSIDE String_C CANT FREE");
        return false;
    }

    free(str_c->chars);
    free(str_c);

    return true;
}


#define String_C_CREATE(string) string_c_create(string, sizeof(string))
#define STRING_C_CREATE_BUFFER(string) string_c_create(string, strlen(string))


//UTILITY
void string_c_print(const String_C* str)
{
    MASSERT(str);
    //just a normal string print
    printf("%s\n", str->chars);
}


String_C* string_c_slice_from(const String_C* s, const u64 slice_size)
{
    //creates a new string ending where the passed in size was

    if (slice_size > s->length)
    {
        WARN("String_C SLICE FROM: Passed in slice_size greater than String_C length")
        return NULL;
    }

    return string_c_create(s->chars, slice_size);
}

String_C* string_c_strip_from_end(String_C* str, const char stop_character)
{
    //creates a new string that stops and includes the stop_character

    u64 i = str->length-1;
    for (; i > 0; i--)
    {
        if (str->chars[i] == stop_character)
        {
            break;
        }
    }

    //where ever I end up is the new length of the String_C
    return string_c_slice_from(str, i + 1);
}


String_C* string_c_concat(const String_C* str1, const String_C* str2)
{
    //creates a new String_C from the two String_Cs

    String_C* out_str = malloc(sizeof(String_C));
    u64 combined_length = str1->length + str2->length;
    out_str->chars = (char*)malloc(combined_length + 1);
    memset(out_str->chars, 0, combined_length);

    out_str->length = combined_length;

    memcpy(out_str->chars, str1->chars, str1->length);
    memcpy(&out_str->chars[str1->length], str2->chars, str2->length);
    out_str->chars[out_str->length] = '\0';

    return out_str;
}


#define String_C_strip_path(str_c) string_c_strip_from_end(str_c, '\\')


/*
bool String_C_compare(const String_C* str1, const String_C* str2)
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


bool str_is_empty(const String_C* str)
{
    MASSERT(str);
    return str->length == 0;
}

String_C* String_C_duplicate(const String_C* str)
{
    //plus one cause we don't have a null String_C terminator
    return String_C_create_internal(str);
}



String_C* String_C_strip_whitespace(String_C* str)
{
    String_C* out_String_C = String_C_duplicate(str);

    //two pointer
    u64 i = 0;
    u64 j = 0; // iterates forward
    while (j < str->length)
    {
        if (str->chars[j] != ' ')
        {
            out_String_C->chars[i] = str->chars[j];
            i++;
        }
        j++;
    }
    //where ever i ends up is the new length of the String_C
    out_String_C->length = i;
    return out_String_C;
}


const char* String_C_convert_to_c_String_C(const String_C* s)
{
    //when you need to convert it to a valid directory, this function is nice
    char* c_String_C = malloc(sizeof(char) * (s->length + 1)); // +1 for the null terminated String_C
    memcpy(c_String_C, s->chars, s->length);
    c_String_C[s->length] = '\0';
    return c_String_C;
}




String_C* String_C_slice_from_to(const String_C* s, const u64 slice_begin, const u64 slice_end)
{
    MASSERT(s);

    if (slice_begin > slice_end)
    {
        WARN("String_C FROM: Passed in slice begin greater than slice end")
        return NULL;
    }
    if (slice_end > s->length)
    {
        WARN("String_C SLICE FROM TO : Passed in slice_end greater than String_C length")
        return NULL;
    }

    String_C* new_String_C_str = malloc(sizeof(String_C));
    const u64 new_length = slice_end - slice_begin;

    new_String_C_str->chars = (char*)malloc(sizeof(char) * new_length);
    memset(new_String_C_str->chars, 0, sizeof(char) * new_length);

    new_String_C_str->length = new_length;

    for (u64 i = 0; i < new_length; i++)
    {
        //copy from the passed in, to the new String_C
        new_String_C_str->chars[i] = s->chars[slice_begin + i];
    }

    return new_String_C_str;
}


*/


#endif //String_CS_H
