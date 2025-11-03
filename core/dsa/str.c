

#include "str.h"

#include <assert.h>
#include <string.h>



String* string_create_new(uint8_t* word, uint32_t length)
{

    String* str = malloc(sizeof(String));
    //memset(str, 0, sizeof(MString));

    //important to note that we use -1 to not include the null terminated string
    str->chars = (char*)malloc(sizeof(char) * length-1);
    memset(str->chars, 0, sizeof(char) * length-1);

    str->length = length-1;

    for (uint32_t i = 0; i < str->length; i++)
    {
        str->chars[i] = word[i];
    }

    return str;
}



String* string_create(const char* word, uint32_t length)
{
    String* str = malloc(sizeof(String));
    //memset(str, 0, sizeof(MString));

    //important to note that we use -1 to not include the null terminated string
    str->chars = (char*)malloc(sizeof(char) * length-1);
    memset(str->chars, 0, sizeof(char) * length-1);

    str->length = length-1;

    for (uint32_t i = 0; i < str->length; i++)
    {
        str->chars[i] = word[i];
    }

    return str;
}



bool string_free(String* string)
{
    assert(string);

    free(string->chars);
    //if (string->chars != NULL){ return false;}
    free(string);
    //if (string != NULL){ return false;}

    return true;
}

bool string_compare(const String* str1, const String* str2)
{
    assert(str1 != NULL);
    assert(str2 != NULL);
    if (str1->length != str2->length) return false;

    for (uint32_t i = 0; i < str1->length; i++)
    {
        if (str1->chars[i] != str2->chars[i]) return false;
    }

    return true;

}

void string_append(String* str1, String* str2)
{
    //realloc for the new size
    str1->chars = realloc(str1->chars, str1->length + str2->length);


    for (uint32_t i = 0; i < str2->length; i++)
    {
        str1->chars[str1->length+i] = str2->chars[i];
    }

    str1->length = str1->length + str2->length;

    string_print(str1);
}

void string_print(String* str)
{
    assert(str);

    for (uint32_t i = 0; i < str->length; i++)
    {
        printf("%c", str->chars[i]);
    }
    printf("\n");

}

bool string_is_empty(String* str)
{
    assert(str);
    return str->length == 0;
}

String* string_duplicate(String* str)
{
    //plus one cause we don't have a null string terminator
    return string_create(str->chars, str->length+1);

    //NOTE: doing a memcopy would be more perfomant
}


void string_test()
{
    String* testojnuaohdsus = STRING_CREATE("testing something");
    String* testojn = string_create_new("testing something", sizeof("testing something"));
    string_print(testojnuaohdsus);
    string_print(testojn);

    printf("STRING START \n");

    String* str1 = string_create("hello string", sizeof("hello string"));
    string_print(str1);

    //String* test_string_two = string_create_no_length("hello string");

    String* str3 = string_create("string, hello", sizeof("string, hello"));
    string_print(str3);



    printf("%d\n", string_compare(str1, str1));

    // string_append(str1, str2);


    String* str4 = string_duplicate(str1);
    printf("STRING 4\n");
    string_print(str4);

    string_free(str1);
    // string_free(str2);

    printf("STRING END\n\n");

}
