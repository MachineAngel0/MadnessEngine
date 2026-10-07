#ifndef REFLECTION_H
#define REFLECTION_H

#include <stdbool.h>
#include "madness_lexer.h"
#include "reflection_types.h"



#define REFLECTION_HEADER_FILE_LIST_COUNT 10
#define REFLECTION_ARRAY_SIZES 1000
// NOTE: you could probably do 128 and no one would ever likely go over that, unless their insane
#define REFLECTION_TYPE_FIELDS_MAX 64

typedef enum Reflection_Container_Type
{
    Reflection_Container_Type_VARIABLE,
    Reflection_Container_Type_STACK_ARRAY,
    Reflection_Container_Type_POINTER,
} Reflection_Container_Type;

typedef enum Reflection_Enum_Type
{
    Reflection_Enum_Type_Normal,
    Reflection_Enum_Type_Bitflag,
} Reflection_Enum_Type;

const char* Reflection_Enum_Type_String_Lut[] =
{
    [Reflection_Enum_Type_Normal] = "Reflection_Enum_Type_Normal",
    [Reflection_Enum_Type_Bitflag] = "Reflection_Enum_Type_Bitflag",
};


typedef struct Reflection_Constant
{
    const char* name; // name of constant
    //info about the constant, i don
    // Reflection_Type type; // TODO: maybe, for now i should avoid floats
    u64 value;
} Reflection_Constant;

typedef struct Reflection_Type_Enum
{
    const char* name;
    u32 enum_position; // get the size of the enum
    Reflection_Enum_Type type;
    u64 bitvalue;
} Reflection_Type_Enum;

typedef struct Reflection_Enum
{
    const char* name; // name of struct/enum
    Reflection_Type_Enum type_list[REFLECTION_TYPE_FIELDS_MAX];
    u32 enum_size;
} Reflection_Enum;


typedef struct Reflection_Struct_Field
{
    const char* field_name;
    Reflection_Type type; // size is implicit in the type
    const char* type_name; // size is implicit in the type
    Reflection_Container_Type container_type;
    u32 stack_size;
} Reflection_Struct_Field;

typedef struct Reflection_Struct
{
    const char* name; // name of struct/enum
    size_t struct_size; // this is here because padding is a thing
    Reflection_Struct_Field type_list[REFLECTION_TYPE_FIELDS_MAX];
    u32 type_list_size;
} Reflection_Struct;

typedef enum Reflection_Parse_Type
{
    REFLECTION_PARSE_CONSTANT,
    REFLECTION_PARSE_ENUM,
    REFLECTION_PARSE_STRUCT,
} Reflection_Parse_Type;


typedef struct Reflection_System
{
    Allocator* allocator;
    Frame_Allocator* frame_allocator;

    Reflection_Constant* reflection_registry_constants;
    u32 reflection_constants_size;
    u32 reflection_constants_size_max;

    Reflection_Enum* reflection_registry_enums;
    u32 reflection_enums_size;
    u32 reflection_enum_size_max;

    Reflection_Struct* reflection_registry_structs;
    u32 reflection_struct_size;
    u32 reflection_struct_size_max;


    String** header_file_list;
    u32 header_file_list_count;
    u32 header_file_list_capacity;
} Reflection_System;


Reflection_System* reflection_system_init(Memory_System* memory_system);
void reflection_system_deinit(Reflection_System* reflection_system, Memory_System* memory_system);

void reflection_system_reset(Reflection_System* reflection_system);

//API

//builds the structures
void reflection_system_parse(Reflection_System* reflection_system, const char* file_path,
                             Reflection_Parse_Type parse_type);

void reflection_system_parse_constants(Reflection_System* reflection_system, Lexer* lexer);
void reflection_system_parse_enums(Reflection_System* reflection_system, Lexer* lexer);
void reflection_system_parse_struct(Reflection_System* reflection_system, Lexer* lexer);


//for individual queries
u64 reflection_system_constant_query(Reflection_System* reflection_system, const char* constant_name);
Reflection_Struct* reflection_system_struct_query(Reflection_System* reflection_system, const char* struct_name);

//INTERNAL

//Constants
void reflection_system_add_constant(Reflection_System* reflection_system, const char* constant_name, u64 value);


//Enum
void reflection_system_add_enum_entry(Reflection_System* reflection_system, const char* enum_name);

void reflection_system_add_enum_field(Reflection_System* reflection_system, const char* enum_name,
                                      const char* type_field_name, Reflection_Enum_Type enum_type, u64 bitflag_value);

Reflection_Enum* reflection_system_enum_query(Reflection_System* reflection_system, const char* enum_name);
bool reflection_system_does_enum_exist(Reflection_System* reflection_system, const char* enum_name);


//Struct
bool reflection_system_add_struct(Reflection_System* reflection_system, const char* struct_name);


bool reflection_system_add_struct_field(Reflection_System* reflection_system, const char* struct_name,
                                        Reflection_Type reflection_type, const char* type_name,
                                        const char* struct_member_name);

bool reflection_system_add_struct_field_ptr_heap(Reflection_System* reflection_system, const char* struct_name,
                                                 Reflection_Type reflection_type, const char* type_name,
                                                 const char* struct_member_name);
bool reflection_system_add_struct_field_ptr_stack(Reflection_System* reflection_system, const char* struct_name,
                                                  Reflection_Type reflection_type, const char* type_name,
                                                  const char* struct_member_name, u64 array_size);

bool reflection_system_set_default_values(Reflection_System* reflection_system, Reflection_Type reflection_type,
                                          void* data);


bool reflection_system_does_struct_exist(Reflection_System* reflection_system, const char* struct_name);


//HELLA IMPORTANT FUNCTION CALL HERE
void reflection_data_to_files(Reflection_System* reflection_system, const char* function_name,
                              const char* generated_enum_file_path, const char* generated_struct_file_path);

void reflection_data_generate_structs_from_shader(Reflection_System* reflection_system, const char* function_name,
                                                  const char* generated_struct_file_path);





#endif //REFLECTION_H
