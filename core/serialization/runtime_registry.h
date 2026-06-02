#ifndef DATA_REGISTRY_H
#define DATA_REGISTRY_H
#include <stdbool.h>

#include "defines.h"
#include "compiler/reflection_system.h"


typedef struct Reflection_Runtime_Constants
{
    const char* name;
    Reflection_Type type;
    size_t offset;
} Reflection_Runtime_Constants;


typedef struct Reflection_Runtime_Enum
{
    const char* name;
    const char** enum_names;
    u32 count;
} Reflection_Runtime_Enum;

typedef struct Reflection_Runtime_Struct_Field
{
    const char* name;
    const char* type_name;
    Reflection_Type type;
    u32 offset;
} Reflection_Runtime_Struct_Field;


typedef struct Reflection_Runtime_Struct
{
    const char* name;
    Reflection_Runtime_Struct_Field* fields;
    u32 field_count;
    u32 struct_size;
} Reflection_Runtime_Struct;

// meant to be used for the ui rn
typedef struct Reflection_Runtime_Data
{
    const char* identifier;
    void* data;
    u32 struct_index_reference;
    // u32 dynamic_array_index;
} Reflection_Runtime_Data;


typedef struct Reflection_Runtime_Meta_File
{
    String string;
} Reflection_Runtime_Meta_File;


#define Reflection_Runtime_Version 1
#define Reflection_Runtime_MAGIC_NUMBER 643651231 // literally just random numbers
#define Reflection_Runtime_Meta_Data_File_Path "../z_assets/abilities/reflection_meta_data.bin"

typedef struct Reflection_Runtime_Meta_File_Header
{
    u32 version;
    u32 magic_number;
    u32 data_count;
} Reflection_Runtime_Meta_File_Header;


//?? this should all just be part of the reflection system now that im looking at it


typedef struct Reflection_Registry
{
    DYNAMIC_ARRAY_TYPE(Reflection_Runtime_Enum)* enum_list;

    DYNAMIC_ARRAY_TYPE(Reflection_Runtime_Struct)* struct_list;


    DYNAMIC_ARRAY_TYPE(Reflection_Runtime_Data)* runtime_data;
    DYNAMIC_ARRAY_TYPE(Reflection_Runtime_Data_MetaFile)* meta_file_data;

    // TODO: change to a free list or even a pool allocator
    Allocator* allocator;
    Allocator_Interface allocator_interface;


    //basically we want to just store structs in here
} Reflection_Registry;


Reflection_Registry* reflection_registry_init(Memory_System* memory_system)
{
    Reflection_Registry* reflection_registry = (Reflection_Registry*)malloc(sizeof(Reflection_Registry));


    u64 mem_size = MB(16);


    reflection_registry->allocator = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_REFLECTION);

    void* alloc_memory = memory_system_alloc(memory_system, mem_size, MEMORY_SUBSYSTEM_REFLECTION);
    allocator_init(reflection_registry->allocator, alloc_memory, mem_size);

    reflection_registry->allocator_interface = allocator_inferface_create(reflection_registry->allocator);


    reflection_registry->enum_list = dynamic_array_create(Reflection_Runtime_Enum, 100,
                                                          reflection_registry->allocator_interface);
    reflection_registry->struct_list = dynamic_array_create(Reflection_Runtime_Struct, 100,
                                                            reflection_registry->allocator_interface);

    reflection_registry->runtime_data = dynamic_array_create(Reflection_Runtime_Data, 1000,
                                                             reflection_registry->allocator_interface);
    reflection_registry->meta_file_data = dynamic_array_create(Reflection_Runtime_Meta_File, 100,
                                                               reflection_registry->allocator_interface);

    return reflection_registry;
}

void reflection_registry_load_meta_data(Reflection_Registry* reflection_registry, const char* file_path)
{
    FILE* fptr = fopen(file_path, "rb");

    if (!fptr)
    {
        WARN("reflection_registry_load_meta_data: unable to open file")
        return;
    }

    Reflection_Runtime_Meta_File_Header file_header;
    fread(&file_header, sizeof(Reflection_Runtime_Meta_File_Header), 1, fptr);
    if (file_header.magic_number != (u32)Reflection_Runtime_MAGIC_NUMBER)
    {
        WARN("reflection_registry_load_meta_data: invalid magic number")
        return;
    }
    for (u32 i = 0; i < file_header.data_count; i++)
    {
        String read_in_string;
        fread(&read_in_string.length, sizeof(read_in_string.length), 1, fptr);
        read_in_string.chars = allocator_alloc(reflection_registry->allocator, read_in_string.length);
        fread(read_in_string.chars, read_in_string.length, 1, fptr);
        dynamic_array_push(reflection_registry->meta_file_data, &read_in_string);
    }

    //do the actual load




    fclose(fptr);
}

void reflection_registry_save_meta_data(Reflection_Registry* reflection_registry, const char* file_path)
{
    FILE* fptr = fopen(file_path, "wb");

    if (!fptr)
    {
        WARN("reflection_registry_save_meta_data: unable to open file")
        return;
    }


    Reflection_Runtime_Meta_File_Header file_header = {
        .version = Reflection_Runtime_Version,
        .magic_number = (u32)Reflection_Runtime_MAGIC_NUMBER,
        .data_count = reflection_registry->meta_file_data->num_items,
    };
    fwrite(&file_header, sizeof(Reflection_Runtime_Meta_File_Header), 1, fptr);


    for (int i = 0; i < reflection_registry->meta_file_data->num_items; ++i)
    {
        Reflection_Runtime_Meta_File data = dynamic_array_get(reflection_registry->meta_file_data,
                                                              Reflection_Runtime_Meta_File, i);
        fwrite(&data.string.length, sizeof(data.string.length), 1, fptr);
        fwrite(data.string.chars, data.string.length, 1, fptr);
    }

    fclose(fptr);
}


void reflection_registry_shutdown(Reflection_Registry* reflection_registry)
{
    free(reflection_registry);
}

void reflection_registry_add_enums(Reflection_Registry* reflection_registry, Reflection_Runtime_Enum reflection_enum)
{
    dynamic_array_push(reflection_registry->enum_list, &reflection_enum);
}


Reflection_Runtime_Enum reflection_registry_get_enum(Reflection_Registry* reflection_registry,
                                                     const char* enum_name)
{
    for (u32 enum_index = 0; enum_index < reflection_registry->enum_list->num_items; enum_index++)
    {
        Reflection_Runtime_Enum runtime_enum = dynamic_array_get(reflection_registry->enum_list,
                                                                 Reflection_Runtime_Enum, enum_index);

        if (strcmp(runtime_enum.name, enum_name) == 0)
        {
            return runtime_enum;
        }
    }

    MASSERT(false);
    return (Reflection_Runtime_Enum){0};
}

void reflection_registry_add_struct(Reflection_Registry* reflection_registry,
                                    Reflection_Runtime_Struct reflection_struct)
{
    //copy the data because it will go off the stack when we are done
    Reflection_Runtime_Struct_Field* fields_copy = reflection_registry->allocator_interface.alloc(
        reflection_registry->allocator_interface.allocator,
        sizeof(Reflection_Runtime_Struct_Field) * reflection_struct.field_count,
        DEFAULT_ALIGNMENT
    );
    memcpy(fields_copy, reflection_struct.fields,
           sizeof(Reflection_Runtime_Struct_Field) * reflection_struct.field_count);
    reflection_struct.fields = fields_copy; // set the heap allocated data

    dynamic_array_push(reflection_registry->struct_list, &reflection_struct);
}


Reflection_Runtime_Struct reflection_registry_get_struct(Reflection_Registry* reflection_registry,
                                                         const char* struct_name)
{
    for (u32 struct_index = 0; struct_index < reflection_registry->struct_list->num_items; struct_index++)
    {
        Reflection_Runtime_Struct runtime_struct = dynamic_array_get(reflection_registry->struct_list,
                                                                     Reflection_Runtime_Struct, struct_index);

        if (strcmp(runtime_struct.name, struct_name) == 0)
        {
            return runtime_struct;
        }
    }

    MASSERT(false);
    return (Reflection_Runtime_Struct){0};
}

//create the data if not found
Reflection_Runtime_Data reflection_registry_get_or_create_runtime_data(Reflection_Registry* reflection_registry,
                                                                       const char* struct_name, const char* identifier)
{
    Scratch_Allocator scratch_allocator = scratch_allocator_begin(reflection_registry->allocator);
    const char* struct_identifier = c_string_concat(struct_name, identifier, scratch_allocator.allocator);

    //typically this will be used in local scope, and freed off the stack when done,
    //so its recommended that it's not a pointer

    Reflection_Runtime_Data runtime_data = {0};
    for (u32 i = 0; i < reflection_registry->runtime_data->num_items; i++)
    {
        runtime_data = dynamic_array_get(reflection_registry->runtime_data, Reflection_Runtime_Data, i);
        if (strcmp(runtime_data.identifier, struct_identifier) == 0)
        {
            scratch_allocator_end(scratch_allocator);
            return runtime_data;
        }
    }
    scratch_allocator_end(scratch_allocator);


    //if we get here that means the data does not exist so we create it
    Reflection_Runtime_Struct runtime_struct = {0};
    u32 found_index = INT_MAX;
    for (u32 struct_index = 0; struct_index < reflection_registry->struct_list->num_items; struct_index++)
    {
        runtime_struct = dynamic_array_get(reflection_registry->struct_list,
                                           Reflection_Runtime_Struct, struct_index);

        if (strcmp(runtime_struct.name, struct_name) == 0)
        {
            found_index = struct_index;
            break;
        }
    }

    MASSERT(found_index != INT_MAX);

    Reflection_Runtime_Data new_runtime_data;
    new_runtime_data.data = reflection_registry->allocator_interface.alloc(
        reflection_registry->allocator_interface.allocator, runtime_struct.struct_size, DEFAULT_ALIGNMENT);
    new_runtime_data.identifier = c_string_concat(struct_name, identifier, reflection_registry->allocator);
    new_runtime_data.struct_index_reference = found_index;
    dynamic_array_push(reflection_registry->runtime_data, &new_runtime_data);

    return new_runtime_data;
}

Reflection_Runtime_Struct reflection_registry_get_runtime_data_struct(Reflection_Registry* reflection_registry,
                                                                      Reflection_Runtime_Data runtime_data)
{
    return dynamic_array_get(reflection_registry->struct_list, Reflection_Runtime_Struct,
                             runtime_data.struct_index_reference);
}


Reflection_Type reflection_registry_str_to_type(const char* str)
{
    if (strcmp(str, "bool") == 0)
    {
        return REFLECTION_TYPE_BOOL;
    }
    if (strcmp(str, "char") == 0)
    {
        return REFLECTION_TYPE_CHAR;
    }
    if (strcmp(str, "str") == 0)
    {
        //TODO: special case, handle it differently
        return REFLECTION_TYPE_STRING;
    }
    if (strcmp(str, "s32") == 0)
    {
        return REFLECTION_TYPE_S32;
    }
    if (strcmp(str, "u32") == 0)
    {
        return REFLECTION_TYPE_U32;
    }
    if (strcmp(str, "f32") == 0 || strcmp(str, "float") == 0)
    {
        return REFLECTION_TYPE_F32;
    }
    if (strcmp(str, "enum") == 0)
    {
        return REFLECTION_TYPE_ENUM;
    }

    return 0;
}


void reflection_registry_to_txt_format(Reflection_Registry* reflection_registry, const char* struct_name,
                                       const char* identifier, void* struct_data, const char* file_name)
{
    //check for a matching file first, if nothing, then create a new file
    // then load in the data/default data

    FILE* file = fopen(file_name, "w");

    for (u32 struct_index = 0; struct_index < reflection_registry->struct_list->num_items; struct_index++)
    {
        const Reflection_Runtime_Struct runtime_struct = dynamic_array_get(reflection_registry->struct_list,
                                                                           Reflection_Runtime_Struct, struct_index);

        //check if we found the correct struct
        if (strcmp(runtime_struct.name, struct_name) != 0) { continue; }

        fprintf(file, "#%s%s\n", struct_name, identifier);

        for (u32 field_index = 0; field_index < runtime_struct.field_count; field_index++)
        {
            Reflection_Runtime_Struct_Field field = runtime_struct.fields[field_index];
            fprintf(file, "%s", field.name);
            //write out the type
            switch (field.type)
            {
            case REFLECTION_TYPE_INVALID:
                break;
            case REFLECTION_TYPE_BOOL:
                fwrite("[bool]", strlen("[bool]"), 1, file);
                break;
            case REFLECTION_TYPE_U8:
                fwrite("[u8]", strlen("[u8]"), 1, file);
                break;
            case REFLECTION_TYPE_U16:
                fwrite("[u16]", strlen("[u16]"), 1, file);
                break;
            case REFLECTION_TYPE_U32:
                fwrite("[u32]", strlen("[u32]"), 1, file);
                break;
            case REFLECTION_TYPE_U64:
                fwrite("[u64]", strlen("[u64]"), 1, file);
                break;
            case REFLECTION_TYPE_S8:
                fwrite("[s8]", strlen("[s8]"), 1, file);
                break;
            case REFLECTION_TYPE_S16:
                fwrite("[s16]", strlen("[s16]"), 1, file);
                break;
            case REFLECTION_TYPE_S32:
                fwrite("[s32]", strlen("[s32]"), 1, file);
                break;
            case REFLECTION_TYPE_S64:
                fwrite("[s64]", strlen("[s64]"), 1, file);
                break;
            case REFLECTION_TYPE_F32:
                fwrite("[f32]", strlen("[f32]"), 1, file);
                break;
            case REFLECTION_TYPE_F64:
                fwrite("[f64]", strlen("[f64]"), 1, file);
                break;
            case REFLECTION_TYPE_SIZE_T:
                fwrite("[size_t]", strlen("[size_t]"), 1, file);
                break;
            case REFLECTION_TYPE_CHAR:
                fwrite("[char]", strlen("[char]"), 1, file);
                break;
            case REFLECTION_TYPE_STRING:
                fwrite("[str]", strlen("[str]"), 1, file);
                break;
            case REFLECTION_TYPE_ENUM:
                fwrite("[enum]", strlen("[enum]"), 1, file);
                break;
            case REFLECTION_TYPE_STRUCT:
                fwrite("[struct]", strlen("[struct]"), 1, file);
                break;
            case REFLECTION_TYPE_CHAR_STRING:
                fwrite("[char*]", strlen("[char*]"), 1, file);
                break;
            case REFLECTION_TYPE_MAX:
                break;
            }

            fwrite("[", strlen("["), 1, file);
            //TODO: get the proper array count
            /*
            char str[64];
            sprintf(str, "%d", cur->array_count);
            fwrite(str, strlen(str), 1, file);
            */
            fwrite("1", strlen("1"), 1, file); // TODO: remove with the above
            fwrite("]", strlen("]"), 1, file);
            fwrite(":\n", strlen(":\n"), 1, file);


            void* data = (u8*)struct_data + field.offset;

            switch (field.type)
            {
            case REFLECTION_TYPE_INVALID:
                break;
            case REFLECTION_TYPE_U8:
                fprintf(file, "- %d", *(u8*)data);
                break;
            case REFLECTION_TYPE_U16:
                fprintf(file, "- %d", *(u16*)data);
                break;
            case REFLECTION_TYPE_U32:
                fprintf(file, "- %d", *(u32*)data);
                break;
            case REFLECTION_TYPE_U64:
                fprintf(file, "- %llu", *(u64*)data);
                break;
            case REFLECTION_TYPE_S8:
                fprintf(file, "- %d", *(s8*)data);
                break;
            case REFLECTION_TYPE_S16:
                fprintf(file, "- %d", *(s16*)data);
                break;
            case REFLECTION_TYPE_S32:
                fprintf(file, "- %d", *(s32*)data);
                break;
            case REFLECTION_TYPE_S64:
                fprintf(file, "- %lld", *(s64*)data);
                break;
            case REFLECTION_TYPE_F32:
                fprintf(file, "- %f", *(f32*)data);
                break;
            case REFLECTION_TYPE_F64:
                fprintf(file, "- %f", *(f64*)data);
                break;
            case REFLECTION_TYPE_SIZE_T:
                fprintf(file, "- %llu", *(size_t*)data);
                break;
            case REFLECTION_TYPE_BOOL:
                if (*(bool*)(data))
                {
                    fwrite("- true", strlen("- true"), 1, file);
                }
                else
                {
                    fwrite("- false", strlen("- false"), 1, file);
                }
                break;
            case REFLECTION_TYPE_CHAR:
                fwrite(data, sizeof(char), 1, file);
                break;
            case REFLECTION_TYPE_STRING:
                const String* string_data = data;
                fwrite(string_data->chars, string_data->length, 1, file);
                break;
            case REFLECTION_TYPE_CHAR_STRING:
                fwrite(data, strlen(data), 1, file);
                break;
            case REFLECTION_TYPE_ENUM:
                fprintf(file, "- %d", *(u32*)data);
                break;
            case REFLECTION_TYPE_STRUCT:
                break;
            case REFLECTION_TYPE_MAX:
                break;
            }
            fprintf(file, "\n");
        }
        break;
    }
    fclose(file);
}

void reflection_registry_read_from_txt_format(Reflection_Registry* reflection_registry, const char* struct_name,
                                              const char* identifier, void* out_struct_data, const char* file_name)
{
    //check for the file, if nothing, then create a new file
    // then load in the data/default data

    //find the struct
    bool found = false;
    Reflection_Runtime_Struct runtime_struct = {0};
    for (u32 struct_index = 0; struct_index < reflection_registry->struct_list->num_items; struct_index++)
    {
        runtime_struct = dynamic_array_get(reflection_registry->struct_list, Reflection_Runtime_Struct, struct_index);
        if (strcmp(runtime_struct.name, struct_name) == 0)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        MASSERT(false);
        return;
    }


    FILE* fptr = fopen(file_name, "r");
    fseek(fptr, 0L, SEEK_END);
    size_t file_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    u8* buffer = malloc(file_size);
    fread(buffer, file_size, 1, fptr);
    DEBUG("%s", (char*)buffer);

    //we want to find the identifier we are looking for

    //TODO: Free
    const char* hash_struct = c_string_concat("#", struct_name, NULL);
    const char* struct_identifier = c_string_concat(hash_struct, identifier, NULL);


    char* line = strtok((char*)buffer, "\n");
    while (line)
    {
        if (line[0] != '#')
        {
            line = strtok(NULL, "\n");
            continue;
        }

        if (strcmp(line, struct_identifier) != 0)
        {
            line = strtok(NULL, "\n");
            continue;
        }
        line = strtok(NULL, "\n");

        for (u32 field_index = 0; field_index < runtime_struct.field_count; field_index++)
        {
            char field_name[128];
            char value_str[32];
            char array_count[32];
            //scans for our format
            // arr: field[type][N]:
            if (sscanf_s(line, "%127[^[][%31[^]]][%31[^]]:",
                         field_name, (unsigned)sizeof(field_name),
                         value_str, (unsigned)sizeof(value_str),
                         array_count, (unsigned)sizeof(array_count)) == 3)
            {
                DEBUG("arr");
                DEBUG("field name: %s", field_name);
                DEBUG("type: %s", value_str);
                DEBUG("arr length: %s", array_count);

                Reflection_Runtime_Struct_Field field = runtime_struct.fields[field_index];
                if (strcmp(field.name, field_name) != 0)
                {
                    line = strtok(NULL, "\n");
                    continue;
                }

                //move to the data line
                line = strtok(NULL, "\n");
                if (line[0] != '-') { MASSERT(false); }
                int i = 1;
                char field_data[128];
                u8 write_count = 0;
                //skip white space
                while (line[i] == ' ')
                {
                    i++;
                }
                //read in the data
                while (line[i] != '\n' && line[i] != '\0')
                {
                    field_data[write_count++] = line[i];
                    i++;
                }
                field_data[write_count] = '\0';
                DEBUG("field data arr: %s", field_data);


                void* data = (u8*)out_struct_data + field.offset;
                switch (field.type)
                {
                case REFLECTION_TYPE_INVALID:
                    break;
                case REFLECTION_TYPE_U8:
                    u8 u8_num = c_string_to_number(field_data, strlen(field_data));
                    memcpy(data, &u8_num, sizeof(u8));
                    break;
                case REFLECTION_TYPE_U16:
                    u16 u16_num = c_string_to_number(field_data, strlen(field_data));
                    memcpy(data, &u16_num, sizeof(u16));
                    break;
                case REFLECTION_TYPE_U32:
                    u32 u_num = c_string_to_number(field_data, strlen(field_data));
                    memcpy(data, &u_num, sizeof(u32));
                    break;
                case REFLECTION_TYPE_U64:
                    u64 u64_num = c_string_to_number(field_data, strlen(field_data));
                    memcpy(data, &u64_num, sizeof(u64));
                    break;
                case REFLECTION_TYPE_S8:
                    s8 s8_num = c_string_to_number(field_data, strlen(field_data));
                    memcpy(data, &s8_num, sizeof(s8));
                    break;
                case REFLECTION_TYPE_S16:
                    s16 s16_num = c_string_to_number(field_data, strlen(field_data));
                    memcpy(data, &s16_num, sizeof(s16));
                    break;
                case REFLECTION_TYPE_S32:
                    s32 s32_num = c_string_to_number(field_data, strlen(field_data));
                    memcpy(data, &s32_num, sizeof(s32));
                    break;
                case REFLECTION_TYPE_S64:
                    s64 s64_num = c_string_to_number(field_data, strlen(field_data));
                    memcpy(data, &s64_num, sizeof(s64));
                    break;
                case REFLECTION_TYPE_F32:
                    f32 f_num = c_string_to_float(field_data);
                    memcpy(data, &f_num, sizeof(f32));
                    break;
                case REFLECTION_TYPE_F64:
                    f64 f64_num = c_string_to_float(field_data);
                    memcpy(data, &f64_num, sizeof(f64));
                    break;
                case REFLECTION_TYPE_SIZE_T:
                    memcpy(data, value_str, sizeof(size_t));
                    break;
                case REFLECTION_TYPE_BOOL:
                    if (strcmp(field_data, "true") == 0)
                    {
                        bool t = true;
                        memcpy(data, &t, sizeof(bool));
                    }
                    else if (strcmp(field_data, "false") == 0)
                    {
                        bool f = false;
                        memcpy(data, &f, sizeof(bool));
                    }
                    break;
                case REFLECTION_TYPE_CHAR:
                    memcpy(data, value_str, sizeof(char));
                    break;
                case REFLECTION_TYPE_CHAR_STRING:
                    break;
                case REFLECTION_TYPE_STRING:
                    break;
                case REFLECTION_TYPE_ENUM:
                    u32 enum_num = (u32)c_string_to_number(field_data, strlen(field_data));
                    memcpy(data, &enum_num, sizeof(u32));
                    break;
                case REFLECTION_TYPE_STRUCT:
                    break;
                case REFLECTION_TYPE_MAX:
                    break;
                }
                line = strtok(NULL, "\n");
            }
        }
    }

    fclose(fptr);
}

void reflection_registry_runtime_load_data_from_txt(Reflection_Registry* reflection_registry)
{
    for (int i = 0; i < reflection_registry->runtime_data->num_items; ++i)
    {
        Reflection_Runtime_Data runtime_data = dynamic_array_get(reflection_registry->runtime_data,
                                                                 Reflection_Runtime_Data, i);

        Reflection_Runtime_Struct runtime_struct = reflection_registry_get_runtime_data_struct(
            reflection_registry, runtime_data);

        Scratch_Allocator scratch_allocator = scratch_allocator_begin(reflection_registry->allocator);

        const char* replace_later_path = "../z_assets/abilities/";
        const char* intermediate_file_path = c_string_concat(replace_later_path, runtime_data.identifier,
                                                             scratch_allocator.allocator);
        const char* final_file_path = c_string_concat(intermediate_file_path, ".yaml", scratch_allocator.allocator);

        reflection_registry_to_txt_format(reflection_registry, runtime_struct.name,
                                          runtime_data.identifier, runtime_data.data, final_file_path);

        scratch_allocator_end(scratch_allocator);
    }
}

void reflection_registry_runtime_serialize_all_data_to_txt_format(Reflection_Registry* reflection_registry)
{
    for (int i = 0; i < reflection_registry->runtime_data->num_items; ++i)
    {
        Reflection_Runtime_Data runtime_data = dynamic_array_get(reflection_registry->runtime_data,
                                                                 Reflection_Runtime_Data, i);

        Reflection_Runtime_Struct runtime_struct = reflection_registry_get_runtime_data_struct(
            reflection_registry, runtime_data);

        Scratch_Allocator scratch_allocator = scratch_allocator_begin(reflection_registry->allocator);

        const char* replace_later_path = "../z_assets/abilities/";
        const char* intermediate_file_path = c_string_concat(replace_later_path, runtime_data.identifier,
                                                             scratch_allocator.allocator);
        const char* final_file_path = c_string_concat(intermediate_file_path, ".yaml", scratch_allocator.allocator);

        reflection_registry_to_txt_format(reflection_registry, runtime_struct.name,
                                          runtime_data.identifier, runtime_data.data, final_file_path);

        Reflection_Runtime_Meta_File meta_file = {.string = STRING_STRLEN(final_file_path)};

        // TODO: this is causing duplicates
        // also load in the .yaml data, which means splitting the
        dynamic_array_push(reflection_registry->meta_file_data, &meta_file);

        scratch_allocator_end(scratch_allocator);
    }


    reflection_registry_save_meta_data(reflection_registry, Reflection_Runtime_Meta_Data_File_Path);
}


#endif //DATA_REGISTRY_H
