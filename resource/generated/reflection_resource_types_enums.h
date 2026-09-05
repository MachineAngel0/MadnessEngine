#include <stddef.h>
#include "runtime_registry.h"
#include "../resource/resource_types.h"

const char* Asset_Type_enum_string[] = {
	[ASSET_TEXTURE]= "ASSET_TEXTURE", 
	[ASSET_FONT]= "ASSET_FONT", 
	[ASSET_SPRITE]= "ASSET_SPRITE", 
	[ASSET_STATIC_MESH]= "ASSET_STATIC_MESH", 
	[ASSET_SKINNED_MESH]= "ASSET_SKINNED_MESH", 
	[ASSET_AUDIO]= "ASSET_AUDIO", 
	[ASSET_MATERIAL]= "ASSET_MATERIAL", 
	[ASSET_MATERIAL_INSTANCE]= "ASSET_MATERIAL_INSTANCE", 
	[ASSET_SCENE]= "ASSET_SCENE", 
	[ASSET_PARTICLE_EFFECT]= "ASSET_PARTICLE_EFFECT", 
	[ASSET_PARTICLE_EMITTER]= "ASSET_PARTICLE_EMITTER", 
	[ASSET_TYPE_MAX]= "ASSET_TYPE_MAX", 
};

const char* Asset_Load_State_enum_string[] = {
	[ASSET_LOAD_STATE_UNLOADED]= "ASSET_LOAD_STATE_UNLOADED", 
	[ASSET_LOAD_STATE_QUEUED]= "ASSET_LOAD_STATE_QUEUED", 
	[ASSET_LOAD_STATE_LOADED]= "ASSET_LOAD_STATE_LOADED", 
};

const char* Texture_Format_enum_string[] = {
	[Texture_Format_Default]= "Texture_Format_Default", 
};

const char* Texture_Filter_enum_string[] = {
	[Texture_Filter_Nearest]= "Texture_Filter_Nearest", 
	[Texture_Filter_Linear]= "Texture_Filter_Linear", 
};

const char* Shader_Mesh_Type_enum_string[] = {
	[Shader_Mesh_Type_Mesh]= "Shader_Mesh_Type_Mesh", 
	[Shader_Mesh_Type_Skinned]= "Shader_Mesh_Type_Skinned", 
};

const char* Shader_Transluency_Type_enum_string[] = {
	[Shader_Transluency_Type_Opaque]= "Shader_Transluency_Type_Opaque", 
	[Shader_Transluency_Type_Transparent]= "Shader_Transluency_Type_Transparent", 
};

const char* Shader_Renderpass_Type_enum_string[] = {
	"Renderpass_Type_Predepth", 
	"Renderpass_Type_Shadow", 
	"Renderpass_Type_Color", 
	"Renderpass_Type_Reserved1", 
	"Renderpass_Type_Reserved2", 
	"Renderpass_Type_Reserved3", 
	"Renderpass_Type_Reserved4", 
	"Renderpass_Type_Reserved5", 
};

const char* Shader_Blend_enum_string[] = {
	[Shader_Blend_Mode_Default]= "Shader_Blend_Mode_Default", 
	[Shader_Blend_Mode_Alpha]= "Shader_Blend_Mode_Alpha", 
	[Shader_Blend_Mode_PreMultiplied_Alpha]= "Shader_Blend_Mode_PreMultiplied_Alpha", 
	[Shader_Blend_Mode_Additive]= "Shader_Blend_Mode_Additive", 
	[Shader_Blend_Mode_Soft_Additive]= "Shader_Blend_Mode_Soft_Additive", 
	[Shader_Blend_Mode_Multiply]= "Shader_Blend_Mode_Multiply", 
	[Shader_Blend_Mode_Multiply2x]= "Shader_Blend_Mode_Multiply2x", 
	[Shader_Blend_Mode_Subtract]= "Shader_Blend_Mode_Subtract", 
	[Shader_Blend_Mode_Reverse_Subtract]= "Shader_Blend_Mode_Reverse_Subtract", 
};

const char* Mesh_PBR_Flags_enum_string[] = {
	"MESH_PIPELINE_COLOR", 
	"MESH_PIPELINE_NORMAL", 
	"MESH_PIPELINE_EMISSIVE", 
	"MESH_PIPELINE_ROUGHNESS", 
	"MESH_PIPELINE_METALLIC", 
	"MESH_PIPELINE_AO", 
	"MESH_PIPELINE_ENUM_MAX", 
};

const char* Index_Type_enum_string[] = {
	[INDEX_TYPE_U16]= "INDEX_TYPE_U16", 
	[INDEX_TYPE_U32]= "INDEX_TYPE_U32", 
};

void generate_runtime_enums_resources(Reflection_Registry* reflection_registry)
{
	const Reflection_Runtime_Enum Asset_Type_enum =
	{
		.name = "Asset_Type",
		.enum_names = Asset_Type_enum_string,
		.count = 12,
		.type = Reflection_Enum_Type_Normal,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Asset_Type_enum);

	const Reflection_Runtime_Enum Asset_Load_State_enum =
	{
		.name = "Asset_Load_State",
		.enum_names = Asset_Load_State_enum_string,
		.count = 3,
		.type = Reflection_Enum_Type_Normal,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Asset_Load_State_enum);

	const Reflection_Runtime_Enum Texture_Format_enum =
	{
		.name = "Texture_Format",
		.enum_names = Texture_Format_enum_string,
		.count = 1,
		.type = Reflection_Enum_Type_Normal,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Texture_Format_enum);

	const Reflection_Runtime_Enum Texture_Filter_enum =
	{
		.name = "Texture_Filter",
		.enum_names = Texture_Filter_enum_string,
		.count = 2,
		.type = Reflection_Enum_Type_Normal,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Texture_Filter_enum);

	const Reflection_Runtime_Enum Shader_Mesh_Type_enum =
	{
		.name = "Shader_Mesh_Type",
		.enum_names = Shader_Mesh_Type_enum_string,
		.count = 2,
		.type = Reflection_Enum_Type_Normal,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Shader_Mesh_Type_enum);

	const Reflection_Runtime_Enum Shader_Transluency_Type_enum =
	{
		.name = "Shader_Transluency_Type",
		.enum_names = Shader_Transluency_Type_enum_string,
		.count = 2,
		.type = Reflection_Enum_Type_Normal,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Shader_Transluency_Type_enum);

	const Reflection_Runtime_Enum Shader_Renderpass_Type_enum =
	{
		.name = "Shader_Renderpass_Type",
		.enum_names = Shader_Renderpass_Type_enum_string,
		.count = 8,
		.type = Reflection_Enum_Type_Bitflag,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Shader_Renderpass_Type_enum);

	const Reflection_Runtime_Enum Shader_Blend_enum =
	{
		.name = "Shader_Blend",
		.enum_names = Shader_Blend_enum_string,
		.count = 9,
		.type = Reflection_Enum_Type_Normal,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Shader_Blend_enum);

	const Reflection_Runtime_Enum Mesh_PBR_Flags_enum =
	{
		.name = "Mesh_PBR_Flags",
		.enum_names = Mesh_PBR_Flags_enum_string,
		.count = 7,
		.type = Reflection_Enum_Type_Bitflag,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Mesh_PBR_Flags_enum);

	const Reflection_Runtime_Enum Index_Type_enum =
	{
		.name = "Index_Type",
		.enum_names = Index_Type_enum_string,
		.count = 2,
		.type = Reflection_Enum_Type_Normal,
		.bitflag_values = NULL, //filled in the function if applicable
	};
	reflection_registry_add_enums(reflection_registry, Index_Type_enum);

}
