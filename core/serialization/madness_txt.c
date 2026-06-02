#include "madness_txt.h"

#include "c_string.h"


Madness_txt* madness_txt_init(Allocator_Interface* allocator_interface)
{
    Madness_txt* system = (Madness_txt*)malloc(sizeof(Madness_txt));
    system->object_count = 0;
    memset(system->objects, 0, sizeof(madness_txt_object) * MADNESS_TXT_OBJECT_MAX);


    return system;
}

void madness_txt_shutdown(Madness_txt* txt)
{
}


object_handle madness_txt_object_create(Madness_txt* txt, const char* name)
{
    madness_txt_object* object = &txt->objects[txt->object_count++];
    object->name = name;
    return (object_handle){txt->object_count - 1};
}


void madness_txt_object_add_item(Madness_txt* txt, object_handle handle, const char* field_name, Reflection_Type type,
                                 u32 array_size)
{
    madness_txt_object_subfield* new_subfield = malloc(sizeof(madness_txt_object_subfield));
    new_subfield->field_name = field_name;
    new_subfield->next = NULL;

    new_subfield->type = type;
    new_subfield->array_count = array_size;
    new_subfield->data = NULL;

    madness_txt_object* object = &txt->objects[handle.handle];

    if (!object->head)
    {
        object->head = new_subfield;
        object->last = new_subfield;
    }
    else
    {
        madness_txt_object_subfield* subfield_temp = object->last;
        subfield_temp->next = new_subfield;
        object->last = new_subfield;
    }
}

void madness_txt_object_write_data_single(Madness_txt* txt, object_handle handle, const char* field_name,
                                   void* data)
{
    madness_txt_object* object = &txt->objects[handle.handle];
    madness_txt_object_subfield* cur = object->head;

    while (cur)
    {
        if (strcmp(cur->field_name, field_name) != 0)
        {
            cur = cur->next;
            continue;
        }
        size_t type_size = madness_txt_type_size(cur->type);

        if (cur->type == REFLECTION_TYPE_STRING)
        {
            const char** data_strings = (const char**)data;
            if (!cur->string)
            {
                cur->string = malloc(sizeof(String*) * cur->array_count);
            }
            for (int i = 0; i < cur->array_count; ++i)
            {
                cur->string[i].length = strlen(data_strings[i]);
                cur->string[i].chars = strdup(data_strings[i]);
            }
        }
        else
        {
            if (!cur->data)
            {
                cur->data = malloc(type_size * cur->array_count);
            }

            for (int i = 0; i < cur->array_count; ++i)
            {
                memcpy((u8*)cur->data + (type_size * i), (u8*)data + (type_size * i), type_size);
            }
        }


        return;
    }
}

void madness_txt_object_write_data_full(Madness_txt* txt, object_handle handle, void* struct_data)
{
    /*madness_txt_object* object = &txt->objects[handle.handle];
    madness_txt_object_subfield* cur = object->head;

    u64 offset;

    while (cur)
    {
        size_t type_size = madness_txt_type_size(cur->type);

        if (cur->type == REFLECTION_TYPE_STRING)
        {
            const char** data_strings = (const char**)struct_data->data;
            if (!cur->string)
            {
                cur->string = malloc(sizeof(String*) * cur->array_count);
            }
            for (int i = 0; i < cur->array_count; ++i)
            {
                cur->string[i].length = strlen(data_strings[i]);
                cur->string[i].chars = strdup(data_strings[i]);
            }
        }
        else
        {
            if (!cur->data)
            {
                cur->data = malloc(type_size * cur->array_count);
            }

            for (int i = 0; i < cur->array_count; ++i)
            {
                memcpy((u8*)cur->data + (type_size * i), (u8*)data + (type_size * i), type_size);
            }
        }


        return;
    }*/
}

void madness_txt_object_write_data_arg(Madness_txt* txt, object_handle handle, ...)
{
    va_list args;
    va_start(args, handle);

    madness_txt_object* object = &txt->objects[handle.handle];
    madness_txt_object_subfield* cur = object->head;

    while (cur)
    {
        void* data = va_arg(args, void*);
        size_t type_size = madness_txt_type_size(cur->type);

        if (cur->type == REFLECTION_TYPE_STRING)
        {
            const char** data_strings = (const char**)data;
            if (!cur->string)
            {
                cur->string = malloc(sizeof(String) * cur->array_count);
            }
            for (int i = 0; i < cur->array_count; ++i)
            {
                cur->string[i].length = strlen(data_strings[i]);
                cur->string[i].chars = strdup(data_strings[i]);
            }
        }
        else
        {
            if (!cur->data)
            {
                cur->data = malloc(type_size * cur->array_count);
            }
            memcpy(cur->data, data, type_size * cur->array_count);
        }

        cur = cur->next;
    }

    va_end(args);
}


void madness_txt_object_write_file(Madness_txt* txt, const char* file_path, object_handle* object_handle)
{
    FILE* fptr = fopen(file_path, "w");

    madness_txt_object* object = &txt->objects[object_handle->handle];
    madness_txt_object_subfield* cur = object->head;

    fwrite("#", strlen("#"), 1, fptr);
    fwrite(object->name, strlen(object->name), 1, fptr);
    fwrite("\n", strlen("\n"), 1, fptr);


    while (cur)
    {
        fwrite(cur->field_name, strlen(cur->field_name), 1, fptr);

        //write out the type
        switch (cur->type)
        {
        case REFLECTION_TYPE_BOOL:
            fwrite("[bool]", strlen("[bool]"), 1, fptr);
            break;
        case REFLECTION_TYPE_S32:
            fwrite("[s32]", strlen("[s32]"), 1, fptr);
            break;
        case REFLECTION_TYPE_U32:
            fwrite("[u32]", strlen("[u32]"), 1, fptr);
            break;
        case REFLECTION_TYPE_F32:
            fwrite("[f32]", strlen("[f32]"), 1, fptr);
            break;
        case REFLECTION_TYPE_CHAR:
            fwrite("[char]", strlen("[char]"), 1, fptr);
            break;
        case REFLECTION_TYPE_STRING:
            fwrite("[str]", strlen("[str]"), 1, fptr);
            break;
        case REFLECTION_TYPE_INVALID:
            break;
        case REFLECTION_TYPE_U8:
            break;
        case REFLECTION_TYPE_U16:
            break;
        case REFLECTION_TYPE_U64:
            break;
        case REFLECTION_TYPE_S8:
            break;
        case REFLECTION_TYPE_S16:
            break;
        case REFLECTION_TYPE_S64:
            break;
        case REFLECTION_TYPE_F64:
            break;
        case REFLECTION_TYPE_SIZE_T:
            break;
        case REFLECTION_TYPE_ENUM:
            break;
        case REFLECTION_TYPE_STRUCT:
            break;
        case REFLECTION_TYPE_MAX:
            break;
        }


        fwrite("[", strlen("["), 1, fptr);
        char str[64];
        sprintf(str, "%d", cur->array_count);
        fwrite(str, strlen(str), 1, fptr);
        fwrite("]", strlen("]"), 1, fptr);


        fwrite(":\n", strlen(":\n"), 1, fptr);

        for (int i = 0; i < cur->array_count; ++i)
        {
            fwrite("- ", strlen("- "), 1, fptr);

            //write out the data
            switch (cur->type)
            {
            case REFLECTION_TYPE_BOOL:

                if (*(bool*)((u8*)cur->data + (i * sizeof(bool))))
                {
                    fwrite("true", strlen("true"), 1, fptr);
                }
                else
                {
                    fwrite("false", strlen("false"), 1, fptr);
                }
                break;
            case REFLECTION_TYPE_S32:
                char str[64];
                sprintf(str, "%d", *(s32*)((u8*)cur->data + (i * sizeof(s32))));
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_U32:
                char str_u[64];
                sprintf(str_u, "%d", *(u32*)((u8*)cur->data + (i * sizeof(u32))));
                fwrite(str_u, strlen(str_u), 1, fptr);
                break;

            case REFLECTION_TYPE_F32:
                char str_f[64];
                sprintf(str_f, "%f", *(f32*)((u8*)cur->data + (i * sizeof(f32))));
                fwrite(str_f, strlen(str_f), 1, fptr);
                break;
            case REFLECTION_TYPE_CHAR:
                fwrite((u8*)cur->data + (i * sizeof(char)), sizeof(char), 1, fptr);
                break;
            case REFLECTION_TYPE_STRING:
                fwrite(cur->string[i].chars, cur->string[i].length, 1, fptr);
                break;
            case REFLECTION_TYPE_INVALID:
                break;
            case REFLECTION_TYPE_U8:
                break;
            case REFLECTION_TYPE_U16:
                break;
            case REFLECTION_TYPE_U64:
                break;
            case REFLECTION_TYPE_S8:
                break;
            case REFLECTION_TYPE_S16:
                break;
            case REFLECTION_TYPE_S64:
                break;
            case REFLECTION_TYPE_F64:
                break;
            case REFLECTION_TYPE_SIZE_T:
                break;
            case REFLECTION_TYPE_ENUM:
                break;
            case REFLECTION_TYPE_STRUCT:
                break;
            case REFLECTION_TYPE_MAX:
                break;
            }
            fwrite("\n", strlen("\n"), 1, fptr);
        }

        cur = cur->next;
    }


    fclose(fptr);
}

void madness_txt_read_file(Madness_txt* txt, const char* file_path, object_handle* handle)
{
    FILE* fptr = fopen(file_path, "r");
    fseek(fptr, 0L, SEEK_END);
    size_t file_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    u8* buffer = malloc(file_size);
    fread(buffer, file_size, 1, fptr);

    DEBUG("%s", (char*)buffer);

    //parse the file here
    //structure
    //   name[type]:
    //   - data:
    //   - data2 (if applicable)

    madness_txt_object* object = &txt->objects[handle->handle];


    madness_txt_object_subfield* cur = object->head;

    //i guess we just want a copy that has no data in it


    char* line = strtok((char*)buffer, "\n");

    while (line && cur)
    {
        if (line[0] == '#' || line[0] == '\n') // skip comments and empty lines
        {
            line = strtok(NULL, "\n");
            continue;
        }

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
            DEBUG("arr")
            DEBUG("field name: %s", field_name)
            DEBUG("type: %s", value_str)
            DEBUG("arr length: %s", array_count)
            line = strtok(NULL, "\n");

            if (strcmp(cur->field_name, field_name) != 0)
            {
                MASSERT(false);
            }
            size_t arr_size = c_string_to_number(array_count, sizeof(array_count));
            size_t type_size = madness_txt_str_to_type(value_str);
            if (cur->array_count != arr_size != 0)
            {
                MASSERT(false);
            }
            if (type_size != madness_txt_type_size(cur->type))
            {
                MASSERT(false);
            }

            for (size_t arr_index = 0; arr_index < arr_size; arr_index++)
            {
                if (line[0] == '-')
                {
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
                    WARN("field data arr: %s", field_data);

                    //do a conversion based on the type
                    switch (cur->type)
                    {
                    case REFLECTION_TYPE_BOOL:
                        if (strcmp(field_data, "true") == 0)
                        {
                            bool t = true;
                            memcpy((u8*)cur->data + (type_size * arr_index), &t, sizeof(bool));
                        }
                        else if (strcmp(field_data, "false") == 0)
                        {
                            bool f = false;
                            memcpy((u8*)cur->data + (type_size * arr_index), &f, sizeof(bool));
                        }
                        else
                        {
                            MASSERT(false);
                        }
                        break;
                    case REFLECTION_TYPE_S32:
                        s32 num = (s32)c_string_to_number(field_data, strlen(field_data));
                        memcpy((u8*)cur->data + (type_size * arr_index), &num, sizeof(s32));
                        break;
                    case REFLECTION_TYPE_U32:
                        u32 u_num = (u32)c_string_to_number(field_data, strlen(field_data));
                        u32* u_data = cur->data;
                        memcpy(&u_data[arr_index], &u_num, sizeof(u32));
                        break;
                    case REFLECTION_TYPE_F32:
                        f32 f_num = (f32)c_string_to_float(field_data);
                        memcpy((u8*)cur->data + (type_size * arr_index), &f_num, sizeof(f32));
                        break;
                    case REFLECTION_TYPE_CHAR:
                        memcpy((u8*)cur->data + (type_size * arr_index), &field_data, sizeof(char));
                        break;
                    case REFLECTION_TYPE_STRING:
                        cur->string[i].chars = field_data;
                        cur->string[i].length = strlen(field_data);
                        break;
                    case REFLECTION_TYPE_INVALID:
                        break;
                    case REFLECTION_TYPE_U8:
                        break;
                    case REFLECTION_TYPE_U16:
                        break;
                    case REFLECTION_TYPE_U64:
                        break;
                    case REFLECTION_TYPE_S8:
                        break;
                    case REFLECTION_TYPE_S16:
                        break;
                    case REFLECTION_TYPE_S64:
                        break;
                    case REFLECTION_TYPE_F64:
                        break;
                    case REFLECTION_TYPE_SIZE_T:
                        break;
                    case REFLECTION_TYPE_ENUM:
                        break;
                    case REFLECTION_TYPE_STRUCT:
                        break;
                    case REFLECTION_TYPE_MAX:
                        break;
                    }
                }

                //write out the data


                //we don't want to skip twice since we do a skip after each while loop iteration
                if (arr_index != arr_size - 1)
                {
                    line = strtok(NULL, "\n");
                }
            }
        }

        line = strtok(NULL, "\n");
        cur = cur->next;
    }

    return;
}

void madness_txt_object_read_args(Madness_txt* txt, object_handle handle, ...)
{
    UNIMPLEMENTED()
}

void madness_txt_object_read(Madness_txt* txt, object_handle handle, const char* field_name,
                             void* write_to_data)
{
    madness_txt_object* write_object = &txt->objects[handle.handle];
    madness_txt_object_subfield* cur = write_object->head;

    while (cur)
    {
        if (strcmp(cur->field_name, field_name) != 0)
        {
            cur = cur->next;
            continue;
        }
        size_t type_size = madness_txt_type_size(cur->type);

        if (cur->type == REFLECTION_TYPE_STRING)
        {
            const char** data_strings = (const char**)write_to_data;
            for (int i = 0; i < cur->array_count; ++i)
            {
                data_strings[i] = strdup(cur->string[i].chars);
            }
        }
        else
        {
            for (int i = 0; i < cur->array_count; ++i)
            {
                memcpy((u8*)write_to_data + (type_size * i), (u8*)cur->data + (type_size * i), type_size);
            }
        }
        return;
    }
}


size_t madness_txt_str_to_type(char* str)
{
    if (strcmp(str, "bool") == 0)
    {
        return sizeof(bool);
    }
    if (strcmp(str, "char") == 0)
    {
        return sizeof(char);
    }
    if (strcmp(str, "str") == 0)
    {
        //TODO: special case, handle it differently
        return sizeof(char*);
    }
    if (strcmp(str, "s32") == 0)
    {
        return sizeof(s32);
    }
    if (strcmp(str, "u32") == 0)
    {
        return sizeof(u32);
    }
    if (strcmp(str, "f32") == 0 || strcmp(str, "float") == 0)
    {
        return sizeof(f32);
    }

    return 0;
}

size_t madness_txt_type_size(Reflection_Type type)
{
    u32 size = 0;
    switch (type)
    {
    case REFLECTION_TYPE_INVALID:
        size = 0;
        break;
    case REFLECTION_TYPE_U8:
        size = sizeof(u8);
        break;
    case REFLECTION_TYPE_U16:
        size = sizeof(u16);
        break;
    case REFLECTION_TYPE_U32:
        size = sizeof(u32);
        break;
    case REFLECTION_TYPE_U64:
        size = sizeof(u64);
        break;
    case REFLECTION_TYPE_S8:
        size = sizeof(s8);
        break;
    case REFLECTION_TYPE_S16:
        size = sizeof(s16);
        break;
    case REFLECTION_TYPE_S32:
        size = sizeof(s32);
        break;
    case REFLECTION_TYPE_S64:
        size = sizeof(s32);
        break;
    case REFLECTION_TYPE_F32:
        size = sizeof(f32);
        break;
    case REFLECTION_TYPE_F64:
        size = sizeof(f64);
        break;
    case REFLECTION_TYPE_SIZE_T:
        size = sizeof(size_t);
        break;
    case REFLECTION_TYPE_BOOL:
        size = sizeof(bool);
        break;
    case REFLECTION_TYPE_STRING:
        size = sizeof(char*);
        break;
    case REFLECTION_TYPE_CHAR:
        size = sizeof(char);
        break;
    case REFLECTION_TYPE_ENUM:
        size = sizeof(u32);
        break;
    case REFLECTION_TYPE_STRUCT:
        size = 0;
        break;
    case REFLECTION_TYPE_MAX:
        break;
    }
    return size;
}


void madness_txt_object_test_example(Madness_txt* txt)
{
    madness_txt_test_struct_example example_struct = {
        .bool_type = false,
        .signed_int_type = 59,
        .unsigned_int_type = 456,
        .float_type = 10.58f,
        .char_type = 'e',
        .string_type = "oh no",
        .array_type = {10, 50, 85},
        .array_size = 3,
        .unsigned_key = 10,
        .float_value = 5.7f,
        .unsigned_key_arr = {1, 73, 75},
        .unsigned_value_arr = {740, 7, 25},
        .hash_arr_size = 3,
        .string_array_type = {"oh no", "oh yes"},
        .string_array_size = 2,
        .char_array_type = {'n', 'o'},
        .char_array_size = 2,
    };

    object_handle handle_example = madness_txt_object_create(txt, "madness test structure");
    madness_txt_object_add_item(txt, handle_example, "bool_type", REFLECTION_TYPE_BOOL, 1);
    madness_txt_object_add_item(txt, handle_example, "signed_int_type", REFLECTION_TYPE_S32, 1);
    madness_txt_object_add_item(txt, handle_example, "unsigned_int_type", REFLECTION_TYPE_U32, 1);
    madness_txt_object_add_item(txt, handle_example, "float_type", REFLECTION_TYPE_F32, 1);
    madness_txt_object_add_item(txt, handle_example, "char_type", REFLECTION_TYPE_CHAR, 1);
    madness_txt_object_add_item(txt, handle_example, "string_type", REFLECTION_TYPE_STRING, 1);

    madness_txt_object_add_item(txt, handle_example, "array_type", REFLECTION_TYPE_U32, example_struct.array_size);
    madness_txt_object_add_item(txt, handle_example, "array_size", REFLECTION_TYPE_U32, 1);

    madness_txt_object_add_item(txt, handle_example, "unsigned_key", REFLECTION_TYPE_U32, 1);
    madness_txt_object_add_item(txt, handle_example, "float_value", REFLECTION_TYPE_F32, 1);

    madness_txt_object_add_item(txt, handle_example, "unsigned_key_arr", REFLECTION_TYPE_U32,
                                example_struct.hash_arr_size);
    madness_txt_object_add_item(txt, handle_example, "unsigned_value_arr", REFLECTION_TYPE_U32,
                                example_struct.hash_arr_size);
    madness_txt_object_add_item(txt, handle_example, "hash_arr_size", REFLECTION_TYPE_U32,1);

    madness_txt_object_add_item(txt, handle_example, "string_array_type", REFLECTION_TYPE_STRING,
                                example_struct.string_array_size);
    madness_txt_object_add_item(txt, handle_example, "string_array_size", REFLECTION_TYPE_U32,1);

    madness_txt_object_add_item(txt, handle_example, "char_array_type", REFLECTION_TYPE_CHAR,
                                example_struct.char_array_size);
    madness_txt_object_add_item(txt, handle_example, "char_array_size", REFLECTION_TYPE_U32,1);

    //write
    /*madness_txt_object_set_write_data(txt, handle_example, "bool_type", &example_struct.bool_type);
    madness_txt_object_set_write_data(txt, handle_example, "signed_int_type", &example_struct.signed_int_type);
    madness_txt_object_set_write_data(txt, handle_example, "float_type", &example_struct.float_value);
    madness_txt_object_set_write_data(txt, handle_example, "char_type", &example_struct.char_type);
    madness_txt_object_set_write_data(txt, handle_example, "string_type", &example_struct.string_type);

    madness_txt_object_set_write_data(txt, handle_example, "array_type", &example_struct.array_type);

    madness_txt_object_set_write_data(txt, handle_example, "unsigned_key", &example_struct.unsigned_key);
    madness_txt_object_set_write_data(txt, handle_example, "float_value", &example_struct.float_value);

    madness_txt_object_set_write_data(txt, handle_example, "unsigned_key_arr", &example_struct.unsigned_key_arr);
    madness_txt_object_set_write_data(txt, handle_example, "unsigned_value_arr", &example_struct.unsigned_value_arr);
    madness_txt_object_set_write_data(txt, handle_example, "string_array_type",
                                      example_struct.string_array_type);
    madness_txt_object_set_write_data(txt, handle_example, "char_array_type", &example_struct.char_array_type);*/

    madness_txt_object_write_data_arg(txt, handle_example,
                                      &example_struct.bool_type,
                                      &example_struct.signed_int_type,
                                      &example_struct.unsigned_int_type,
                                      &example_struct.float_type,
                                      &example_struct.char_type,
                                      &example_struct.string_type,
                                      &example_struct.array_type,
                                      &example_struct.unsigned_key,
                                      &example_struct.float_value,
                                      &example_struct.unsigned_key_arr,
                                      &example_struct.unsigned_value_arr,
                                      example_struct.string_array_type,
                                      &example_struct.char_array_type
    );

    const char* file_test_example = "../core/serialization/madness_txt_test_write.yaml";
    madness_txt_object_write_file(txt, file_test_example, &handle_example);


    //
    madness_txt_read_file(txt, "../core/serialization/madness_txt_test_write.yaml", &handle_example);

    madness_txt_test_struct_example example_struct2 = {0};

    /*madness_txt_object_read_args(txt, handle_example,
                                      &example_struct2.bool_type,
                                      &example_struct2.signed_int_type,
                                      &example_struct2.unsigned_int_type,
                                      &example_struct2.float_type,
                                      &example_struct2.char_type,
                                      &example_struct2.string_type,
                                      &example_struct2.array_type,
                                      &example_struct2.unsigned_key,
                                      &example_struct2.float_value,
                                      &example_struct2.unsigned_key_arr,
                                      &example_struct2.unsigned_value_arr,
                                      example_struct2.string_array_type,
                                      &example_struct2.char_array_type);*/

    madness_txt_object_read(txt, handle_example, "bool_type", &example_struct2.bool_type);
    madness_txt_object_read(txt, handle_example, "signed_int_type", &example_struct2.signed_int_type);
    madness_txt_object_read(txt, handle_example, "unsigned_int_type", &example_struct2.unsigned_int_type);
    madness_txt_object_read(txt, handle_example, "float_type", &example_struct2.float_type);
    madness_txt_object_read(txt, handle_example, "char_type", &example_struct2.char_type);
    madness_txt_object_read(txt, handle_example, "string_type", &example_struct2.string_type);

    madness_txt_object_read(txt, handle_example, "array_type", &example_struct2.array_type);

    madness_txt_object_read(txt, handle_example, "unsigned_key", &example_struct2.unsigned_key);
    madness_txt_object_read(txt, handle_example, "float_value", &example_struct2.float_value);

    madness_txt_object_read(txt, handle_example, "unsigned_key_arr", &example_struct2.unsigned_key_arr);
    madness_txt_object_read(txt, handle_example, "unsigned_value_arr", &example_struct2.unsigned_value_arr);
    madness_txt_object_read(txt, handle_example, "string_array_type",
                            example_struct2.string_array_type);
    madness_txt_object_read(txt, handle_example, "char_array_type",
                            example_struct2.char_array_type);

    // madness_txt_object_write_data_single(txt, handle_example, "bool_type", &example_struct.bool_type);
    // madness_txt_object_write_data_single(txt, handle_example, "signed_int_type", &example_struct.signed_int_type);
    // madness_txt_object_write_data_single(txt, handle_example, "float_type", &example_struct.float_value);
    // madness_txt_object_write_data_single(txt, handle_example, "char_type", &example_struct.char_type);
    // madness_txt_object_write_data_single(txt, handle_example, "string_type", &example_struct.string_type);
    //
    // madness_txt_object_write_data_single(txt, handle_example, "array_type", &example_struct.array_type);
    //
    // madness_txt_object_write_data_single(txt, handle_example, "unsigned_key", &example_struct.unsigned_key);
    // madness_txt_object_write_data_single(txt, handle_example, "float_value", &example_struct.float_value);
    //
    // madness_txt_object_write_data_single(txt, handle_example, "unsigned_key_arr", &example_struct.unsigned_key_arr);
    // madness_txt_object_write_data_single(txt, handle_example, "unsigned_value_arr", &example_struct.unsigned_value_arr);
    // madness_txt_object_write_data_single(txt, handle_example, "string_array_type",
    //                               example_struct.string_array_type);
    // madness_txt_object_write_data_single(txt, handle_example, "char_array_type",
    //                               example_struct.char_array_type);
}
