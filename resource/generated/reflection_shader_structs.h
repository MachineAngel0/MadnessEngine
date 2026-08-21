#include <stddef.h>
#include "runtime_registry.h"
#include "../renderer/shaders/shader_includes/materials.glsl"

const char* shader_struct_string_list[] = {
	"Material_Instance", 
	"Pbr", 
};

void generate_runtime_structs_shader(Reflection_Registry* reflection_registry)
{
	Reflection_Runtime_Struct_Field Material_Instance_Fields[] =
	{
	};

	 Reflection_Runtime_Struct Material_Instance_Runtime_Struct =
	{
		.name = "Material_Instance",
		.fields = Material_Instance_Fields,
		.field_count = 0,
		.struct_size = sizeof(Material_Instance)
	};

	reflection_registry_add_struct(reflection_registry, Material_Instance_Runtime_Struct);

	Reflection_Runtime_Struct_Field Pbr_Fields[] =
	{
		{
			.name = "color",
			.type = REFLECTION_TYPE_VEC4,
			.type_name = "vec4",
			.offset = offsetof(Pbr, color)
		},
		{
			.name = "ambient_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Pbr, ambient_strength)
		},
		{
			.name = "roughness_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Pbr, roughness_strength)
		},
		{
			.name = "metallic_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Pbr, metallic_strength)
		},
		{
			.name = "normal_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Pbr, normal_strength)
		},
		{
			.name = "ambient_occlusion_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Pbr, ambient_occlusion_strength)
		},
		{
			.name = "emissive_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Pbr, emissive_strength)
		},
	};

	 Reflection_Runtime_Struct Pbr_Runtime_Struct =
	{
		.name = "Pbr",
		.fields = Pbr_Fields,
		.field_count = 7,
		.struct_size = sizeof(Pbr)
	};

	reflection_registry_add_struct(reflection_registry, Pbr_Runtime_Struct);

}
