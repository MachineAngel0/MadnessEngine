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
    reflection_system_parse(reflection_system, "../renderer/shaders/shader_includes/materials.glsl", REFLECTION_PARSE_ENUM);
    reflection_system_parse(reflection_system, "../renderer/shaders/shader_includes/materials.glsl",
                            REFLECTION_PARSE_STRUCT);

    reflection_data_to_files(reflection_system, "material",
                             "../resource/generated/reflection_shader_enums.h",
                             "../resource/generated/reflection_shader_structs.h");



    memory_system_shutdown(&memory_system);





}
