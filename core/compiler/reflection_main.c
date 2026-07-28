#include "defines.h"
#include "memory_system.h"

int main(void)
{
    Memory_System memory_system = {0};
    memory_system_init(&memory_system, MB(4));


    Reflection_System* reflection_system = reflection_system_init(&memory_system);

    reflection_system_parse(reflection_system, "../resource/material_types.h",
                            REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(reflection_system, "../resource/material_types.h", REFLECTION_PARSE_ENUM);
    reflection_system_parse(reflection_system, "../resource/material_types.h",
                            REFLECTION_PARSE_STRUCT);

    reflection_data_to_files(reflection_system, "material",
                             "../resource/generated/mat_enums.h",
                             "../resource/generated/mat_structs.h");


    // reflection_game_data(reflection_system);


    reflection_system_parse(reflection_system, "../MadnessPulse/game_constants.h", REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(reflection_system, "../MadnessPulse/game_enums.h", REFLECTION_PARSE_ENUM);
    reflection_system_parse(reflection_system, "../MadnessPulse/game_structs.h", REFLECTION_PARSE_STRUCT);

    reflection_data_to_files(reflection_system, "madness_pulse_game",
                           "../MadnessPulse/game_reflection_enums_generated2.h",
                           "../MadnessPulse/game_reflection_struct_generated2.h");



    reflection_system_parse(reflection_system, "../resource/resource_types.h",
                            REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(reflection_system, "../resource/resource_types.h", REFLECTION_PARSE_ENUM);
    reflection_system_parse(reflection_system, "../resource/resource_types.h",
                            REFLECTION_PARSE_STRUCT);

    reflection_data_to_files(reflection_system, "resources",
                             "../resource/generated/resource_types_enums.h",
                             "../resource/generated/resource_types_structs.h");



    memory_system_shutdown(&memory_system);





}
