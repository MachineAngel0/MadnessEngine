#include <stddef.h>
#include "runtime_registry.h"
#include "../resource/material_types.h"

void generate_runtime_structs_material(Reflection_Registry* reflection_registry)
{
	Reflection_Runtime_Struct_Field Material_Spherical_Billboard_Fields[] =
	{
		{
			.name = "point",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Material_Spherical_Billboard, point)
		},
		{
			.name = "texture_idx",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Spherical_Billboard, texture_idx)
		},
		{
			.name = "size",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Material_Spherical_Billboard, size)
		},
		{
			.name = "rotation",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Material_Spherical_Billboard, rotation)
		},
	};

	 Reflection_Runtime_Struct Material_Spherical_Billboard_Runtime_Struct =
	{
		.name = "Material_Spherical_Billboard",
		.fields = Material_Spherical_Billboard_Fields,
		.field_count = 4,
		.struct_size = sizeof(Material_Spherical_Billboard)
	};

	reflection_registry_add_struct(reflection_registry, Material_Spherical_Billboard_Runtime_Struct);

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
			.name = "padding0",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, padding0)
		},
		{
			.name = "color_texture",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Default, color_texture)
		},
		{
			.name = "normal_texture",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Default, normal_texture)
		},
		{
			.name = "metallic_texture",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Default, metallic_texture)
		},
		{
			.name = "roughness_texture",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Default, roughness_texture)
		},
		{
			.name = "ambient_occlusion_texture",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Default, ambient_occlusion_texture)
		},
		{
			.name = "emissive_texture",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Default, emissive_texture)
		},
	};

	 Reflection_Runtime_Struct Material_Default_Runtime_Struct =
	{
		.name = "Material_Default",
		.fields = Material_Default_Fields,
		.field_count = 15,
		.struct_size = sizeof(Material_Default)
	};

	reflection_registry_add_struct(reflection_registry, Material_Default_Runtime_Struct);

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
