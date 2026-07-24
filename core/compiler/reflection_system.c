#include "reflection_system.h"


Reflection_System* reflection_system_init(Memory_System* memory_system)
{
    Reflection_System* reflection_system = memory_system_alloc(memory_system, sizeof(Allocator),
                                                               MEMORY_SUBSYSTEM_REFLECTION);
    reflection_system->reflection_registry_constants = HASH_TABLE_CREATE(Reflection_Constant, 1000);
    reflection_system->reflection_registry_enums = HASH_TABLE_CREATE(Reflection_Enum, 1000);
    reflection_system->reflection_registry_structs = HASH_TABLE_CREATE(Reflection_Struct, 1000);


    u64 mem_size = KB(32);

    reflection_system->allocator = memory_system_allocator_create(memory_system, mem_size, MEMORY_SUBSYSTEM_REFLECTION);
    reflection_system->frame_allocator = memory_system_allocator_create(
        memory_system, mem_size, MEMORY_SUBSYSTEM_REFLECTION);

    reflection_system->header_file_list_capacity = HEADER_FILE_LIST_COUNT;
    reflection_system->header_file_list_count = 0;
    reflection_system->header_file_list = allocator_alloc(reflection_system->allocator,
                                                          sizeof(String*) * reflection_system->
                                                          header_file_list_capacity);

    return reflection_system;
}

void reflection_system_shutdown()
{
    //TODO: use an arena
}

void reflection_system_parse_constants(Reflection_System* reflection_system, Lexer* lexer)
{
    MASSERT(reflection_system);
    MASSERT(lexer);
    DEBUG("CONSTANTS PARSE START")

    u64 token_size = darray_get_size(lexer->tokens);

    for (u64 i = 0; i < token_size; i++)
    {
        if (lexer->tokens[i].type == Token_MACRO)
        {
            if (&lexer->tokens[i + 1] && lexer->tokens[i + 1].type == Token_Identifier)
            {
                if (&lexer->tokens[i + 2] && lexer->tokens[i + 2].type == Token_Number)
                {
                    reflection_system_add_constant(reflection_system,
                                                   string_builder_to_c_string(
                                                       &lexer->tokens[i + 1].string_builder),
                                                   string_builder_to_number(
                                                       &lexer->tokens[i + 2].string_builder));
                    i += 2;
                }
            }
        }
    }
}

void reflection_system_parse_enums(Reflection_System* reflection_system, Lexer* lexer)
{
    MASSERT(reflection_system);
    MASSERT(lexer);
    DEBUG("ENUM PARSE START")

    u64 token_size = darray_get_size(lexer->tokens);

    for (u64 i = 0; i < token_size; i++)
    {
        if (lexer->tokens[i].type == Token_Enum)
        {
            //create a registry with the specific name
            i++; // get the name

            // string_builder_print(&lexer3->tokens[i].string_builder);
            const char* enum_name = string_builder_to_c_string(&lexer->tokens[i].string_builder);
            reflection_system_add_enum(reflection_system, enum_name);

            i++; // offset into the enum structure

            u32 enum_value = 0;
            while (i < token_size && lexer->tokens[i].type != Token_CloseBrace)
            {
                if (lexer->tokens[i].type == Token_Identifier)
                {
                    // string_builder_print(&lexer3->tokens[i].string_builder);
                    reflection_system_add_enum_field(reflection_system, enum_name,
                                                     string_builder_to_c_string(&lexer->tokens[i].string_builder));
                    enum_value++;
                }
                i++;
            }
        }
    }
}

static void reflection_system_parse_fields_for_structs(
    Reflection_System* reflection_system,
    Token* tokens,
    u64* i,
    const char* struct_name,
    Token_Type field_token_type, // the type tag passed to add functions
    const char* type_name_str) // string name of the type
{
    u64 j = *i;

    if (tokens[j + 1].type == Token_Identifier)
    {
        const char* field_name = string_builder_to_c_string(&tokens[j + 1].string_builder);

        if (tokens[j + 2].type == Token_OpenBracket)
        {
            // stack array: type name[SIZE]:
            if (tokens[j + 3].type == Token_Identifier)
            {
                u64 array_size = reflection_system_constant_query(
                    reflection_system,
                    string_builder_to_c_string(&tokens[j + 3].string_builder));
                reflection_system_add_struct_field_ptr_stack(
                    reflection_system, struct_name,
                    field_token_type, type_name_str,
                    field_name, array_size);
                *i += 3;
            }
            else if (tokens[j + 3].type == Token_Number)
            {
                reflection_system_add_struct_field_ptr_stack(
                    reflection_system, struct_name,
                    field_token_type, type_name_str,
                    field_name,
                    string_builder_to_number(&tokens[j + 3].string_builder));
                *i += 3;
            }
        }
        else
        {
            // basic field: type name
            reflection_system_add_struct_field(
                reflection_system, struct_name,
                field_token_type, type_name_str, field_name);
            *i += 1;
        }
    }
    else if (tokens[j + 1].type == Token_Asterisk)
    {
        if (tokens[j + 2].type == Token_Identifier)
        {
            // heap pointer: type* name
            reflection_system_add_struct_field_ptr_heap(
                reflection_system, struct_name,
                field_token_type, type_name_str,
                string_builder_to_c_string(&tokens[j + 2].string_builder));
            *i += 2;
        }
    }
}

void reflection_system_parse_struct(Reflection_System* reflection_system, Lexer* lexer)
{
    MASSERT(reflection_system);
    MASSERT(lexer);
    DEBUG("STRUCT PARSE START");

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
        Token_S8,
        Token_S16,
        Token_S32,
        Token_S64,
        Token_F32,
        Token_F64,
        Token_char,
        Token_size_t,
        Token_bool,

        TOKEN_VEC2,
        TOKEN_VEC3,
        TOKEN_VEC4,
        TOKEN_MAT3,
        TOKEN_MAT4,
        TOKEN_UUID,
    };



    Token* pruned_tokens = lexer_prune_tokens(lexer->tokens, keep_list, ARRAY_SIZE(keep_list));
    u64 pruned_token_list_size = darray_get_size(pruned_tokens);


    for (u64 i = 0; i < pruned_token_list_size; i++)
    {
        if (pruned_tokens[i].type == Token_Struct)
        {
            i++; // move past the struct
            // string_builder_print(&token_list[i].string_builder); // struct name
            const char* struct_name = string_builder_to_c_string(&pruned_tokens[i].string_builder);
            reflection_system_add_struct(reflection_system, struct_name);

            i++; // move past the open bracket
            while (i < pruned_token_list_size && pruned_tokens[i].type != Token_CloseBrace)
            {
                if (lexer_is_token_data_type(pruned_tokens[i]))
                {
                    reflection_system_parse_fields_for_structs(
                        reflection_system, pruned_tokens, &i,
                        struct_name,
                        pruned_tokens[i].type,
                        string_builder_to_c_string(&pruned_tokens[i].string_builder));
                }
                else if (pruned_tokens[i].type == Token_Identifier)
                {
                    const char* type_name = string_builder_to_c_string(&pruned_tokens[i].string_builder);

                    if (reflection_system_does_enum_exist(reflection_system, type_name))
                    {
                        reflection_system_parse_fields_for_structs(
                            reflection_system, pruned_tokens, &i,
                            struct_name, Token_Enum, type_name);
                    }
                    else if (reflection_system_does_struct_exist(reflection_system, type_name))
                    {
                        reflection_system_parse_fields_for_structs(
                            reflection_system, pruned_tokens, &i,
                            struct_name, Token_Struct, type_name);
                    }
                }
                i++;
            }
        }
    }
}

Reflection_Compact_List reflection_system_get_data(Reflection_System* reflection_system)
{
    Reflection_Compact_List list = {0};
    list.constant_list = malloc(
        sizeof(Reflection_Constant) * reflection_system->reflection_registry_constants->num_entries);
    list.constant_list_size = reflection_system->reflection_registry_constants->num_entries;

    list.enum_list = malloc(sizeof(Reflection_Enum) * reflection_system->reflection_registry_enums->num_entries);
    list.enum_list_size = reflection_system->reflection_registry_enums->num_entries;

    list.struct_list = malloc(sizeof(Reflection_Struct) * reflection_system->reflection_registry_structs->num_entries);
    list.struct_list_size = reflection_system->reflection_registry_structs->num_entries;

    u32 constant_count = 0;
    u32 enum_count = 0;
    u32 struct_count = 0;


    for (u64 i = 0; i < reflection_system->reflection_registry_constants->capacity; i++)
    {
        if (!reflection_system->reflection_registry_constants->key_str_data[i]) { continue; }

        Reflection_Constant* reflection_constant = &((Reflection_Constant*)reflection_system->
                                                                           reflection_registry_constants->value_data)[
            i];
        list.constant_list[constant_count] = *reflection_constant;
        constant_count++;
    }

    for (u64 i = 0; i < reflection_system->reflection_registry_enums->capacity; i++)
    {
        if (!reflection_system->reflection_registry_enums->key_str_data[i]) { continue; }
        Reflection_Enum* reflection_enum = &((Reflection_Enum*)reflection_system->reflection_registry_enums->value_data)
            [i];
        list.enum_list[enum_count] = *reflection_enum;
        enum_count++;
    }

    for (u64 i = 0; i < reflection_system->reflection_registry_structs->capacity; i++)
    {
        if (!reflection_system->reflection_registry_structs->key_str_data[i]) { continue; }
        Reflection_Struct* reflection_struct = &((Reflection_Struct*)reflection_system->reflection_registry_structs->
            value_data)[i];
        list.struct_list[struct_count] = *reflection_struct;
        struct_count++;
    }

    return list;
}


void reflection_system_parse(Reflection_System* reflection_system, const char* file_path,
                             const Reflection_Parse_Type parse_type)
{
    Lexer* lexer = lexer_init();
    //so we dont go overboard with our header counts, and that we find header we havne't used yet to include
    if (reflection_system->header_file_list_count <= reflection_system->header_file_list_capacity)
    {
        bool found = false;
        for (u64 i = 0; i < reflection_system->header_file_list_count; i++)
        {
            if (string_compare_c_string(reflection_system->header_file_list[i], file_path))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            reflection_system->header_file_list[reflection_system->header_file_list_count++]
                = string_create_allocator(file_path, strlen(file_path), reflection_system->allocator);
        }
    }
    else
    {
        WARN("reflection_system_parse: increase size of header file capacity")
    }


    lexer_generate_tokens(lexer, file_path);

    switch (parse_type)
    {
    case REFLECTION_PARSE_CONSTANT:
        reflection_system_parse_constants(reflection_system, lexer);
        break;
    case REFLECTION_PARSE_ENUM:
        reflection_system_parse_enums(reflection_system, lexer);
        break;
    case REFLECTION_PARSE_STRUCT:
        reflection_system_parse_struct(reflection_system, lexer);
        break;
    }

    lexer_free(lexer);
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
    reflection_enum.name = c_string_duplicate_allocator(enum_name, reflection_system->allocator);
    hash_table_insert(reflection_system->reflection_registry_enums, enum_name, &reflection_enum);
}

void reflection_system_add_enum_field(Reflection_System* reflection_system, const char* enum_name,
                                      const char* type_field_name)
{
    Reflection_Enum reflection_enum = {0};
    if (hash_table_get(reflection_system->reflection_registry_enums, enum_name, &reflection_enum))
    {
        Reflection_Type_Enum type_info;

        char* type_field_str = c_string_duplicate_allocator(type_field_name, reflection_system->allocator);
        type_info.name = type_field_str;
        type_info.enum_position = reflection_enum.enum_size;

        reflection_enum.enum_size += 1;
        darray_push(reflection_enum.type_list, type_info);


        hash_table_set(reflection_system->reflection_registry_enums,
                       c_string_duplicate_allocator(enum_name, reflection_system->allocator), &reflection_enum);

        reflection_system_add_constant(reflection_system, type_field_str, type_info.enum_position);
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

Reflection_Enum_Query_Array reflection_system_enum_query_list(Reflection_System* reflection_system,
                                                              const char* enum_name, Frame_Allocator* frame_allocator)
{
    //generate the enums first and then the struct data we would want
    Reflection_Enum enum_info = reflection_system_enum_query(reflection_system, enum_name);

    u64 enum_iteration_size = darray_get_size(enum_info.type_list);

    Reflection_Enum_Query_Array query_list = {0};
    query_list.enum_array_sizes = enum_iteration_size;
    query_list.enum_array_names = allocator_alloc(frame_allocator, sizeof(char*) * query_list.enum_array_sizes);

    for (u64 j = 0; j < enum_iteration_size; j++)
    {
        query_list.enum_array_names[j] = enum_info.type_list[j].name;
    }


    return query_list;
}


bool reflection_system_add_struct(Reflection_System* reflection_system, const char* struct_name)
{
    Reflection_Struct reflection_struct = {0};
    reflection_struct.name = c_string_duplicate_allocator(struct_name, reflection_system->allocator);
    reflection_struct.type_list = darray_create_reserve(Reflection_Struct_Field, 1);
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
        Reflection_Struct_Field type_info = {0};
        type_info.field_name = c_string_duplicate_allocator(struct_member_name, reflection_system->allocator);
        type_info.type = Compiler_type_to_Reflection_Type_LUT[reflection_type]; // size is implicit in the type
        type_info.type_name = type_name;
        type_info.container_type = Reflection_Container_Type_VARIABLE;

        darray_push(reflection_struct.type_list, type_info);

        hash_table_set(reflection_system->reflection_registry_structs,
                       struct_name, &reflection_struct);
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
        Reflection_Struct_Field type_info = {0};
        type_info.field_name = struct_member_name;
        type_info.type = Compiler_type_to_Reflection_Type_LUT[reflection_type]; // size is implicit in the type
        type_info.type_name = type_name;
        type_info.container_type = Reflection_Container_Type_POINTER;
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
        Reflection_Struct_Field type_info = {0};
        type_info.field_name = struct_member_name;
        type_info.type = Compiler_type_to_Reflection_Type_LUT[reflection_type]; // size is implicit in the type
        type_info.type_name = type_name;
        type_info.container_type = Reflection_Container_Type_STACK_ARRAY;
        type_info.stack_size = array_size;

        darray_push(reflection_struct.type_list, type_info);

        hash_table_set(reflection_system->reflection_registry_structs, struct_name, &reflection_struct);
        return true;
    }
    MASSERT(false);
    return false;
}

bool reflection_system_set_default_values(Reflection_System* reflection_system, Reflection_Type reflection_type,
    void* data)
{
    MASSERT(false);
    //TODO:
    switch (reflection_type)
    {
    case REFLECTION_TYPE_INVALID:
        break;
    case REFLECTION_TYPE_U8:
        break;
    case REFLECTION_TYPE_U16:
        break;
    case REFLECTION_TYPE_U32:
        break;
    case REFLECTION_TYPE_U64:
        break;
    case REFLECTION_TYPE_S8:
        break;
    case REFLECTION_TYPE_S16:
        break;
    case REFLECTION_TYPE_S32:
        break;
    case REFLECTION_TYPE_S64:
        break;
    case REFLECTION_TYPE_F32:
        break;
    case REFLECTION_TYPE_F64:
        break;
    case REFLECTION_TYPE_SIZE_T:
        break;
    case REFLECTION_TYPE_BOOL:
        break;
    case REFLECTION_TYPE_CHAR:
        break;
    case REFLECTION_TYPE_CHAR_STRING:
        break;
    case REFLECTION_TYPE_STRING:
        break;
    case REFLECTION_TYPE_ENUM:
        break;
    case REFLECTION_TYPE_STRUCT:
        break;
    case REFLECTION_TYPE_VEC2:
        break;
    case REFLECTION_TYPE_VEC3:
        break;
    case REFLECTION_TYPE_VEC4:
        break;
    case REFLECTION_TYPE_MAT3:
        break;
    case REFLECTION_TYPE_MAT4:
        break;
    case REFLECTION_TYPE_UUID:
        break;
    case REFLECTION_TYPE_MAX:
        break;
    }
    return true;

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

Reflection_Struct_Field* reflection_system_generate_struct_offset(Reflection_System* reflection_system,
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

static const char* reflection_type_to_str(Reflection_Type type)
{
    switch (type)
    {
    case REFLECTION_TYPE_U8: return "REFLECTION_TYPE_U8";
    case REFLECTION_TYPE_U16: return "REFLECTION_TYPE_U16";
    case REFLECTION_TYPE_U32: return "REFLECTION_TYPE_U32";
    case REFLECTION_TYPE_U64: return "REFLECTION_TYPE_U64";
    case REFLECTION_TYPE_S8: return "REFLECTION_TYPE_S8";
    case REFLECTION_TYPE_S16: return "REFLECTION_TYPE_S16";
    case REFLECTION_TYPE_S32: return "REFLECTION_TYPE_S32";
    case REFLECTION_TYPE_S64: return "REFLECTION_TYPE_S64";
    case REFLECTION_TYPE_F32: return "REFLECTION_TYPE_F32";
    case REFLECTION_TYPE_F64: return "REFLECTION_TYPE_F64";
    case REFLECTION_TYPE_SIZE_T: return "REFLECTION_TYPE_SIZE_T";
    case REFLECTION_TYPE_BOOL: return "REFLECTION_TYPE_BOOL";
    case REFLECTION_TYPE_STRING: return "REFLECTION_TYPE_STRING";
    case REFLECTION_TYPE_CHAR: return "REFLECTION_TYPE_CHAR";
    case REFLECTION_TYPE_ENUM: return "REFLECTION_TYPE_ENUM";
    case REFLECTION_TYPE_STRUCT: return "REFLECTION_TYPE_STRUCT";
    case REFLECTION_TYPE_VEC2: return "REFLECTION_TYPE_VEC2";
    case REFLECTION_TYPE_VEC3: return "REFLECTION_TYPE_VEC3";
    case REFLECTION_TYPE_VEC4: return "REFLECTION_TYPE_VEC4";
    case REFLECTION_TYPE_MAT3: return "REFLECTION_TYPE_MAT3";
    case REFLECTION_TYPE_MAT4: return "REFLECTION_TYPE_MAT4";
    case REFLECTION_TYPE_UUID: return "REFLECTION_TYPE_UUID";


    default: return "REFLECTION_TYPE_INVALID";
    }
}

void reflection_game_data(Reflection_System* reflection_system)
{
    //TODO: we are not freeing anything and especially nothing using the string_builder to C-string function
    MASSERT(reflection_system);

    reflection_system_parse(reflection_system, "../MadnessPulse/game_constants.h", REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(reflection_system, "../MadnessPulse/game_enums.h", REFLECTION_PARSE_ENUM);
    reflection_system_parse(reflection_system, "../MadnessPulse/game_structs.h", REFLECTION_PARSE_STRUCT);


    Reflection_Compact_List reflection_data = reflection_system_get_data(reflection_system);


    //generate the enums first and then the struct data we would want
    FILE* enum_to_string_lut_file = fopen("../MadnessPulse/game_reflection_enums_generated.h", "w");
    const char* header =
        "#include <stddef.h>\n"
        "#include \"game_constants.h\"\n"
        "#include \"game_enums.h\"\n"
        "#include \"game_structs.h\"\n\n"
        "#include \"runtime_registry.h\"\n\n";

    fwrite(header, strlen(header), 1, enum_to_string_lut_file);


    for (u64 i = 0; i < reflection_data.enum_list_size; i++)
    {
        Reflection_Enum enum_info = reflection_data.enum_list[i];
        if (!enum_info.name) { continue; }

        fwrite("const char* ", strlen("const char* "), 1, enum_to_string_lut_file);
        fwrite(enum_info.name, strlen(enum_info.name), 1, enum_to_string_lut_file);
        fwrite("_enum_string[] = {\n", strlen("_enum_string[] = {\n"), 1, enum_to_string_lut_file);

        u64 enum_iteration_size = darray_get_size(enum_info.type_list);
        for (u64 j = 0; j < enum_iteration_size; j++)
        {
            fwrite("\t[", strlen("\t["), 1, enum_to_string_lut_file);
            fwrite(enum_info.type_list[j].name, strlen(enum_info.type_list[j].name), 1, enum_to_string_lut_file);
            fwrite("]= \"", strlen("]= \""), 1, enum_to_string_lut_file);
            fwrite(enum_info.type_list[j].name, strlen(enum_info.type_list[j].name), 1, enum_to_string_lut_file);
            fwrite("\", \n", strlen("\", \n"), 1, enum_to_string_lut_file);
        }

        fwrite("};\n\n", strlen("};\n\n"), 1, enum_to_string_lut_file);
    }


    fprintf(enum_to_string_lut_file, "void generate_runtime_enums(Reflection_Registry* reflection_registry)\n{\n");

    for (u64 i = 0; i < reflection_data.enum_list_size; i++)
    {
        Reflection_Enum enum_info = reflection_data.enum_list[i];
        if (!enum_info.name) { continue; }

        //this is basically what its writing
        /*void generate_enum_data(Reflection_Registry* reflection_registry)
        {
            const Reflection_Runtime_Enum Resistance_Type_Enum =
            {
                .name = "Resistance_Type",
                .enum_names = Resistance_Type_enum_string_test,
                .count = ARRAY_SIZE(Resistance_Type_enum_string_test),
            };
            reflection_registry_add_enums(reflection_registry, Resistance_Type_Enum);

            //write the next field
        }*/

        fprintf(enum_to_string_lut_file,
                "\tconst Reflection_Runtime_Enum %s_enum =\n"
                "\t{\n"
                "\t\t.name = \"%s\",\n"
                "\t\t.enum_names = %s_enum_string,\n"
                "\t\t.count = ARRAY_SIZE(%s_enum_string),\n"
                "\t};\n"
                "\treflection_registry_add_enums(reflection_registry, %s_enum);\n\n",
                enum_info.name,
                enum_info.name,
                enum_info.name,
                enum_info.name,
                enum_info.name);
    }
    fprintf(enum_to_string_lut_file, "}\n");

    fclose(enum_to_string_lut_file);


    FILE* reflection_offset_file = fopen("../MadnessPulse/game_reflection_struct_generated.h", "w");
    fwrite(header, strlen(header), 1, reflection_offset_file);

    fprintf(reflection_offset_file, "void generate_runtime_structs(Reflection_Registry* reflection_registry)\n{\n");

    for (u64 i = 0; i < reflection_data.struct_list_size; i++)
    {
        Reflection_Struct struct_info = reflection_data.struct_list[i];
        if (!struct_info.name) { continue; }

        u64 field_count = darray_get_size(struct_info.type_list);

        // fields array
        fprintf(reflection_offset_file,
                "\tReflection_Runtime_Struct_Field %s_Fields[] =\n"
                "\t{\n",
                struct_info.name);

        for (u64 j = 0; j < field_count; j++)
        {
            Reflection_Struct_Field field = struct_info.type_list[j];
            if (!field.field_name) { continue; }


            fprintf(reflection_offset_file,
                    "\t\t{\n"
                    "\t\t\t.name = \"%s\",\n"
                    "\t\t\t.type = %s,\n"
                    "\t\t\t.type_name = \"%s\",\n"
                    "\t\t\t.offset = offsetof(%s, %s)\n"
                    "\t\t},\n",
                    field.field_name,
                    reflection_type_to_str(field.type),
                    field.type_name ? field.type_name : "",
                    struct_info.name,
                    field.field_name);
        }

        fprintf(reflection_offset_file, "\t};\n\n");

        // struct info
        fprintf(reflection_offset_file,
                "\t Reflection_Runtime_Struct %s_Runtime_Struct =\n"
                "\t{\n"
                "\t\t.name = \"%s\",\n"
                "\t\t.fields = %s_Fields,\n"
                "\t\t.field_count = %llu,\n"
                "\t\t.struct_size = sizeof(%s)\n"
                "\t};\n\n",
                struct_info.name,
                struct_info.name,
                struct_info.name,
                field_count,
                struct_info.name);

        fprintf(reflection_offset_file,
                "\treflection_registry_add_struct(reflection_registry, %s_Runtime_Struct);\n\n",
                struct_info.name);
    }

    fprintf(reflection_offset_file, "}\n");
    fclose(reflection_offset_file);
}


void reflection_data_to_files(Reflection_System* reflection_system, const char* function_name,
                              const char* generated_enum_file_path, const char* generated_struct_file_path)
{
    //TODO: we are not freeing anything and especially nothing using the string_builder to C-string function
    MASSERT(reflection_system);


    Reflection_Compact_List reflection_data = reflection_system_get_data(reflection_system);


    //generate the enums first and then the struct data we would want
    FILE* enum_to_string_lut_file = fopen(generated_enum_file_path, "w");
    if (!enum_to_string_lut_file)
    {
        MASSERT(false);
    }

    const char* header =
        "#include <stddef.h>\n"
        "#include \"runtime_registry.h\"\n";


    fwrite(header, strlen(header), 1, enum_to_string_lut_file);

    for (u32 i = 0; i < reflection_system->header_file_list_count; i++)
    {
        fwrite("#include \"", strlen("#include \""), 1, enum_to_string_lut_file);

        fwrite(reflection_system->header_file_list[i]->chars, reflection_system->header_file_list[i]->length, 1,
               enum_to_string_lut_file);
        fwrite("\"\n\n", strlen("\"\n\n"), 1, enum_to_string_lut_file);
    }


    for (u64 i = 0; i < reflection_data.enum_list_size; i++)
    {
        Reflection_Enum enum_info = reflection_data.enum_list[i];
        if (!enum_info.name) { continue; }

        fwrite("const char* ", strlen("const char* "), 1, enum_to_string_lut_file);
        fwrite(enum_info.name, strlen(enum_info.name), 1, enum_to_string_lut_file);
        fwrite("_enum_string[] = {\n", strlen("_enum_string[] = {\n"), 1, enum_to_string_lut_file);

        u64 enum_iteration_size = darray_get_size(enum_info.type_list);
        for (u64 j = 0; j < enum_iteration_size; j++)
        {
            fwrite("\t[", strlen("\t["), 1, enum_to_string_lut_file);
            fwrite(enum_info.type_list[j].name, strlen(enum_info.type_list[j].name), 1, enum_to_string_lut_file);
            fwrite("]= \"", strlen("]= \""), 1, enum_to_string_lut_file);
            fwrite(enum_info.type_list[j].name, strlen(enum_info.type_list[j].name), 1, enum_to_string_lut_file);
            fwrite("\", \n", strlen("\", \n"), 1, enum_to_string_lut_file);
        }

        fwrite("};\n\n", strlen("};\n\n"), 1, enum_to_string_lut_file);
    }


    fprintf(enum_to_string_lut_file, "void generate_runtime_enums_");
    fwrite(function_name, strlen(function_name), 1, enum_to_string_lut_file);
    fprintf(enum_to_string_lut_file, "(Reflection_Registry* reflection_registry)\n{\n");

    for (u64 i = 0; i < reflection_data.enum_list_size; i++)
    {
        Reflection_Enum enum_info = reflection_data.enum_list[i];
        if (!enum_info.name) { continue; }

        //this is basically what its writing
        /*void generate_enum_data(Reflection_Registry* reflection_registry)
        {
            const Reflection_Runtime_Enum Resistance_Type_Enum =
            {
                .name = "Resistance_Type",
                .enum_names = Resistance_Type_enum_string_test,
                .count = ARRAY_SIZE(Resistance_Type_enum_string_test),
            };
            reflection_registry_add_enums(reflection_registry, Resistance_Type_Enum);

            //write the next field
        }*/

        fprintf(enum_to_string_lut_file,
                "\tconst Reflection_Runtime_Enum %s_enum =\n"
                "\t{\n"
                "\t\t.name = \"%s\",\n"
                "\t\t.enum_names = %s_enum_string,\n"
                "\t\t.count = ARRAY_SIZE(%s_enum_string),\n"
                "\t};\n"
                "\treflection_registry_add_enums(reflection_registry, %s_enum);\n\n",
                enum_info.name,
                enum_info.name,
                enum_info.name,
                enum_info.name,
                enum_info.name);
    }
    fprintf(enum_to_string_lut_file, "}\n");

    fclose(enum_to_string_lut_file);


    FILE* reflection_offset_file = fopen(generated_struct_file_path, "w");
    if (!reflection_offset_file)
    {
        MASSERT(false);
    }


    fwrite(header, strlen(header), 1, reflection_offset_file);

    for (u32 i = 0; i < reflection_system->header_file_list_count; i++)
    {
        fwrite("#include \"", strlen("#include \""), 1, enum_to_string_lut_file);

        fwrite(reflection_system->header_file_list[i]->chars, reflection_system->header_file_list[i]->length, 1,
               enum_to_string_lut_file);
        fwrite("\"\n\n", strlen("\"\n\n"), 1, enum_to_string_lut_file);
    }


    fprintf(reflection_offset_file, "void generate_runtime_structs_");
    fwrite(function_name, strlen(function_name), 1, reflection_offset_file);
    fprintf(enum_to_string_lut_file, "(Reflection_Registry* reflection_registry)\n{\n");


    for (u64 i = 0; i < reflection_data.struct_list_size; i++)
    {
        Reflection_Struct struct_info = reflection_data.struct_list[i];
        if (!struct_info.name) { continue; }

        u64 field_count = darray_get_size(struct_info.type_list);

        // fields array
        fprintf(reflection_offset_file,
                "\tReflection_Runtime_Struct_Field %s_Fields[] =\n"
                "\t{\n",
                struct_info.name);

        for (u64 j = 0; j < field_count; j++)
        {
            Reflection_Struct_Field field = struct_info.type_list[j];
            if (!field.field_name) { continue; }


            fprintf(reflection_offset_file,
                    "\t\t{\n"
                    "\t\t\t.name = \"%s\",\n"
                    "\t\t\t.type = %s,\n"
                    "\t\t\t.type_name = \"%s\",\n"
                    "\t\t\t.offset = offsetof(%s, %s)\n"
                    "\t\t},\n",
                    field.field_name,
                    reflection_type_to_str(field.type),
                    field.type_name ? field.type_name : "",
                    struct_info.name,
                    field.field_name);
        }

        fprintf(reflection_offset_file, "\t};\n\n");

        // struct info
        fprintf(reflection_offset_file,
                "\t Reflection_Runtime_Struct %s_Runtime_Struct =\n"
                "\t{\n"
                "\t\t.name = \"%s\",\n"
                "\t\t.fields = %s_Fields,\n"
                "\t\t.field_count = %llu,\n"
                "\t\t.struct_size = sizeof(%s)\n"
                "\t};\n\n",
                struct_info.name,
                struct_info.name,
                struct_info.name,
                field_count,
                struct_info.name);

        fprintf(reflection_offset_file,
                "\treflection_registry_add_struct(reflection_registry, %s_Runtime_Struct);\n\n",
                struct_info.name);
    }

    fprintf(reflection_offset_file, "}\n");
    fclose(reflection_offset_file);
}
