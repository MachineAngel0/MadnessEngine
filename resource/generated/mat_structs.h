#include <stddef.h>
#include "runtime_registry.h"
#include "../resource/material_types.h"

void generate_runtime_structs_material(Reflection_Registry* reflection_registry)
{
	Reflection_Runtime_Struct_Field Material_Default_Fields[] =
	{
		{
			.name = "flags",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, flags)
		},
		{
			.name = "color",
			.type = REFLECTION_TYPE_VEC4,
			.type_name = "vec4s",
			.offset = offsetof(Material_Default, color)
		},
		{
			.name = "ambient_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Material_Default, ambient_strength)
		},
		{
			.name = "roughness_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Material_Default, roughness_strength)
		},
		{
			.name = "metallic_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Material_Default, metallic_strength)
		},
		{
			.name = "normal_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Material_Default, normal_strength)
		},
		{
			.name = "ambient_occlusion_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Material_Default, ambient_occlusion_strength)
		},
		{
			.name = "emissive_strength",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Material_Default, emissive_strength)
		},
		{
			.name = "color_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, color_index)
		},
		{
			.name = "normal_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, normal_index)
		},
		{
			.name = "metallic_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, metallic_index)
		},
		{
			.name = "roughness_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, roughness_index)
		},
		{
			.name = "ambient_occlusion_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, ambient_occlusion_index)
		},
		{
			.name = "emissive_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, emissive_index)
		},
		{
			.name = "padding0",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, padding0)
		},
		{
			.name = "padding1",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, padding1)
		},
		{
			.name = "padding2",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, padding2)
		},
	};

	 Reflection_Runtime_Struct Material_Default_Runtime_Struct =
	{
		.name = "Material_Default",
		.fields = Material_Default_Fields,
		.field_count = 17,
		.struct_size = sizeof(Material_Default)
	};

	reflection_registry_add_struct(reflection_registry, Material_Default_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Billboard_Spherical_Fields[] =
	{
		{
			.name = "point",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Material_Billboard_Spherical, point)
		},
		{
			.name = "texture_idx",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Billboard_Spherical, texture_idx)
		},
		{
			.name = "size",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Material_Billboard_Spherical, size)
		},
		{
			.name = "rotation",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Material_Billboard_Spherical, rotation)
		},
	};

	 Reflection_Runtime_Struct Material_Billboard_Spherical_Runtime_Struct =
	{
		.name = "Material_Billboard_Spherical",
		.fields = Material_Billboard_Spherical_Fields,
		.field_count = 4,
		.struct_size = sizeof(Material_Billboard_Spherical)
	};

	reflection_registry_add_struct(reflection_registry, Material_Billboard_Spherical_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Flat_Color_Fields[] =
	{
		{
			.name = "color",
			.type = REFLECTION_TYPE_VEC4,
			.type_name = "vec4",
			.offset = offsetof(Material_Flat_Color, color)
		},
	};

	 Reflection_Runtime_Struct Material_Flat_Color_Runtime_Struct =
	{
		.name = "Material_Flat_Color",
		.fields = Material_Flat_Color_Fields,
		.field_count = 1,
		.struct_size = sizeof(Material_Flat_Color)
	};

	reflection_registry_add_struct(reflection_registry, Material_Flat_Color_Runtime_Struct);

}
