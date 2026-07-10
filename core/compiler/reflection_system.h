#ifndef REFLECTION_H
#define REFLECTION_H

#include <stdbool.h>
#include "compiler.h"

//TODO: Move this out to defines and call it Madness_Types and name them {MADNESS_TYPE_U8}
typedef enum Reflection_Type
{
    REFLECTION_TYPE_INVALID,
    REFLECTION_TYPE_U8,
    REFLECTION_TYPE_U16,
    REFLECTION_TYPE_U32,
    REFLECTION_TYPE_U64,
    REFLECTION_TYPE_S8,
    REFLECTION_TYPE_S16,
    REFLECTION_TYPE_S32,
    REFLECTION_TYPE_S64,
    REFLECTION_TYPE_F32,
    REFLECTION_TYPE_F64,
    REFLECTION_TYPE_SIZE_T,
    REFLECTION_TYPE_BOOL,
    REFLECTION_TYPE_CHAR,
    REFLECTION_TYPE_CHAR_STRING, // char*
    REFLECTION_TYPE_STRING, // String*
    REFLECTION_TYPE_ENUM,
    REFLECTION_TYPE_STRUCT,

    REFLECTION_TYPE_VEC2,
    REFLECTION_TYPE_VEC3,
    REFLECTION_TYPE_VEC4,


    REFLECTION_TYPE_MAX,
} Reflection_Type;

Reflection_Type Compiler_type_to_Reflection_Type_LUT[] = {
    [Token_Unknown] = REFLECTION_TYPE_INVALID,
    [Token_U8] = REFLECTION_TYPE_U8,
    [Token_U16] = REFLECTION_TYPE_U16,
    [Token_U32] = REFLECTION_TYPE_U32,
    [Token_U64] = REFLECTION_TYPE_U64,
    [Token_I8] = REFLECTION_TYPE_S8,
    [Token_I16] = REFLECTION_TYPE_S16,
    [Token_I32] = REFLECTION_TYPE_S32,
    [Token_I64] = REFLECTION_TYPE_S64,
    [Token_F32] = REFLECTION_TYPE_F32,
    [Token_F64] = REFLECTION_TYPE_F64,
    [Token_char] = REFLECTION_TYPE_CHAR,
    [Token_bool] = REFLECTION_TYPE_BOOL,
    [Token_string_type] = REFLECTION_TYPE_STRING,
    [Token_String] = REFLECTION_TYPE_CHAR_STRING,
    [Token_size_t] = REFLECTION_TYPE_SIZE_T,
    [Token_Enum] = REFLECTION_TYPE_ENUM,
    [Token_Struct] = REFLECTION_TYPE_STRUCT,
    [Token_Identifier] = REFLECTION_TYPE_STRUCT,
};

typedef enum Reflection_Container_Type
{
    Reflection_Container_Type_VARIABLE,
    Reflection_Container_Type_STACK_ARRAY,
    Reflection_Container_Type_POINTER,
} Reflection_Container_Type;


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
    u32 enum_position; // get the size of the num
} Reflection_Type_Enum;

typedef struct Reflection_Enum
{
    const char* name; // name of struct/enum
    //darray
    darray_type(Reflection_Type_Enum)* type_list;
    u32 enum_size;
} Reflection_Enum;

typedef struct Reflection_Enum_Query_Array
{
    const char** enum_array_names;
    u32 enum_array_sizes;
} Reflection_Enum_Query_Array;


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
    darray_type(Reflection_Struct_Field)* type_list;
} Reflection_Struct;

typedef enum Reflection_Parse_Type
{
    REFLECTION_PARSE_CONSTANT,
    REFLECTION_PARSE_ENUM,
    REFLECTION_PARSE_STRUCT,
} Reflection_Parse_Type;

typedef struct Reflection_Compact_List
{
    //TODO: not in use, might not use
    Reflection_Constant* constant_list;
    u32 constant_list_size;
    Reflection_Enum* enum_list;
    u32 enum_list_size;
    Reflection_Struct* struct_list;
    u32 struct_list_size;



} Reflection_Compact_List;


typedef struct Reflection_System
{
    // key -> Reflection Constant
    HASH_TABLE_TYPE(Reflection_Constant)* reflection_registry_constants;
    // key -> Reflection_Enum -> Reflection_Type_Enum
    HASH_TABLE_TYPE(Reflection_Type_Enum)* reflection_registry_enums;
    // key -> Reflection_Struct -> Reflection_Type_Struct
    HASH_TABLE_TYPE(Reflection_Struct)* reflection_registry_structs;

    Allocator* allocator;
    Frame_Allocator* frame_allocator;

    String** header_file_list;
    u32 header_file_list_count;
#define HEADER_FILE_LIST_COUNT 10
    u32 header_file_list_capacity;

} Reflection_System;


Reflection_System* reflection_system_init(Memory_System* memory_system);

void reflection_system_shutdown();



//API

//builds the structures
void reflection_system_parse(Reflection_System* reflection_system, const char* file_path, Reflection_Parse_Type parse_type);

void reflection_system_parse_constants(Reflection_System* reflection_system, Lexer* lexer);
void reflection_system_parse_enums(Reflection_System* reflection_system, Lexer* lexer);
void reflection_system_parse_struct(Reflection_System* reflection_system, Lexer* lexer);


//get literally all the data, in a nice array format
Reflection_Compact_List reflection_system_get_data(Reflection_System* reflection_system);


//for individual queries
u64 reflection_system_constant_query(Reflection_System* reflection_system, const char* constant_name);
Reflection_Enum_Query_Array reflection_system_enum_query_list(Reflection_System* reflection_system,
                                                             const char* enum_name, Frame_Allocator* frame_allocator);
Reflection_Struct reflection_system_struct_query(Reflection_System* reflection_system, const char* struct_name);

//INTERNAL

//Constants
void reflection_system_add_constant(Reflection_System* reflection_system, const char* constant_name,  u64 value);


//Enum
void reflection_system_add_enum(Reflection_System* reflection_system, const char* enum_name);

void reflection_system_add_enum_field(Reflection_System* reflection_system, const char* enum_name,
                                      const char* type_field_name);

Reflection_Enum reflection_system_enum_query(Reflection_System* reflection_system, const char* enum_name);
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


bool reflection_system_does_struct_exist(Reflection_System* reflection_system, const char* struct_name);


Reflection_Struct_Field* reflection_system_generate_struct_offset(Reflection_System* reflection_system,
                                                                 const char* struct_name);

void reflection_game_data(Reflection_System* reflection_system);


//HELLA IMPORTANT FUNCTION CALL HERE
void reflection_data_to_files(Reflection_System* reflection_system, const char* function_name,
                              const char* generated_enum_file_path, const char* generated_struct_file_path);

#endif //REFLECTION_H
