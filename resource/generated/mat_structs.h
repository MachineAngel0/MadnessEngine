#include <stddef.h>
#include "runtime_registry.h"
#include "../resource/material_types.h"

void generate_runtime_structs_material(Reflection_Registry* reflection_registry)
{
	Reflection_Runtime_Struct_Field Material_System_Fields[] =
	{
		{
			.name = "prb",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Default",
			.offset = offsetof(Material_System, prb)
		},
		{
			.name = "pbr_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_System, pbr_count)
		},
	};

	 Reflection_Runtime_Struct Material_System_Runtime_Struct =
	{
		.name = "Material_System",
		.fields = Material_System_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_System)
	};

	reflection_registry_add_struct(reflection_registry, Material_System_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Default_Fields[] =
	{
		{
			.name = "flags",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Default, flags)
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
		.field_count = 16,
		.struct_size = sizeof(Material_Default)
	};

	reflection_registry_add_struct(reflection_registry, Material_Default_Runtime_Struct);

	Reflection_Runtime_Struct_Field PC_Mesh_Fields[] =
	{
		{
			.name = "ubo_buffer_idx",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(PC_Mesh, ubo_buffer_idx)
		},
	};

	 Reflection_Runtime_Struct PC_Mesh_Runtime_Struct =
	{
		.name = "PC_Mesh",
		.fields = PC_Mesh_Fields,
		.field_count = 1,
		.struct_size = sizeof(PC_Mesh)
	};

	reflection_registry_add_struct(reflection_registry, PC_Mesh_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_System2_Fields[] =
	{
		{
			.name = "prb",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Default",
			.offset = offsetof(Material_System2, prb)
		},
		{
			.name = "pbr_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_System2, pbr_count)
		},
	};

	 Reflection_Runtime_Struct Material_System2_Runtime_Struct =
	{
		.name = "Material_System2",
		.fields = Material_System2_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_System2)
	};

	reflection_registry_add_struct(reflection_registry, Material_System2_Runtime_Struct);

	Reflection_Runtime_Struct_Field PC_Mesh_New_Fields[] =
	{
		{
			.name = "ubo_buffer_idx",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(PC_Mesh_New, ubo_buffer_idx)
		},
		{
			.name = "padding",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(PC_Mesh_New, padding)
		},
	};

	 Reflection_Runtime_Struct PC_Mesh_New_Runtime_Struct =
	{
		.name = "PC_Mesh_New",
		.fields = PC_Mesh_New_Fields,
		.field_count = 2,
		.struct_size = sizeof(PC_Mesh_New)
	};

	reflection_registry_add_struct(reflection_registry, PC_Mesh_New_Runtime_Struct);

	Reflection_Runtime_Struct_Field PC_Skinned_Mesh_Fields[] =
	{
		{
			.name = "ubo_buffer_idx",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(PC_Skinned_Mesh, ubo_buffer_idx)
		},
		{
			.name = "padding",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(PC_Skinned_Mesh, padding)
		},
	};

	 Reflection_Runtime_Struct PC_Skinned_Mesh_Runtime_Struct =
	{
		.name = "PC_Skinned_Mesh",
		.fields = PC_Skinned_Mesh_Fields,
		.field_count = 2,
		.struct_size = sizeof(PC_Skinned_Mesh)
	};

	reflection_registry_add_struct(reflection_registry, PC_Skinned_Mesh_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Info_Fields[] =
	{
		{
			.name = "shader_file",
			.type = REFLECTION_TYPE_CHAR,
			.type_name = "char",
			.offset = offsetof(Material_Info, shader_file)
		},
		{
			.name = "shader_stage",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Shader_Stage_Type",
			.offset = offsetof(Material_Info, shader_stage)
		},
		{
			.name = "shader_pass",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Shader_Pass_Type",
			.offset = offsetof(Material_Info, shader_pass)
		},
	};

	 Reflection_Runtime_Struct Material_Info_Runtime_Struct =
	{
		.name = "Material_Info",
		.fields = Material_Info_Fields,
		.field_count = 3,
		.struct_size = sizeof(Material_Info)
	};

	reflection_registry_add_struct(reflection_registry, Material_Info_Runtime_Struct);

	Reflection_Runtime_Struct_Field Draw_Data_Fields[] =
	{
		{
			.name = "transform_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Draw_Data, transform_index)
		},
		{
			.name = "material_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Draw_Data, material_index)
		},
	};

	 Reflection_Runtime_Struct Draw_Data_Runtime_Struct =
	{
		.name = "Draw_Data",
		.fields = Draw_Data_Fields,
		.field_count = 2,
		.struct_size = sizeof(Draw_Data)
	};

	reflection_registry_add_struct(reflection_registry, Draw_Data_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Batch_Fields[] =
	{
		{
			.name = "hash_idx",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Batch, hash_idx)
		},
		{
			.name = "definition_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Batch, definition_count)
		},
		{
			.name = "material_stride",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Batch, material_stride)
		},
		{
			.name = "material_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Batch, material_count)
		},
		{
			.name = "push_constant_size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Batch, push_constant_size)
		},
	};

	 Reflection_Runtime_Struct Material_Batch_Runtime_Struct =
	{
		.name = "Material_Batch",
		.fields = Material_Batch_Fields,
		.field_count = 5,
		.struct_size = sizeof(Material_Batch)
	};

	reflection_registry_add_struct(reflection_registry, Material_Batch_Runtime_Struct);

}
