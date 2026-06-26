/*

file format:

variable[bool]:
    - true/false
variable[int32]:
    - 10
variable[uint32]:
    - 10
variable[float]:
    - 10.5
variable[string]:
    - things are bieng said
variable[string]:
    - things are bieng said
    - even on new lines
    //im thinking since its a string type, and i might want to break up its line,
    //then it makes sense to treat them kinda like arrays by default

array[uint][]:
    - 10
    - 10
    - 50

array[string][]:
    - "string"
    - "string1"
    - "string2"

map[type:type]: { "quantity": 10}
map[string:int]: { "quantity": 10}

map[type:type][]:
    - {"quantity": 10}
    - {"quantity": 10}

 variable[float]:
    -* //basically saying we have changed the value from the default, if thats important

//comments
//multiline comment, no you get no extra support for this
 */


/*
 creating what an ideal file format text based api would be

//TODO: what about nested structures??? fucking dont, simple as that, use an array
//TODO: get a few structs you would like to have serialized and see if this is practical at all
file: should support nested items ()
should be super easy to convert into a binary format

read

parse()
read_variable(type, &data)
read_array(type, &data)
read_map(type, &data)

example:



write (optional)

write_variable(name, type)
write_array(name, type)
write_map(name, type)



 */

#ifndef MADNESS_TXT_H
#define MADNESS_TXT_H

#include <stdbool.h>

#include "defines.h"
#include "compiler/reflection_system.h"

//NOTE: ILL BE USING A YAML FILE SINCE IT HAS SYNTAX HIGHLIGHTING, AND IS KIDNA BASED ON THAT FORMAT ANYWAY



typedef struct madness_txt_object_subfield
{
    const char* field_name;

    Reflection_Type type;
    u32 array_count;
    union
    {
        void* data;
        String* string;
    };


    struct madness_txt_object_subfield* next;
} madness_txt_object_subfield;

typedef struct madness_txt_object
{
    const char* name;
    madness_txt_object_subfield* head;
    madness_txt_object_subfield* last;
} madness_txt_object;


#define MADNESS_TXT_OBJECT_MAX 1000

typedef struct Madness_txt
{
    madness_txt_object objects[MADNESS_TXT_OBJECT_MAX];
    u32 object_count;

    //file parsed valuedata
} Madness_txt;



typedef struct object_handle
{
    u32 handle;
} object_handle;


//API

//TODO: use the allocator
Madness_txt* madness_txt_init(Memory_System* memory_system);

//TODO:
void madness_txt_shutdown(Madness_txt* txt);

//object
object_handle madness_txt_object_create(Madness_txt* txt, const char* name);
void madness_txt_object_add_item(Madness_txt* txt, object_handle handle, const char* field_name, Reflection_Type type,
                                 u32 array_size);


//write
void madness_txt_object_write_data_single(Madness_txt* txt, object_handle handle, const char* field_name,
                                       void* data);

void madness_txt_object_write_data_full(Madness_txt* txt, object_handle handle, void* struct_data);


//struct needs to be read in proper order that it was defined when using object add type
void madness_txt_object_write_data_arg(Madness_txt* txt, object_handle handle, ...);

void madness_txt_object_write_file(Madness_txt* txt, const char* file_path, object_handle* object_handle);

//read
void madness_txt_read_file(Madness_txt* txt, const char* file_path, object_handle* handle);

//NOTE ... is meant to be variable length of void* data, which if your struct fields
//struct needs to be read in proper order that it was defined when using object add type
void madness_txt_object_read_args(Madness_txt* txt, object_handle handle, ...);
void madness_txt_object_read(Madness_txt* txt, object_handle handle, const char* field_name,
                             void* write_to_data);


//internal
size_t madness_txt_str_to_type(char* str);
size_t madness_txt_type_size(Reflection_Type type);




//test
typedef struct madness_txt_test_struct_example
{
    bool bool_type;
    s32 signed_int_type;
    u32 unsigned_int_type;
    f32 float_type;
    char char_type;
    char* string_type;

    u32 array_type[3];
    u32 array_size;

    // madness_txt_key_pair map_key;
    u32 unsigned_key;
    f32 float_value;

    u32 unsigned_key_arr[3];
    u32 unsigned_value_arr[3];
    u32 hash_arr_size;

    char* string_array_type[2];
    u32 string_array_size;

    char char_array_type[2];
    u32 char_array_size;

    //TODO: enum
} madness_txt_test_struct_example;


void madness_txt_object_test_example(Madness_txt* txt);


#endif //MADNESS_TXT_H
