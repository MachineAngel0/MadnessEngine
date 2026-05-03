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


typedef struct Reflection_Type_Info
{
    const char* name;
    Reflection_Type type; // size is implicit in the type
    size_t offset; // this is here because padding is a thing
    u32 enum_size; // get the size of the num
} Reflection_Type_Info;

typedef struct Reflection_Enum
{
    const char* name; // name of struct/enum
    //darray
    darray_type(Reflection_Type_Info)* type_list;
} Reflection_Enum;


typedef struct Reflection_Struct
{
    const char* name; // name of struct/enum
    size_t structure_size; // this is here because padding is a thing
    darray_type(Reflection_Type_Info)* type_list;
} Reflection_Struct;


typedef struct reflection_system
{
    // key -> Reflection Structure->[list of types]
    hash_table* reflection_registry_enums;
    hash_table* reflection_registry_structs;
} Reflection_System;

Reflection_System* reflection_system_init()
{
    Reflection_System* reflection_system = (Reflection_System*)malloc(sizeof(Reflection_System));
    reflection_system->reflection_registry_structs = HASH_TABLE_CREATE(Reflection_Struct, 1000);
    reflection_system->reflection_registry_enums = HASH_TABLE_CREATE(Reflection_Enum, 1000);


    return reflection_system;
}

Reflection_System* reflection_system_shutdown()
{
    //TODO: use an arena
}

Reflection_System* reflection_system_add_enum(Reflection_System* reflection_system, const char* enum_name)
{
    Reflection_Enum reflection_enum = {0};
    reflection_enum.type_list = darray_create_reserve(Reflection_Type_Info, 1);
    reflection_enum.name = enum_name;
    hash_table_insert(reflection_system->reflection_registry_enums, enum_name, &reflection_enum);
}

Reflection_System* reflection_system_add_enum_field(Reflection_System* reflection_system, const char* enum_name,
                                                    const char* type_field_name, u32 enum_value)
{
    Reflection_Enum reflection_enum = {0};
    if (hash_table_get(reflection_system->reflection_registry_enums, enum_name, &reflection_enum))
    {
        Reflection_Type_Info type_info;
        type_info.name = type_field_name;
        type_info.type = REFLECTION_TYPE_ENUM; // size is implicit in the type
        type_info.offset = 0;
        type_info.enum_size = enum_value;

        darray_push(reflection_enum.type_list, type_info);

        hash_table_set(reflection_system->reflection_registry_enums, enum_name, &reflection_enum);
    }
    else
    {
        MASSERT(false);
    }
}

Reflection_Type_Info* reflection_system_enum_query(Reflection_System* reflection_system, const char* enum_name)
{
    Reflection_Enum reflection_enum = {0};
    if (hash_table_get(reflection_system->reflection_registry_enums, enum_name, &reflection_enum))
    {
        return reflection_enum.type_list;
    }
    MASSERT(false);
    return NULL;
}


bool reflection_system_add_struct(Reflection_System* reflection_system, const char* struct_name)
{
    Reflection_Struct reflection_struct = {0};
    reflection_struct.name = struct_name;
    reflection_struct.type_list = darray_create_reserve(Reflection_Type_Info, 1);
    reflection_struct.structure_size = 0;
    hash_table_insert(reflection_system->reflection_registry_structs, struct_name, &reflection_struct);
    return true;
}

bool reflection_system_add_struct_field(Reflection_System* reflection_system, const char* struct_name,
                                        Reflection_Type reflection_type, const char* type_struct_name)
{
    Reflection_Struct reflection_struct = {0};
    if (hash_table_get(reflection_system->reflection_registry_structs, struct_name, &reflection_struct))
    {
        Reflection_Type_Info type_info;
        type_info.name = type_struct_name;
        type_info.type = reflection_type; // size is implicit in the type
        type_info.offset = 0; //TODO: idk what to do about this, other than manually generating these

        darray_push(reflection_struct.type_list, type_info);

        hash_table_set(reflection_system->reflection_registry_structs, struct_name, &reflection_struct);
        return true;
    }
    else
    {
        MASSERT(false);
    }
    return false;
}

Reflection_Type_Info* reflection_system_struct_query(Reflection_System* reflection_system, const char* struct_name)
{
    Reflection_Struct reflection_struct = {0};
    if (hash_table_get(reflection_system->reflection_registry_structs, struct_name, &reflection_struct))
    {
        return reflection_struct.type_list;
    }
    MASSERT(false);
}

Reflection_Type_Info* reflection_system_generate_struct_offset(Reflection_System* reflection_system,
                                                               const char* struct_name)
{
    Reflection_Struct reflection_enum = {0};
    if (hash_table_get(reflection_system->reflection_registry_structs, struct_name, &reflection_enum))
    {
        return reflection_enum.type_list;
    }
    MASSERT(false);
}


void reflection_test()
{
    //TODO: we are not freeing anything and especially nothing using the string_builder to C-string function

    Reflection_System* reflection_system = reflection_system_init();

    Lexer* lexer3 = lexer_init();
    lexer_generate_tokens(lexer3, "../MadnessPulse/game_enums.h");
    u64 token_size = darray_get_size(lexer3->tokens);

    DEBUG("ENUM PARSE START")
    for (u64 i = 0; i < token_size; i++)
    {
        if (lexer3->tokens[i].type == Token_Enum)
        {
            //create a registry with the specific name
            i++; // get the name

            string_builder_print(&lexer3->tokens[i].string_builder);
            const char* enum_name = string_builder_to_c_string(&lexer3->tokens[i].string_builder);
            reflection_system_add_enum(reflection_system, enum_name);

            i++; // offset into the enum structure

            u32 enum_value = 0;
            while (i < token_size && lexer3->tokens[i].type != Token_CloseBrace)
            {
                if (lexer3->tokens[i].type == Token_Identifier)
                {
                    string_builder_print(&lexer3->tokens[i].string_builder);
                    reflection_system_add_enum_field(reflection_system, enum_name,
                                                     string_builder_to_c_string(&lexer3->tokens[i].string_builder),
                                                     enum_value);
                    enum_value++;
                }
                i++;
            }
        }
    }


    Reflection_Type_Info* info = reflection_system_enum_query(reflection_system, "Conjure_Type");
    size_t size = darray_get_size(info);

    for (u64 i = 0; i < size; i++)
    {
        printf("TEST VALUES: %s, enum value: %d \n", info[i].name, info[i].enum_size);
    }

    Lexer* lexer4 = lexer_init();
    lexer_generate_tokens(lexer4, "../MadnessPulse/game_structs.h");
    u64 token_size2 = darray_get_size(lexer4->tokens);

    Token_Type keep_list[] = {
        //single char tokens
        Token_OpenParen,
        Token_CloseParen,
        Token_Colon,
        Token_Semicolon,
        Token_Asterisk,
        Token_OpenBracket,
        Token_CloseBracket,
        Token_OpenBrace,
        Token_CloseBrace,

        // Literals.
        Token_String,
        Token_Identifier,
        Token_Number,

        //keywords
        Token_Enum,
        Token_Struct,

        Token_U8,
        Token_U16,
        Token_U32,
        Token_U64,
        Token_I8,
        Token_I16,
        Token_I32,
        Token_I64,
        Token_F32,
        Token_F64,
        Token_char,
        Token_size_t,
        Token_bool,

    };
    Token* token_list = lexer_prune_tokens(lexer4->tokens, keep_list, ARRAY_SIZE(keep_list));
    u64 pruned_token_list_size = darray_get_size(token_list);


    DEBUG("STRUCT PARSE START")
    for (u64 i = 0; i < pruned_token_list_size; i++)
    {
        if (token_list[i].type == Token_Struct)
        {
            i++; // move past the struct
            string_builder_print(&token_list[i].string_builder); // struct name
            const char* struct_name = string_builder_to_c_string(&token_list[i].string_builder);
            reflection_system_add_struct(reflection_system, struct_name);

            i++; // move past the open bracket
            while (i < token_size && token_list[i].type != Token_CloseBrace)
            {
                //what are our cases
                //type -> name
                //type* -> name
                //type -> name->[array]
                //name(struct/enum) -> name
                //name(struct/enum)* -> name
                //name(struct/enum) -> name[array]

                //check for the case of a basic type
                if (lexer_is_token_data_type(token_list[i]))
                {
                    if (token_list[i + 1].type == Token_Asterisk)
                    {
                        printf("is_pointer");
                        if (token_list[i + 2].type == Token_Identifier)
                        {
                            i++;
                            printf("valid");
                        }
                        i++;
                        continue;
                    }
                    else if (token_list[i + 1].type == Token_Identifier)
                    {
                        printf("valid");
                        //check if it's a compile time array
                        if (token_list[i + 2].type == Token_OpenBracket)
                        {
                            printf("array");
                            i++;
                        }
                        i++;
                        continue;
                    }
                }

                //check if the type is registered in the enums/structs section
                if (token_list[i].type == Token_Identifier)
                {
                    printf("idenfifier: %s\n", string_builder_to_c_string(&token_list[i].string_builder));

                    // hash_table_contains(reflection_system->reflection_registry_structs, token_list.)
                    if (hash_table_contains(reflection_system->reflection_registry_enums, string_builder_to_c_string(&token_list[i].string_builder)))
                    {
                       DEBUG("IS ENUM");
                    }

                }


                // Reflection_Type type = Compiler_type_to_Reflection_Type_LUT[token_list[i].type];
                /*if (type != REFLECTION_TYPE_INVALID)
                {
                    printf("%.*s: %.*s \n",
                           (int)token_list[i].string_builder.current_length, token_list[i].string_builder.str,
                           (int)token_list[i + 1].string_builder.current_length,
                           token_list[i + 1].string_builder.str);
                    // string_builder_print(&token_list[i].string_builder);
                    // string_builder_print(&token_list[i+1].string_builder);
                    reflection_system_add_struct_field(reflection_system, struct_name, type,
                                                       string_builder_to_c_string(
                                                           &token_list[i + 1].string_builder));
                    i++;
                }*/

                i++;
            }
        };
    }


    Reflection_Type_Info* struct_info = reflection_system_struct_query(reflection_system, "Reversal_List_Component");
    size_t struct_size = darray_get_size(info);

    for (u64 i = 0; i < struct_size; i++)
    {
        printf("TEST VALUES: %s\n", struct_info[i].name);
    }
}

#endif //REFLECTION_H
