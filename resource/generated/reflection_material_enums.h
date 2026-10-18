#include <stddef.h>
#include "runtime_registry.h"
#include "../resource/material_types.h"

const char* Permutation_PBR_enum_string[] = {
	"PBR_PIPELINE_SKINNED", 
	"PBR_PIPELINE_COLOR", 
	"PBR_PIPELINE_NORMAL", 
	"PBR_PIPELINE_EMISSIVE", 
	"PBR_PIPELINE_ROUGHNESS", 
	"PBR_PIPELINE_METALLIC", 
	"PBR_PIPELINE_AO", 
	"PBR_PIPELINE_ENUM_MAX", 
};

void generate_runtime_enums_material(Reflection_Registry* reflection_registry)
{
	const Reflection_Runtime_Enum Permutation_PBR_enum =
	{
		.name = "Permutation_PBR",
		.enum_names = Permutation_PBR_enum_string,
		.count = 8,
		.type = Reflection_Enum_Type_Bitflag,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Permutation_PBR_enum);

}
