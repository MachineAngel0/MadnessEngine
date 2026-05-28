#ifndef REFLECTION_H
#define REFLECTION_H


#include <stdbool.h>

#include "compiler.h"
#include "hash_table.h"

//TODO: implement the registry
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
    REFLECTION_TYPE_STRING,
    REFLECTION_TYPE_CHAR,
    REFLECTION_TYPE_ENUM,
    REFLECTION_TYPE_STRUCT,
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
    [Token_size_t] = REFLECTION_TYPE_SIZE_T,
    [Token_Enum] = REFLECTION_TYPE_ENUM,
    [Token_Struct] = REFLECTION_TYPE_STRUCT,
    [Token_Identifier] = REFLECTION_TYPE_STRUCT,
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
    u32 enum_position; // get the size of the num
} Reflection_Type_Enum;

typedef struct Reflection_Enum
{
    const char* name; // name of struct/enum
    //darray
    darray_type(Reflection_Type_Enum)* type_list;
    u32 enum_size;
} Reflection_Enum;

typedef struct Reflection_Enum_Query_List
{
    const char** enum_names;
    u32 enum_sizes;
} Reflection_Enum_Query_List;


typedef struct Reflection_Type_Struct
{
    const char* name;
    Reflection_Type type; // size is implicit in the type
    const char* type_name; // size is implicit in the type
    size_t offset; // this is here because padding is a thing
    bool is_ptr_type; //TODO:
    bool is_ptr_stack_type;
    u32 stack_size;
} Reflection_Type_Struct;

typedef struct Reflection_Struct
{
    const char* name; // name of struct/enum
    size_t struct_size; // this is here because padding is a thing
    darray_type(Reflection_Type_Struct)* type_list;
} Reflection_Struct;



typedef struct Reflection_System
{
    // key -> Reflection Constant
    hash_table* reflection_registry_constants;
    // key -> Reflection_Enum -> Reflection_Type_Enum
    hash_table* reflection_registry_enums;
    // key -> Reflection_Struct -> Reflection_Type_Struct
    hash_table* reflection_registry_structs;
} Reflection_System;

Reflection_System* reflection_system_init();

void reflection_system_shutdown();


//Constants
void reflection_system_add_constant(Reflection_System* reflection_system, const char* constant_name, const u64 value);

u64 reflection_system_constant_query(Reflection_System* reflection_system, const char* constant_name);


//Enum
void reflection_system_add_enum(Reflection_System* reflection_system, const char* enum_name);

void reflection_system_add_enum_field(Reflection_System* reflection_system, const char* enum_name,
                                      const char* type_field_name);

Reflection_Enum reflection_system_enum_query(Reflection_System* reflection_system, const char* enum_name);
bool reflection_system_does_enum_exist(Reflection_System* reflection_system, const char* enum_name);


Reflection_Enum_Query_List reflection_system_enum_query_list(Reflection_System* reflection_system,
                                                             const char* enum_name, Frame_Allocator* frame_allocator);

//Struct
bool reflection_system_add_struct(Reflection_System* reflection_system, const char* struct_name);


bool reflection_system_add_struct_field(Reflection_System* reflection_system, const char* struct_name,
                                        Reflection_Type reflection_type, const char* type_name, const char* struct_member_name);

bool reflection_system_add_struct_field_ptr_heap(Reflection_System* reflection_system, const char* struct_name,
                                                 Reflection_Type reflection_type, const char* type_name, const char* struct_member_name);
bool reflection_system_add_struct_field_ptr_stack(Reflection_System* reflection_system, const char* struct_name,
                                                  Reflection_Type reflection_type, const char* type_name, const char* struct_member_name, u64 array_size);

Reflection_Struct reflection_system_struct_query(Reflection_System* reflection_system, const char* struct_name);

bool reflection_system_does_struct_exist(Reflection_System* reflection_system, const char* struct_name);



Reflection_Type_Struct* reflection_system_generate_struct_offset(Reflection_System* reflection_system,
                                                                 const char* struct_name);

Reflection_System* reflection_game_data();

#endif //REFLECTION_H
