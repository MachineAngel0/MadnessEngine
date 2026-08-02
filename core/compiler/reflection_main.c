#include "defines.h"
#include "memory_system.h"

int main(void)
{
    Memory_System memory_system = {0};
    memory_system_init(&memory_system, MB(16));


    Reflection_System* reflection_system = reflection_system_init(&memory_system);


    //resource types
    reflection_system_parse(reflection_system, "../resource/resource_types.h",
                            REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(reflection_system, "../resource/resource_types.h", REFLECTION_PARSE_ENUM);
    reflection_system_parse(reflection_system, "../resource/resource_types.h",
                            REFLECTION_PARSE_STRUCT);

    reflection_data_to_files(reflection_system, "resources",
                             "../resource/generated/reflection_resource_types_enums.h",
                             "../resource/generated/reflection_resource_types_structs.h");


    reflection_system_reset(reflection_system);


    //game types

    reflection_system_parse(reflection_system, "../MadnessPulse/game_constants.h", REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(reflection_system, "../MadnessPulse/game_enums.h", REFLECTION_PARSE_ENUM);
    reflection_system_parse(reflection_system, "../MadnessPulse/game_structs.h", REFLECTION_PARSE_STRUCT);

    reflection_data_to_files(reflection_system, "madness_pulse_game",
                             "../MadnessPulse/generated/reflection_game_enums.h",
                             "../MadnessPulse/generated/reflection_game_struct.h");


    //material types
    reflection_system_reset(reflection_system);

    reflection_system_parse(reflection_system, "../resource/material_types.h",
                            REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(reflection_system, "../resource/material_types.h", REFLECTION_PARSE_ENUM);
    reflection_system_parse(reflection_system, "../resource/material_types.h",
                            REFLECTION_PARSE_STRUCT);

    reflection_data_to_files(reflection_system, "material",
                             "../resource/generated/reflection_material_enums.h",
                             "../resource/generated/reflection_material_structs.h");

    //shader definitions
    reflection_system_reset(reflection_system);


    reflection_system_parse(reflection_system, "../renderer/shaders/shader_includes/materials.glsl",
                            REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(reflection_system, "../renderer/shaders/shader_includes/materials.glsl",
                            REFLECTION_PARSE_ENUM);
    reflection_system_parse(reflection_system, "../renderer/shaders/shader_includes/materials.glsl",
                            REFLECTION_PARSE_STRUCT);

    reflection_data_to_files(reflection_system, "shader",
                             "../resource/generated/reflection_shader_enums.h",
                             "../resource/generated/reflection_shader_structs.h");

    /*
     * NOTE: seems not worth the hassle, different types from cpu to gpu side vec4 vs vec4s and uint vs u32/uint_32
     *  and no way to know if something is a bindless texture just from uint, unless i have a macro, and honestly nah
     *  I would rather generate a shader from cpu side than the reverse, like a shader graph
    reflection_data_generate_structs_from_shader(reflection_system, "shader_structs",
                                                 "../resource/generated/reflection_shader_structs_for_mat.h");
    */
    memory_system_shutdown(&memory_system);
}
