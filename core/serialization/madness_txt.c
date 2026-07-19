#include "madness_txt.h"

#include "c_string.h"


Madness_txt* madness_txt_init(Memory_System* memory_system)
{
    Madness_txt* system = (Madness_txt*)malloc(sizeof(Madness_txt));
    system->object_count = 0;
    memset(system->objects, 0, sizeof(madness_txt_object) * MADNESS_TXT_OBJECT_MAX);


    return system;
}

void madness_txt_shutdown(Madness_txt* txt)
{
}


Madness_Txt_Handle madness_txt_object_create(Madness_txt* txt, const char* name)
{
    madness_txt_object* object = &txt->objects[txt->object_count++];
    object->name = name;
    return (Madness_Txt_Handle){txt->object_count - 1};
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


Madness_Txt_Handle madness_txt_schema_create(Madness_txt* txt, const char* name)
{
    madness_txt_object* object = &txt->objects[txt->object_count++];
    object->name = name;
    return (Madness_Txt_Handle){txt->object_count - 1};
}

void madness_txt_schema_add_type(Madness_txt* txt, Madness_Txt_Handle handle, const char* field_name,
                                 Reflection_Type type, u32 array_size)
{
    madness_txt_object_subfield* new_subfield = malloc(sizeof(madness_txt_object_subfield));
    new_subfield->field_name = field_name;
    new_subfield->next = NULL;

    new_subfield->type = type;
    new_subfield->array_count = array_size;

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


void madness_txt_schema_write(Madness_txt* txt, Madness_Txt_Handle* object_handle, void* struct_data,
                              const char* file_path)
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
        case REFLECTION_TYPE_INVALID:
            MASSERT(false);
            break;
        case REFLECTION_TYPE_U8:
            fwrite("[u8]", strlen("[u8]"), 1, fptr);
            break;
        case REFLECTION_TYPE_U16:
            fwrite("[u16]", strlen("[u16]"), 1, fptr);
            break;
        case REFLECTION_TYPE_U32:
            fwrite("[u32]", strlen("[u32]"), 1, fptr);
            break;
        case REFLECTION_TYPE_U64:
            fwrite("[u64]", strlen("[u64]"), 1, fptr);
            break;
        case REFLECTION_TYPE_S8:
            fwrite("[s8]", strlen("[s8]"), 1, fptr);
            break;
        case REFLECTION_TYPE_S16:
            fwrite("[s16]", strlen("[s16]"), 1, fptr);
            break;
        case REFLECTION_TYPE_S32:
            fwrite("[s32]", strlen("[s32]"), 1, fptr);
            break;
        case REFLECTION_TYPE_S64:
            fwrite("[s64]", strlen("[s64]"), 1, fptr);
            break;
        case REFLECTION_TYPE_F32:
            fwrite("[f32]", strlen("[f32]"), 1, fptr);
            break;
        case REFLECTION_TYPE_F64:
            fwrite("[f64]", strlen("[f64]"), 1, fptr);
            break;
        case REFLECTION_TYPE_SIZE_T:
            fwrite("[size_t]", strlen("[size_t]"), 1, fptr);
            break;
        case REFLECTION_TYPE_BOOL:
            fwrite("[bool]", strlen("[bool]"), 1, fptr);
            break;
        case REFLECTION_TYPE_CHAR:
            fwrite("[char]", strlen("[char]"), 1, fptr);
            break;
        case REFLECTION_TYPE_CHAR_STRING:
            break;
        case REFLECTION_TYPE_STRING:
            fwrite("[str]", strlen("[str]"), 1, fptr);
            break;
        case REFLECTION_TYPE_ENUM:
            fwrite("[enum]", strlen("[enum]"), 1, fptr);
            break;
        case REFLECTION_TYPE_STRUCT:
            MASSERT(false);
            break;
        case REFLECTION_TYPE_VEC2:
            fwrite("[vec2]", strlen("[vec2]"), 1, fptr);
            break;
        case REFLECTION_TYPE_VEC3:
            fwrite("[vec3]", strlen("[vec3]"), 1, fptr);
            break;
        case REFLECTION_TYPE_VEC4:
            fwrite("[vec4]", strlen("[vec4]"), 1, fptr);
            break;
        case REFLECTION_TYPE_MAT3:
            fwrite("[mat3]", strlen("[mat3]"), 1, fptr);
            break;
        case REFLECTION_TYPE_MAT4:
            fwrite("[mat4]", strlen("[mat4]"), 1, fptr);
            break;
        case REFLECTION_TYPE_MAX:
            MASSERT(false);
            break;
        }


        fwrite("[", strlen("["), 1, fptr);
        char str[64];
        sprintf(str, "%d", cur->array_count);
        fwrite(str, strlen(str), 1, fptr);
        fwrite("]", strlen("]"), 1, fptr);


        fwrite(":\n", strlen(":\n"), 1, fptr);

        u8* struct_pos = struct_data;

        for (int i = 0; i < cur->array_count; ++i)
        {
            fwrite("- ", strlen("- "), 1, fptr);


            size_t buffer_size = 64;
            char str[64];

            //write out the data
            switch (cur->type)
            {
            case REFLECTION_TYPE_INVALID:
                MASSERT(false);
                break;
            case REFLECTION_TYPE_U8:
                snprintf(str, buffer_size, "%d", *(u8*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_U16:
                snprintf(str, buffer_size, "%d", *(u16*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_U32:
                snprintf(str, buffer_size, "%d", *(u32*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_U64:
                snprintf(str, buffer_size, "%llu", *(u64*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_S8:
                snprintf(str, buffer_size, "%d", *(s8*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_S16:
                snprintf(str, buffer_size, "%d", *(s16*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_S32:
                snprintf(str, buffer_size, "%d", *(s32*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_S64:
                snprintf(str, buffer_size, "%llu", *(s64*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_F32:
                snprintf(str, buffer_size, "%f", *(f32*)(struct_pos));
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_F64:
                snprintf(str, buffer_size, "%f", *(f64*)(struct_pos));
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_SIZE_T:
                snprintf(str, buffer_size, "%llu", *(size_t*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_BOOL:
                if (*(bool*)struct_pos)
                {
                    fwrite("true", strlen("true"), 1, fptr);
                }
                else
                {
                    fwrite("false", strlen("false"), 1, fptr);
                }
                break;
            case REFLECTION_TYPE_CHAR:
                snprintf(str, buffer_size, "%d", *(char*)struct_pos);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_CHAR_STRING:
                break;
            case REFLECTION_TYPE_STRING:
                // snprintf(str, buffer_size, "%d", *(char*)struct_pos);
                // fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_ENUM:
                break;
            case REFLECTION_TYPE_STRUCT:
                MASSERT(false);
                break;
            case REFLECTION_TYPE_VEC2:
                vec2s* v = (vec2s*)struct_pos;
                snprintf(str, buffer_size, "%f", v->x);
                fwrite(str, strlen(str), 1, fptr);

                snprintf(str, buffer_size, "%f", v->y);
                fwrite(str, strlen(str), 1, fptr);

                break;
            case REFLECTION_TYPE_VEC3:
                vec3s* v3 = (vec3s*)struct_pos;
                snprintf(str, buffer_size, "%f", v3->x);
                fwrite(str, strlen(str), 1, fptr);

                snprintf(str, buffer_size, "%f", v3->y);
                fwrite(str, strlen(str), 1, fptr);

                snprintf(str, buffer_size, "%f", v3->z);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_VEC4:
                vec4s* v4 = (vec4s*)struct_pos;
                snprintf(str, buffer_size, "%f", v4->x);
                fwrite(str, strlen(str), 1, fptr);

                snprintf(str, buffer_size, "%f", v4->y);
                fwrite(str, strlen(str), 1, fptr);

                snprintf(str, buffer_size, "%f", v4->z);
                fwrite(str, strlen(str), 1, fptr);

                snprintf(str, buffer_size, "%f", v4->w);
                fwrite(str, strlen(str), 1, fptr);
                break;
            case REFLECTION_TYPE_MAT3:
                FATAL("TOO LAZY TO DO MAT3")
                break;
            case REFLECTION_TYPE_MAT4:
                FATAL("TOO LAZY TO DO MAT4")
                break;
            case REFLECTION_TYPE_MAX:
                MASSERT(false);
                break;
            }
            struct_pos += reflection_type_get_size(cur->type);

            fwrite("\n", strlen("\n"), 1, fptr);
        }

        cur = cur->next;
    }


    fclose(fptr);
}


void madness_txt_schema_read(Madness_txt* txt, Madness_Txt_Handle* handle, void* out_data, const char* file_path)
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

    u8* struct_pos = out_data;
    u32 line_count = 0;
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
                            memcpy(struct_pos, &t, sizeof(bool));
                        }
                        else if (strcmp(field_data, "false") == 0)
                        {
                            bool f = false;
                            memcpy(struct_pos, &f, sizeof(bool));
                        }
                        else
                        {
                            MASSERT(false);
                        }
                        break;
                    case REFLECTION_TYPE_U8:
                        u8 u8_num = (u8)c_string_to_number(field_data, strlen(field_data));
                        memcpy(struct_pos, &u8_num, sizeof(u8));
                        break;
                    case REFLECTION_TYPE_U16:
                        break;
                    case REFLECTION_TYPE_U32:
                        u32 u_num = (u32)c_string_to_number(field_data, strlen(field_data));
                        memcpy(struct_pos, &u_num, reflection_type_get_size(cur->type));
                        break;
                    case REFLECTION_TYPE_U64:
                        break;
                    case REFLECTION_TYPE_S8:
                        break;
                    case REFLECTION_TYPE_S16:
                        break;
                    case REFLECTION_TYPE_S32:
                        s32 num = (s32)c_string_to_number(field_data, strlen(field_data));
                        memcpy(struct_pos, &num, sizeof(s32));
                        break;
                    case REFLECTION_TYPE_S64:
                        break;
                    case REFLECTION_TYPE_F32:
                        f32 f_num = (f32)c_string_to_float(field_data);
                        memcpy(struct_pos, &f_num, sizeof(f32));
                        break;
                    case REFLECTION_TYPE_F64:
                        break;
                    case REFLECTION_TYPE_CHAR:
                        memcpy(struct_pos, &field_data, sizeof(char));
                        break;
                    case REFLECTION_TYPE_STRING:
                        // cur->string[i].chars = field_data;
                        // cur->string[i].length = strlen(field_data);
                        break;
                    case REFLECTION_TYPE_SIZE_T:
                        break;
                    case REFLECTION_TYPE_ENUM:
                        break;
                    case REFLECTION_TYPE_STRUCT:
                        break;
                    case REFLECTION_TYPE_INVALID:
                        break;
                    case REFLECTION_TYPE_MAX:
                        break;
                    }
                }
                    struct_pos += reflection_type_get_size(cur->type);

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


void madness_txt_test_schema(Madness_txt* txt)
{
    //NEW API
    typedef struct schema_test
    {
        u32 uint32_type;
        bool bool_type;
    } schema_test;

    schema_test test_schema = {
        .uint32_type = 321,
        .bool_type = true,
    };

    //NOTE: the struct has to be properly aligned

    Madness_Txt_Handle handle = madness_txt_schema_create(txt, "scheme a test");
    madness_txt_schema_add_type(txt, handle, "uint32_type", REFLECTION_TYPE_U32, 1);
    madness_txt_schema_add_type(txt, handle, "bool_type", REFLECTION_TYPE_BOOL, 1);


    madness_txt_schema_write(txt, &handle, &test_schema, "../core/serialization/madness_txt_scheme_test_write.yaml");

    schema_test empty_schema = {0};
    madness_txt_schema_read(txt, &handle, &empty_schema, "../core/serialization/madness_txt_scheme_test_write.yaml");


}
