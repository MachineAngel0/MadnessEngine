#include "reflection_system.h"

#include "array.h"

Reflection_System* reflection_system_init()
{
    Reflection_System* reflection_system = (Reflection_System*)malloc(sizeof(Reflection_System));
    reflection_system->reflection_registry_constants = HASH_TABLE_CREATE(Reflection_Constant, 1000);
    reflection_system->reflection_registry_enums = HASH_TABLE_CREATE(Reflection_Enum, 1000);
    reflection_system->reflection_registry_structs = HASH_TABLE_CREATE(Reflection_Struct, 1000);


    return reflection_system;
}

void reflection_system_shutdown()
{
    //TODO: use an arena
}


void reflection_system_add_constant(Reflection_System* reflection_system, const char* constant_name, const u64 value)
{
    Reflection_Constant reflection_constant = {0};
    reflection_constant.name = constant_name;
    reflection_constant.value = value;
    hash_table_insert(reflection_system->reflection_registry_constants, constant_name, &reflection_constant);
}

u64 reflection_system_constant_query(Reflection_System* reflection_system, const char* constant_name)
{
    hash_table_contains(reflection_system->reflection_registry_constants, constant_name);

    Reflection_Constant reflection_constant = {0};
    if (hash_table_get(reflection_system->reflection_registry_constants, constant_name, &reflection_constant))
    {
        return reflection_constant.value;
    }
    MASSERT(false);
    return 0;
}


void reflection_system_add_enum(Reflection_System* reflection_system, const char* enum_name)
{
    Reflection_Enum reflection_enum = {0};
    reflection_enum.type_list = darray_create_reserve(Reflection_Type_Enum, 1);
    reflection_enum.name = enum_name;
    hash_table_insert(reflection_system->reflection_registry_enums, enum_name, &reflection_enum);
}

void reflection_system_add_enum_field(Reflection_System* reflection_system, const char* enum_name,
                                      const char* type_field_name)
{
    Reflection_Enum reflection_enum = {0};
    if (hash_table_get(reflection_system->reflection_registry_enums, enum_name, &reflection_enum))
    {
        Reflection_Type_Enum type_info;
        type_info.name = type_field_name;
        type_info.enum_position = reflection_enum.enum_size;

        reflection_enum.enum_size += 1;
        darray_push(reflection_enum.type_list, type_info);

        hash_table_set(reflection_system->reflection_registry_enums, enum_name, &reflection_enum);

        reflection_system_add_constant(reflection_system, type_field_name, type_info.enum_position);
    }
    else
    {
        MASSERT(false);
    }
}

Reflection_Enum reflection_system_enum_query(Reflection_System* reflection_system, const char* enum_name)
{
    Reflection_Enum reflection_enum = {0};
    if (hash_table_get(reflection_system->reflection_registry_enums, enum_name, &reflection_enum))
    {
        return reflection_enum;
    }
    return (Reflection_Enum){0};
}

bool reflection_system_does_enum_exist(Reflection_System* reflection_system, const char* enum_name)
{
    return hash_table_contains(reflection_system->reflection_registry_enums, enum_name);
}

Reflection_Enum_Query_List reflection_system_enum_query_list(Reflection_System* reflection_system,
                                                             const char* enum_name, Frame_Allocator* frame_allocator)
{
    //generate the enums first and then the struct data we would want
    Reflection_Enum enum_info = reflection_system_enum_query(reflection_system, enum_name);

    u64 enum_iteration_size = darray_get_size(enum_info.type_list);

    Reflection_Enum_Query_List query_list = {0};
    query_list.enum_sizes = enum_iteration_size;
    query_list.enum_names = allocator_alloc(frame_allocator, sizeof(char*) * query_list.enum_sizes);

    for (u64 j = 0; j < enum_iteration_size; j++)
    {
        query_list.enum_names[j] = enum_info.type_list[j].name;
    }


    return query_list;
}


bool reflection_system_add_struct(Reflection_System* reflection_system, const char* struct_name)
{
    Reflection_Struct reflection_struct = {0};
    reflection_struct.name = struct_name;
    reflection_struct.type_list = darray_create_reserve(Reflection_Type_Struct, 1);
    reflection_struct.struct_size = 0;
    hash_table_insert(reflection_system->reflection_registry_structs, struct_name, &reflection_struct);
    return true;
}

bool reflection_system_add_struct_field(Reflection_System* reflection_system, const char* struct_name,
                                        Reflection_Type reflection_type, const char* type_name,
                                        const char* struct_member_name)
{
    Reflection_Struct reflection_struct = {0};
    if (hash_table_get(reflection_system->reflection_registry_structs, struct_name, &reflection_struct))
    {
        Reflection_Type_Struct type_info = {0};
        type_info.name = struct_member_name;
        type_info.type = Compiler_type_to_Reflection_Type_LUT[reflection_type]; // size is implicit in the type
        type_info.type_name = type_name;
        type_info.offset = 0;
        type_info.is_ptr_type = false;
        type_info.is_ptr_stack_type = false;

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

bool reflection_system_add_struct_field_ptr_heap(Reflection_System* reflection_system, const char* struct_name,
                                                 Reflection_Type reflection_type, const char* type_name,
                                                 const char* struct_member_name)
{
    Reflection_Struct reflection_struct = {0};
    if (hash_table_get(reflection_system->reflection_registry_structs, struct_name, &reflection_struct))
    {
        Reflection_Type_Struct type_info = {0};
        type_info.name = struct_member_name;
        type_info.type = Compiler_type_to_Reflection_Type_LUT[reflection_type]; // size is implicit in the type
        type_info.type_name = type_name;
        type_info.offset = 0;
        type_info.is_ptr_type = true;
        type_info.is_ptr_stack_type = false;
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


bool reflection_system_add_struct_field_ptr_stack(Reflection_System* reflection_system, const char* struct_name,
                                                  Reflection_Type reflection_type, const char* type_name,
                                                  const char* struct_member_name, u64 array_size)

{
    Reflection_Struct reflection_struct = {0};
    if (hash_table_get(reflection_system->reflection_registry_structs, struct_name, &reflection_struct))
    {
        Reflection_Type_Struct type_info = {0};
        type_info.name = struct_member_name;
        type_info.type = Compiler_type_to_Reflection_Type_LUT[reflection_type]; // size is implicit in the type
        type_info.type_name = type_name;
        type_info.offset = 0;
        type_info.is_ptr_stack_type = true;
        type_info.is_ptr_type = false;
        type_info.stack_size = array_size;

        darray_push(reflection_struct.type_list, type_info);

        hash_table_set(reflection_system->reflection_registry_structs, struct_name, &reflection_struct);
        return true;
    }
    MASSERT(false);
    return false;
}

Reflection_Struct reflection_system_struct_query(Reflection_System* reflection_system, const char* struct_name)
{
    Reflection_Struct reflection_struct = {0};
    if (hash_table_get(reflection_system->reflection_registry_structs, struct_name, &reflection_struct))
    {
        return reflection_struct;
    }
    MASSERT(false);
    return (Reflection_Struct){0};
}

bool reflection_system_does_struct_exist(Reflection_System* reflection_system, const char* struct_name)
{
    return hash_table_contains(reflection_system->reflection_registry_structs, struct_name);
}

Reflection_Type_Struct* reflection_system_generate_struct_offset(Reflection_System* reflection_system,
                                                                 const char* struct_name)
{
    Reflection_Struct reflection_enum = {0};
    if (hash_table_get(reflection_system->reflection_registry_structs, struct_name, &reflection_enum))
    {
        return reflection_enum.type_list;
    }
    MASSERT(false);
    return NULL;
}


Reflection_System* reflection_game_data()
{
    //TODO: we are not freeing anything and especially nothing using the string_builder to C-string function

    Reflection_System* reflection_system = reflection_system_init();

    Lexer* lexer_constants = lexer_init();
    lexer_generate_tokens(lexer_constants, "../MadnessPulse/game_constants.h");
    u64 token_size_constants = darray_get_size(lexer_constants->tokens);


    DEBUG("CONSTANTS PARSE START")
    for (u64 i = 0; i < token_size_constants; i++)
    {
        if (lexer_constants->tokens[i].type == Token_MACRO)
        {
            if (&lexer_constants->tokens[i + 1] && lexer_constants->tokens[i + 1].type == Token_Identifier)
            {
                if (&lexer_constants->tokens[i + 2] && lexer_constants->tokens[i + 2].type == Token_Number)
                {
                    reflection_system_add_constant(reflection_system,
                                                   string_builder_to_c_string(
                                                       &lexer_constants->tokens[i + 1].string_builder),
                                                   string_builder_to_number(
                                                       &lexer_constants->tokens[i + 2].string_builder));
                    i += 2;
                }
            }
        }
    }

    // u64 val =  reflection_system_constant_query(reflection_system, "INVENTORY_MAX_BATTLE_LIST");


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

            // string_builder_print(&lexer3->tokens[i].string_builder);
            const char* enum_name = string_builder_to_c_string(&lexer3->tokens[i].string_builder);
            reflection_system_add_enum(reflection_system, enum_name);

            i++; // offset into the enum structure

            u32 enum_value = 0;
            while (i < token_size && lexer3->tokens[i].type != Token_CloseBrace)
            {
                if (lexer3->tokens[i].type == Token_Identifier)
                {
                    // string_builder_print(&lexer3->tokens[i].string_builder);
                    reflection_system_add_enum_field(reflection_system, enum_name,
                                                     string_builder_to_c_string(&lexer3->tokens[i].string_builder));
                    enum_value++;
                }
                i++;
            }
        }
    }


    Lexer* lexer4 = lexer_init();
    lexer_generate_tokens(lexer4, "../MadnessPulse/game_structs.h");

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
            // string_builder_print(&token_list[i].string_builder); // struct name
            const char* struct_name = string_builder_to_c_string(&token_list[i].string_builder);
            reflection_system_add_struct(reflection_system, struct_name);

            i++; // move past the open bracket
            while (i < pruned_token_list_size && token_list[i].type != Token_CloseBrace)
            {
                //what are our cases
                //type -> name
                //type* -> name
                //type -> name->[array]
                //name(struct/enum) -> name
                //name(struct/enum)* -> name
                //name(struct/enum) -> name[array]


                //process a basic type first
                if (lexer_is_token_data_type(token_list[i]))
                {
                    if (token_list[i + 1].type == Token_Identifier)
                    {
                        //check for stack pointer
                        if (token_list[i + 2].type == Token_OpenBracket)
                        {
                            if (token_list[i + 3].type == Token_Identifier)
                            {
                                //generate the array member
                                u64 array_size = reflection_system_constant_query(
                                    reflection_system, string_builder_to_c_string(&token_list[i + 3].string_builder));
                                reflection_system_add_struct_field_ptr_stack(reflection_system, struct_name,
                                                                             token_list[i].type,
                                                                             string_builder_to_c_string(
                                                                                 &token_list[i].string_builder),
                                                                             string_builder_to_c_string(
                                                                                 &token_list[i + 1].string_builder),
                                                                             array_size);
                                i += 3;
                            }
                            else if (token_list[i + 3].type == Token_Number)
                            {
                                reflection_system_add_struct_field_ptr_stack(reflection_system, struct_name,
                                                                             token_list[i].type,
                                                                             string_builder_to_c_string(
                                                                                 &token_list[i].string_builder),
                                                                             string_builder_to_c_string(
                                                                                 &token_list[i + 1].string_builder),
                                                                             string_builder_to_number(
                                                                                 &token_list[i + 3].string_builder));
                                i += 3;
                            }
                        }
                        else // basic type
                        {
                            reflection_system_add_struct_field(reflection_system, struct_name,
                                                               token_list[i].type,
                                                               "", string_builder_to_c_string(
                                                                   &token_list[i + 1].string_builder));
                            i += 1;
                        }
                    }
                    //check for heap pointer
                    else if (token_list[i + 1].type == Token_Asterisk)
                    {
                        if (token_list[i + 2].type == Token_Identifier)
                        {
                            //generate the array member
                            reflection_system_add_struct_field_ptr_heap(reflection_system,
                                                                        struct_name, token_list[i].type,
                                                                        string_builder_to_c_string(
                                                                            &token_list[i].string_builder),
                                                                        string_builder_to_c_string(
                                                                            &token_list[i + 2].string_builder));
                            i += 2;
                        }
                    }
                }
                else if (token_list[i].type == Token_Identifier) // this can be either a struct or an enum
                {
                    if (reflection_system_does_enum_exist(reflection_system,
                                                          string_builder_to_c_string(&token_list[i].string_builder)))
                    {
                        if (token_list[i + 1].type == Token_Identifier)
                        {
                            //check for stack pointer
                            if (token_list[i + 2].type == Token_OpenBracket)
                            {
                                if (token_list[i + 3].type == Token_Identifier)
                                {
                                    //generate the array member
                                    u64 array_size = reflection_system_constant_query(
                                        reflection_system,
                                        string_builder_to_c_string(&token_list[i + 3].string_builder));
                                    reflection_system_add_struct_field_ptr_stack(reflection_system, struct_name,
                                        Token_Enum, string_builder_to_c_string(
                                            &token_list[i].string_builder),
                                        string_builder_to_c_string(
                                            &token_list[i + 1].string_builder),
                                        array_size);
                                    i += 3;
                                }
                                else if (token_list[i + 3].type == Token_Number)
                                {
                                    reflection_system_add_struct_field_ptr_stack(reflection_system, struct_name,
                                        Token_Enum, string_builder_to_c_string(
                                            &token_list[i].string_builder),
                                        string_builder_to_c_string(
                                            &token_list[i + 1].string_builder),
                                        string_builder_to_number(
                                            &token_list[i + 3].string_builder));
                                    i += 3;
                                }
                            }
                            else // basic type
                            {
                                reflection_system_add_struct_field(reflection_system, struct_name,
                                                                   Token_Enum,
                                                                   string_builder_to_c_string(
                                                                       &token_list[i].string_builder),
                                                                   string_builder_to_c_string(
                                                                       &token_list[i + 1].string_builder));
                                i += 1;
                            }
                        }
                        //check for heap pointer
                        else if (token_list[i + 1].type == Token_Asterisk)
                        {
                            if (token_list[i + 2].type == Token_Identifier)
                            {
                                //generate the array member
                                reflection_system_add_struct_field_ptr_heap(reflection_system,
                                                                            struct_name, Token_Enum,
                                                                            string_builder_to_c_string(
                                                                                &token_list[i].string_builder),
                                                                            string_builder_to_c_string(
                                                                                &token_list[i + 2].string_builder));
                                i += 2;
                            }
                        }
                    }
                    else if (reflection_system_does_struct_exist(reflection_system,
                                                                 string_builder_to_c_string(
                                                                     &token_list[i].string_builder)))
                    {
                        if (token_list[i + 1].type == Token_Identifier)
                        {
                            //check for stack pointer
                            if (token_list[i + 2].type == Token_OpenBracket)
                            {
                                if (token_list[i + 3].type == Token_Identifier)
                                {
                                    //generate the array member
                                    u64 array_size = reflection_system_constant_query(
                                        reflection_system,
                                        string_builder_to_c_string(&token_list[i + 3].string_builder));
                                    reflection_system_add_struct_field_ptr_stack(reflection_system, struct_name,
                                        Token_Struct, string_builder_to_c_string(
                                            &token_list[i].string_builder),
                                        string_builder_to_c_string(
                                            &token_list[i + 1].string_builder),
                                        array_size);
                                    i += 3;
                                }
                                else if (token_list[i + 3].type == Token_Number)
                                {
                                    reflection_system_add_struct_field_ptr_stack(reflection_system, struct_name,
                                        Token_Struct, string_builder_to_c_string(
                                            &token_list[i].string_builder),
                                        string_builder_to_c_string(
                                            &token_list[i + 1].string_builder),
                                        string_builder_to_number(
                                            &token_list[i + 3].string_builder));
                                    i += 3;
                                }
                            }
                            else // basic type
                            {
                                reflection_system_add_struct_field(reflection_system, struct_name,
                                                                   Token_Struct,
                                                                   string_builder_to_c_string(
                                                                       &token_list[i].string_builder),
                                                                   string_builder_to_c_string(
                                                                       &token_list[i + 1].string_builder));
                                i += 1;
                            }
                        }
                        //check for heap pointer
                        else if (token_list[i + 1].type == Token_Asterisk)
                        {
                            if (token_list[i + 2].type == Token_Identifier)
                            {
                                //generate the array member
                                reflection_system_add_struct_field_ptr_heap(reflection_system,
                                                                            struct_name, Token_Struct,
                                                                            string_builder_to_c_string(
                                                                                &token_list[i].string_builder),
                                                                            string_builder_to_c_string(
                                                                                &token_list[i + 2].string_builder));
                                i += 2;
                            }
                        }
                    }
                }
                i++;
            }
        }
    }


    //generate the enums first and then the struct data we would want
    FILE* reflection_file = fopen("../MadnessPulse/game_reflection_generated.c", "w");
    const char* header =
        "#include <stddef.h>\n"
        "#include \"game_constants.h\"\n"
        "#include \"game_enums.h\"\n"
        "#include \"game_structs.h\"\n\n";

    fwrite(header, strlen(header), 1, reflection_file);


    for (u64 i = 0; i < reflection_system->reflection_registry_enums->capacity; i++)
    {
        if (!reflection_system->reflection_registry_enums->key_str_data[i]) { continue; }

        Reflection_Enum enum_info = reflection_system_enum_query(
            reflection_system, reflection_system->reflection_registry_enums->key_str_data[i]);
        if (!enum_info.name) { continue; }

        fwrite("const char* ", strlen("const char* "), 1, reflection_file);
        fwrite(enum_info.name, strlen(enum_info.name), 1, reflection_file);
        fwrite("_enum_string[] = {\n", strlen("_Enum_String[] = {\n"), 1, reflection_file);

        u64 enum_iteration_size = darray_get_size(enum_info.type_list);
        for (u64 j = 0; j < enum_iteration_size; j++)
        {
            fwrite("\t[", strlen("\t["), 1, reflection_file);
            fwrite(enum_info.type_list[j].name, strlen(enum_info.type_list[j].name), 1, reflection_file);
            fwrite("]= \"", strlen("]= \""), 1, reflection_file);
            fwrite(enum_info.type_list[j].name, strlen(enum_info.type_list[j].name), 1, reflection_file);
            fwrite("\", \n", strlen("\", \n"), 1, reflection_file);
        }

        fwrite("};\n\n", strlen("};\n\n"), 1, reflection_file);
    }


    /*
        for (u64 i = 0; i < reflection_system->reflection_registry_structs->capacity; i++)
        {
            if (!reflection_system->reflection_registry_structs->key_str_data[i]) { continue; }

            Reflection_Struct struct_info = reflection_system_struct_query(
                   reflection_system, reflection_system->reflection_registry_structs->key_str_data[i]);
            if (!struct_info.type_list->name) { continue; }
            // struct_info->name;
            u64 struct_iteration_size = darray_get_size(struct_info.type_list);

            // void component_serialize(){
            fwrite("void", strlen("void"), 1, reflection_file);
            fwrite(" ", 1, 1, reflection_file);
            fwrite(struct_info.name, strlen(struct_info.name), 1, reflection_file);
            const char* function_name = "_serialize(){\n";
            fwrite(function_name, strlen(function_name), 1, reflection_file);

            for (u64 j = 0; j < struct_iteration_size; j++)
            {
                if (!struct_info.type_list[j].name) { continue; }
                //offsetof(struct_info.name, struct_info.type_list[j].name);
                fwrite("\toffsetof(", strlen("\toffsetof("), 1, reflection_file);
                fwrite(struct_info.name, strlen(struct_info.name), 1, reflection_file);
                fwrite(", ", strlen(", "), 1, reflection_file);
                fwrite(struct_info.type_list[j].name, strlen(struct_info.type_list[j].name), 1, reflection_file);
                fwrite(");\n", strlen(");\n"), 1, reflection_file);
            }
            fwrite("}\n\n", strlen("}\n\n"), 1, reflection_file);
        }
    */

    fclose(reflection_file);

    return reflection_system;
}
