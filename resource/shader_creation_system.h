#ifndef SHADER_CREATION_SYSTEM_H
#define SHADER_CREATION_SYSTEM_H
#include <stdbool.h>

//NOTE: Testing a shader creation system for funs and learning

typedef enum Shader_Type
{
    SHADER_TYPE_U32,
    SHADER_TYPE_I32,
    SHADER_TYPE_FLOAT,
    SHADER_TYPE_VEC2,
    SHADER_TYPE_VEC3,
    SHADER_TYPE_MAT3,
    SHADER_TYPE_MAT4,
    SHADER_TYPE_MAX,
} Shader_Type;

const char* shader_type_lookup[SHADER_TYPE_MAX] =
{
    [SHADER_TYPE_U32] = "uint",
    [SHADER_TYPE_I32] = "int",
    [SHADER_TYPE_FLOAT] = "float",
    [SHADER_TYPE_VEC2] = "vec2",
    [SHADER_TYPE_VEC3] = "vec3",
    [SHADER_TYPE_MAT3] = "mat3",
    [SHADER_TYPE_MAT4] = "mat4",
};


typedef enum Shader_Operation
{
    SHADER_Operation_Addition,
    SHADER_Operation_Subtract,
    SHADER_Operation_Multiply,
    SHADER_Operation_Divide,
    SHADER_Operation_Texture_Blend,
} Shader_Operation;

typedef enum Shader_Stage
{
    SHADER_STAGE_VERTEX,
    SHADER_STAGE_FRAGMENT,
    SHADER_STAGE_TESSELATION,
    //NOTE: idk what geo is, and compute should be its own thing
    // SHADER_STAGE_GEOMETRY,
    // SHADER_STAGE_COMPUTE,
    SHADER_STAGE_MAX,
} Shader_Stage;

typedef enum BUFFER_LAYOUT_TYPE
{
    BUFFER_LAYOUT_st140, // structs rounds up alignments to 16 bytes
    BUFFER_LAYOUT_st430,
    BUFFER_LAYOUT_scalar, //use until performance becomes an issue, then align structs properly
} BUFFER_LAYOUT_TYPE;

//NOTE: these are assumed to be graphic shaders and not compute shaders
//NOTE: i should not be using a shader handle for this

typedef struct Shader_Creation_System
{
    bool unimplemented;
    Arena* frame_arena;
    FILE* open_file;
    const char* current_file_name;

    const char* param_names[100];
    Shader_Type param_type[100];
    u32 param_count;
} Shader_Creation_System;

typedef struct Shader_Creation_Handle
{
    u32 handle;
} Shader_Creation_Handle;

typedef struct Shader_Param_Handle
{
    u32 handle;
} Shader_Param_Handle;

#define SHADER_PATH "../renderer/shaders/"
#define SHADER_TEST_NAME "test_shader"

Shader_Creation_System* shader_creation_system_init(Memory_System* memory_system)
{
    Shader_Creation_System* shader_creation_system = memory_system_alloc(memory_system, sizeof(Shader_Creation_System),
                                                                         MEMORY_SUBSYSTEM_RESOURCE);
    shader_creation_system->frame_arena = memory_system_alloc(memory_system, sizeof(Arena), MEMORY_SUBSYSTEM_RESOURCE);

    u64 arena_memory_amount = KB(512);
    void* mem = memory_system_alloc(memory_system, arena_memory_amount, MEMORY_SUBSYSTEM_RESOURCE);
    arena_init(shader_creation_system->frame_arena, mem, arena_memory_amount, NULL);

    return shader_creation_system;
}


Shader_Creation_Handle shader_creation_system_shader_start(Shader_Creation_System* shader_system,
                                                           const char* shader_name, Shader_Stage shader_stage)
{
    arena_clear(shader_system->frame_arena); // really dumb but it does work, alternatively, we could use a stack arena
    shader_system->param_count = 0;

    const char* file_name = c_string_concat(SHADER_PATH, shader_name, shader_system->frame_arena);
    switch (shader_stage)
    {
    case SHADER_STAGE_VERTEX:
        shader_system->current_file_name = c_string_concat(file_name, ".vert", shader_system->frame_arena);
        break;
    case SHADER_STAGE_FRAGMENT:
        shader_system->current_file_name = c_string_concat(file_name, ".frag", shader_system->frame_arena);
        break;
    case SHADER_STAGE_TESSELATION:
        UNIMPLEMENTED();
        break;
    case SHADER_STAGE_MAX:
        UNIMPLEMENTED();
        break;
    }

    shader_system->open_file = fopen(shader_system->current_file_name, "w");

    if (!shader_system->open_file)
    {
        return (Shader_Creation_Handle){0};
    }
}

void shader_creation_system_shader_end(Shader_Creation_System* shader_system)
{
    fclose(shader_system->open_file);
}


//NOTE: these should be hardcoded, like for PBR_OQAQUE, we know what the inputs and outputs are, in general,
bool shader_creation_system_add_input(Shader_Creation_System* shader_system, Shader_Creation_Handle handle,
                                      Shader_Type shader_type, const char* shader_input_name);
bool shader_creation_system_add_output(Shader_Creation_System* shader_system, Shader_Creation_Handle handle,
                                       Shader_Type shader_type, const char* shader_output_name);

Shader_Param_Handle shader_creation_system_add_param(Shader_Creation_System* shader_system,
                                                     Shader_Creation_Handle handle, Shader_Type shader_type,
                                                     const char* param_name)
{
    Shader_Param_Handle param_handle = {shader_system->param_count};
    shader_system->param_type[shader_system->param_count] = shader_type;
    shader_system->param_names[shader_system->param_count] = param_name;
    shader_system->param_count++;

    fprintf(shader_system->open_file, "%s %s;\n", shader_type_lookup[shader_type], param_name);


    return param_handle;
}

bool shader_creation_system_addition(Shader_Creation_System* shader_system, Shader_Creation_Handle handle,
                                     Shader_Param_Handle param1, Shader_Param_Handle param2)
{
    const char* name1 = shader_system->param_names[param1.handle];
    const char* name2 = shader_system->param_names[param2.handle];

    fprintf(shader_system->open_file, "%s + %s;\n", name1, name2);
}

bool shader_creation_system_test(Shader_Creation_System* shader_creation_system)
{
    Shader_Creation_Handle handle = shader_creation_system_shader_start(shader_creation_system, SHADER_TEST_NAME,
                                                                        SHADER_STAGE_VERTEX);

    Shader_Param_Handle wind_speed = shader_creation_system_add_param(shader_creation_system, handle, SHADER_TYPE_FLOAT,
                                                                      "wind_speed");
    Shader_Param_Handle wind_direction = shader_creation_system_add_param(
        shader_creation_system, handle, SHADER_TYPE_FLOAT, "wind_direction");

    shader_creation_system_addition(shader_creation_system, handle, wind_speed, wind_direction);

    shader_creation_system_shader_end(shader_creation_system);
}


/* TODO: maybe
Shader_Creation_Handle shader_system_struct_create(Shader_Creation_System* shader_system);
bool shader_system_struct_add_field(Shader_Creation_System* shader_system);

//these would be globals generally, and then pipeline shaders can just specify which one they need
Shader_Creation_Handle shader_system_uniform_create(Shader_Creation_System* shader_system);
bool shader_system_uniform_add_field(Shader_Creation_System* shader_system, Shader_Creation_Handle handle);

Shader_Creation_Handle shader_system_push_constant_create(Shader_Creation_System* shader_system, Shader_Creation_Handle handle, Shader_Type shader_type);
bool shader_system_push_constant_add_field(Shader_Creation_System* shader_system, Shader_Creation_Handle handle);
 */


#endif //SHADER_CREATION_SYSTEM_H
