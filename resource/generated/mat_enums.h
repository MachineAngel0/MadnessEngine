#include <stddef.h>
#include "runtime_registry.h"
#include "../resource/material_types.h"

const char* Shader_Pass_Type_enum_string[] = {
	[Shader_Pass_Type_MESH_PBR_OPAQUE]= "Shader_Pass_Type_MESH_PBR_OPAQUE", 
	[Shader_Pass_Type_MESH_PBR_TRANSPARENCY]= "Shader_Pass_Type_MESH_PBR_TRANSPARENCY", 
	[Shader_Pass_Type_SHADOW]= "Shader_Pass_Type_SHADOW", 
};

const char* Shader_Stage_Type_enum_string[] = {
	[Shader_Stage_Type_Vertex]= "Shader_Stage_Type_Vertex", 
	[Shader_Stage_Type_Fragment]= "Shader_Stage_Type_Fragment", 
	[Shader_Stage_Type_Compute]= "Shader_Stage_Type_Compute", 
};

const char* Material_Flag_enum_string[] = {
	[MATERIAL_FLAG_PBR]= "MATERIAL_FLAG_PBR", 
	[MATERIAL_FLAG_UV_ANIM]= "MATERIAL_FLAG_UV_ANIM", 
};

const char* Mesh_PBR_Flags_enum_string[] = {
	[MESH_PIPELINE_COLOR]= "MESH_PIPELINE_COLOR", 
	[MESH_PIPELINE_NORMAL]= "MESH_PIPELINE_NORMAL", 
	[MESH_PIPELINE_EMISSIVE]= "MESH_PIPELINE_EMISSIVE", 
	[MESH_PIPELINE_ROUGHNESS]= "MESH_PIPELINE_ROUGHNESS", 
	[MESH_PIPELINE_METALLIC]= "MESH_PIPELINE_METALLIC", 
	[MESH_PIPELINE_AO]= "MESH_PIPELINE_AO", 
	[MESH_PIPELINE_ENUM_MAX]= "MESH_PIPELINE_ENUM_MAX", 
};

void generate_runtime_enums_material(Reflection_Registry* reflection_registry)
{
	const Reflection_Runtime_Enum Shader_Pass_Type_enum =
	{
		.name = "Shader_Pass_Type",
		.enum_names = Shader_Pass_Type_enum_string,
		.count = ARRAY_SIZE(Shader_Pass_Type_enum_string),
	};
	reflection_registry_add_enums(reflection_registry, Shader_Pass_Type_enum);

	const Reflection_Runtime_Enum Shader_Stage_Type_enum =
	{
		.name = "Shader_Stage_Type",
		.enum_names = Shader_Stage_Type_enum_string,
		.count = ARRAY_SIZE(Shader_Stage_Type_enum_string),
	};
	reflection_registry_add_enums(reflection_registry, Shader_Stage_Type_enum);

	const Reflection_Runtime_Enum Material_Flag_enum =
	{
		.name = "Material_Flag",
		.enum_names = Material_Flag_enum_string,
		.count = ARRAY_SIZE(Material_Flag_enum_string),
	};
	reflection_registry_add_enums(reflection_registry, Material_Flag_enum);

	const Reflection_Runtime_Enum Mesh_PBR_Flags_enum =
	{
		.name = "Mesh_PBR_Flags",
		.enum_names = Mesh_PBR_Flags_enum_string,
		.count = ARRAY_SIZE(Mesh_PBR_Flags_enum_string),
	};
	reflection_registry_add_enums(reflection_registry, Mesh_PBR_Flags_enum);

}
