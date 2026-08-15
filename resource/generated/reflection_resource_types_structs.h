#include <stddef.h>
#include "runtime_registry.h"
#include "../resource/resource_types.h"

const char* resources_struct_string_list[] = {
	"Handle", 
	"Asset_MetaData", 
	"Texture_Handle", 
	"Madness_Asset", 
	"Material_Handle", 
	"Madness_Mesh_Handle", 
	"Madness_Mesh_Handle_Internal", 
	"Madness_SkMesh_Handle", 
	"Madness_SkMesh_Handle_Internal", 
	"Transform_Handle", 
	"Sprite_Handle", 
	"Animation_Handle", 
	"Madness_Texture", 
	"Texture_GPU_Upload", 
	"Madness_Texture_Runtime", 
	"Glyph", 
	"Madness_Font", 
	"Madness_Font_Runtime", 
	"PC_General", 
	"PC_Particle", 
	"PC_Shadow_Mapping", 
	"Material_Info", 
	"Material_GPU_Definition", 
	"Material_Asset", 
	"Material_Instance", 
	"Material_Asset_Runtime", 
	"Material_Batch", 
	"Particle_Animation_vec3", 
	"Particle_Animation_vec2", 
	"Particle_Animation_float", 
	"Particle_Emitter", 
	"Particle", 
	"Particle_Mesh", 
	"Mesh_Indirect_Draw", 
	"Skinned_Draw_Data", 
	"Madness_Skinned_Submesh_Instance", 
	"Madness_Animation", 
	"Madness_Skinned_Mesh_Instance", 
	"Madness_SubMesh_Instance", 
	"Madness_Mesh_Instance", 
	"Madness_Skinned_SubMesh", 
	"Madness_SubMesh", 
	"Madness_Mesh", 
	"Madness_Skinned_Mesh", 
	"Madness_Mesh_GPU_Data", 
	"Madness_SkMesh_GPU_Data", 
	"Mesh_GPU_Upload", 
	"Skinned_Mesh_GPU_Upload", 
	"Madness_Mesh_Runtime", 
	"Madness_SkMesh_Runtime", 
	"Material_System", 
	"Sprite_System", 
	"Texture_System", 
	"Scene", 
	"Mesh_System", 
	"Animation_System", 
	"Particle_System", 
	"Render_Packet_3D", 
	"Render_Packet_UI", 
	"Render_Packet_Sprite", 
	"Render_Packet_Particle", 
	"Render_Packet", 
	"Asset_Registry", 
	"Asset_System", 
};

void generate_runtime_structs_resources(Reflection_Registry* reflection_registry)
{
	Reflection_Runtime_Struct_Field Handle_Fields[] =
	{
		{
			.name = "id",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Handle, id)
		},
		{
			.name = "generation",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Handle, generation)
		},
	};

	 Reflection_Runtime_Struct Handle_Runtime_Struct =
	{
		.name = "Handle",
		.fields = Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Handle)
	};

	reflection_registry_add_struct(reflection_registry, Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Asset_MetaData_Fields[] =
	{
		{
			.name = "uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Asset_MetaData, uuid)
		},
		{
			.name = "hash",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Asset_MetaData, hash)
		},
		{
			.name = "type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Asset_Type",
			.offset = offsetof(Asset_MetaData, type)
		},
		{
			.name = "source_file",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Asset_MetaData, source_file)
		},
		{
			.name = "engine_path",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Asset_MetaData, engine_path)
		},
	};

	 Reflection_Runtime_Struct Asset_MetaData_Runtime_Struct =
	{
		.name = "Asset_MetaData",
		.fields = Asset_MetaData_Fields,
		.field_count = 5,
		.struct_size = sizeof(Asset_MetaData)
	};

	reflection_registry_add_struct(reflection_registry, Asset_MetaData_Runtime_Struct);

	Reflection_Runtime_Struct_Field Texture_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Texture_Handle, handle)
		},
		{
			.name = "generation",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Texture_Handle, generation)
		},
	};

	 Reflection_Runtime_Struct Texture_Handle_Runtime_Struct =
	{
		.name = "Texture_Handle",
		.fields = Texture_Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Texture_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Texture_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Asset_Fields[] =
	{
		{
			.name = "path_hash",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Asset, path_hash)
		},
		{
			.name = "reference_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Asset, reference_count)
		},
		{
			.name = "type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Asset_Type",
			.offset = offsetof(Madness_Asset, type)
		},
		{
			.name = "engine_path",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Madness_Asset, engine_path)
		},
	};

	 Reflection_Runtime_Struct Madness_Asset_Runtime_Struct =
	{
		.name = "Madness_Asset",
		.fields = Madness_Asset_Fields,
		.field_count = 4,
		.struct_size = sizeof(Madness_Asset)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Asset_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Handle_Fields[] =
	{
		{
			.name = "buffer_handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Handle, buffer_handle)
		},
	};

	 Reflection_Runtime_Struct Material_Handle_Runtime_Struct =
	{
		.name = "Material_Handle",
		.fields = Material_Handle_Fields,
		.field_count = 1,
		.struct_size = sizeof(Material_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Material_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Mesh_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh_Handle, handle)
		},
	};

	 Reflection_Runtime_Struct Madness_Mesh_Handle_Runtime_Struct =
	{
		.name = "Madness_Mesh_Handle",
		.fields = Madness_Mesh_Handle_Fields,
		.field_count = 1,
		.struct_size = sizeof(Madness_Mesh_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Mesh_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Mesh_Handle_Internal_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh_Handle_Internal, handle)
		},
	};

	 Reflection_Runtime_Struct Madness_Mesh_Handle_Internal_Runtime_Struct =
	{
		.name = "Madness_Mesh_Handle_Internal",
		.fields = Madness_Mesh_Handle_Internal_Fields,
		.field_count = 1,
		.struct_size = sizeof(Madness_Mesh_Handle_Internal)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Mesh_Handle_Internal_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_SkMesh_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SkMesh_Handle, handle)
		},
	};

	 Reflection_Runtime_Struct Madness_SkMesh_Handle_Runtime_Struct =
	{
		.name = "Madness_SkMesh_Handle",
		.fields = Madness_SkMesh_Handle_Fields,
		.field_count = 1,
		.struct_size = sizeof(Madness_SkMesh_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Madness_SkMesh_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_SkMesh_Handle_Internal_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SkMesh_Handle_Internal, handle)
		},
	};

	 Reflection_Runtime_Struct Madness_SkMesh_Handle_Internal_Runtime_Struct =
	{
		.name = "Madness_SkMesh_Handle_Internal",
		.fields = Madness_SkMesh_Handle_Internal_Fields,
		.field_count = 1,
		.struct_size = sizeof(Madness_SkMesh_Handle_Internal)
	};

	reflection_registry_add_struct(reflection_registry, Madness_SkMesh_Handle_Internal_Runtime_Struct);

	Reflection_Runtime_Struct_Field Transform_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Transform_Handle, handle)
		},
		{
			.name = "gen",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Transform_Handle, gen)
		},
	};

	 Reflection_Runtime_Struct Transform_Handle_Runtime_Struct =
	{
		.name = "Transform_Handle",
		.fields = Transform_Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Transform_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Transform_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Sprite_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Sprite_Handle, handle)
		},
		{
			.name = "gen",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Sprite_Handle, gen)
		},
	};

	 Reflection_Runtime_Struct Sprite_Handle_Runtime_Struct =
	{
		.name = "Sprite_Handle",
		.fields = Sprite_Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Sprite_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Sprite_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Animation_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Animation_Handle, handle)
		},
		{
			.name = "gen",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Animation_Handle, gen)
		},
	};

	 Reflection_Runtime_Struct Animation_Handle_Runtime_Struct =
	{
		.name = "Animation_Handle",
		.fields = Animation_Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Animation_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Animation_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Texture_Fields[] =
	{
		{
			.name = "width",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Texture, width)
		},
		{
			.name = "height",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Texture, height)
		},
		{
			.name = "channels",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Texture, channels)
		},
		{
			.name = "format",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Texture_Format",
			.offset = offsetof(Madness_Texture, format)
		},
		{
			.name = "pixels_size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Texture, pixels_size)
		},
		{
			.name = "type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Asset_Type",
			.offset = offsetof(Madness_Texture, type)
		},
		{
			.name = "font_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Texture, font_index)
		},
		{
			.name = "bindless_slot_query",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Texture, bindless_slot_query)
		},
		{
			.name = "bindless_slot",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Texture, bindless_slot)
		},
		{
			.name = "generation",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Texture, generation)
		},
	};

	 Reflection_Runtime_Struct Madness_Texture_Runtime_Struct =
	{
		.name = "Madness_Texture",
		.fields = Madness_Texture_Fields,
		.field_count = 10,
		.struct_size = sizeof(Madness_Texture)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Texture_Runtime_Struct);

	Reflection_Runtime_Struct_Field Texture_GPU_Upload_Fields[] =
	{
		{
			.name = "madness_texture",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Texture",
			.offset = offsetof(Texture_GPU_Upload, madness_texture)
		},
		{
			.name = "pixel_data",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Texture_GPU_Upload, pixel_data)
		},
	};

	 Reflection_Runtime_Struct Texture_GPU_Upload_Runtime_Struct =
	{
		.name = "Texture_GPU_Upload",
		.fields = Texture_GPU_Upload_Fields,
		.field_count = 2,
		.struct_size = sizeof(Texture_GPU_Upload)
	};

	reflection_registry_add_struct(reflection_registry, Texture_GPU_Upload_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Texture_Runtime_Fields[] =
	{
		{
			.name = "version",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Texture_Runtime, version)
		},
		{
			.name = "texture",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Texture",
			.offset = offsetof(Madness_Texture_Runtime, texture)
		},
		{
			.name = "pixel_data",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Texture_Runtime, pixel_data)
		},
	};

	 Reflection_Runtime_Struct Madness_Texture_Runtime_Runtime_Struct =
	{
		.name = "Madness_Texture_Runtime",
		.fields = Madness_Texture_Runtime_Fields,
		.field_count = 3,
		.struct_size = sizeof(Madness_Texture_Runtime)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Texture_Runtime_Runtime_Struct);

	Reflection_Runtime_Struct_Field Glyph_Fields[] =
	{
		{
			.name = "width",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Glyph, width)
		},
		{
			.name = "xoff",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Glyph, xoff)
		},
		{
			.name = "advance",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Glyph, advance)
		},
		{
			.name = "u0",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Glyph, u0)
		},
	};

	 Reflection_Runtime_Struct Glyph_Runtime_Struct =
	{
		.name = "Glyph",
		.fields = Glyph_Fields,
		.field_count = 4,
		.struct_size = sizeof(Glyph)
	};

	reflection_registry_add_struct(reflection_registry, Glyph_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Font_Fields[] =
	{
		{
			.name = "glyphs",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Glyph",
			.offset = offsetof(Madness_Font, glyphs)
		},
	};

	 Reflection_Runtime_Struct Madness_Font_Runtime_Struct =
	{
		.name = "Madness_Font",
		.fields = Madness_Font_Fields,
		.field_count = 1,
		.struct_size = sizeof(Madness_Font)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Font_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Font_Runtime_Fields[] =
	{
		{
			.name = "version",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Font_Runtime, version)
		},
		{
			.name = "font_texture",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Font",
			.offset = offsetof(Madness_Font_Runtime, font_texture)
		},
		{
			.name = "texture",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Texture",
			.offset = offsetof(Madness_Font_Runtime, texture)
		},
		{
			.name = "pixel_data",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Font_Runtime, pixel_data)
		},
	};

	 Reflection_Runtime_Struct Madness_Font_Runtime_Runtime_Struct =
	{
		.name = "Madness_Font_Runtime",
		.fields = Madness_Font_Runtime_Fields,
		.field_count = 4,
		.struct_size = sizeof(Madness_Font_Runtime)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Font_Runtime_Runtime_Struct);

	Reflection_Runtime_Struct_Field PC_General_Fields[] =
	{
	};

	 Reflection_Runtime_Struct PC_General_Runtime_Struct =
	{
		.name = "PC_General",
		.fields = PC_General_Fields,
		.field_count = 0,
		.struct_size = sizeof(PC_General)
	};

	reflection_registry_add_struct(reflection_registry, PC_General_Runtime_Struct);

	Reflection_Runtime_Struct_Field PC_Particle_Fields[] =
	{
	};

	 Reflection_Runtime_Struct PC_Particle_Runtime_Struct =
	{
		.name = "PC_Particle",
		.fields = PC_Particle_Fields,
		.field_count = 0,
		.struct_size = sizeof(PC_Particle)
	};

	reflection_registry_add_struct(reflection_registry, PC_Particle_Runtime_Struct);

	Reflection_Runtime_Struct_Field PC_Shadow_Mapping_Fields[] =
	{
		{
			.name = "light_matrix",
			.type = REFLECTION_TYPE_MAT4,
			.type_name = "mat4",
			.offset = offsetof(PC_Shadow_Mapping, light_matrix)
		},
	};

	 Reflection_Runtime_Struct PC_Shadow_Mapping_Runtime_Struct =
	{
		.name = "PC_Shadow_Mapping",
		.fields = PC_Shadow_Mapping_Fields,
		.field_count = 1,
		.struct_size = sizeof(PC_Shadow_Mapping)
	};

	reflection_registry_add_struct(reflection_registry, PC_Shadow_Mapping_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Info_Fields[] =
	{
		{
			.name = "shader_name",
			.type = REFLECTION_TYPE_PATH_STRING,
			.type_name = "Path_String",
			.offset = offsetof(Material_Info, shader_name)
		},
		{
			.name = "material_name",
			.type = REFLECTION_TYPE_PATH_STRING,
			.type_name = "Path_String",
			.offset = offsetof(Material_Info, material_name)
		},
		{
			.name = "renderpass",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Shader_Renderpass_Type",
			.offset = offsetof(Material_Info, renderpass)
		},
		{
			.name = "transluency",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Shader_Transluency_Type",
			.offset = offsetof(Material_Info, transluency)
		},
		{
			.name = "mesh_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Shader_Mesh_Type",
			.offset = offsetof(Material_Info, mesh_type)
		},
	};

	 Reflection_Runtime_Struct Material_Info_Runtime_Struct =
	{
		.name = "Material_Info",
		.fields = Material_Info_Fields,
		.field_count = 5,
		.struct_size = sizeof(Material_Info)
	};

	reflection_registry_add_struct(reflection_registry, Material_Info_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_GPU_Definition_Fields[] =
	{
		{
			.name = "field_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_GPU_Definition, field_count)
		},
		{
			.name = "struct_size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_GPU_Definition, struct_size)
		},
		{
			.name = "name_hashes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Material_GPU_Definition, name_hashes)
		},
		{
			.name = "field_offsets",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_GPU_Definition, field_offsets)
		},
	};

	 Reflection_Runtime_Struct Material_GPU_Definition_Runtime_Struct =
	{
		.name = "Material_GPU_Definition",
		.fields = Material_GPU_Definition_Fields,
		.field_count = 4,
		.struct_size = sizeof(Material_GPU_Definition)
	};

	reflection_registry_add_struct(reflection_registry, Material_GPU_Definition_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Asset_Fields[] =
	{
		{
			.name = "material_info",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Info",
			.offset = offsetof(Material_Asset, material_info)
		},
		{
			.name = "material_gpu_definition",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_GPU_Definition",
			.offset = offsetof(Material_Asset, material_gpu_definition)
		},
	};

	 Reflection_Runtime_Struct Material_Asset_Runtime_Struct =
	{
		.name = "Material_Asset",
		.fields = Material_Asset_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_Asset)
	};

	reflection_registry_add_struct(reflection_registry, Material_Asset_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Instance_Fields[] =
	{
		{
			.name = "material_asset_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Instance, material_asset_uuid)
		},
		{
			.name = "data_size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Material_Instance, data_size)
		},
	};

	 Reflection_Runtime_Struct Material_Instance_Runtime_Struct =
	{
		.name = "Material_Instance",
		.fields = Material_Instance_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_Instance)
	};

	reflection_registry_add_struct(reflection_registry, Material_Instance_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Asset_Runtime_Fields[] =
	{
		{
			.name = "version",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Asset_Runtime, version)
		},
		{
			.name = "asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Asset",
			.offset = offsetof(Material_Asset_Runtime, asset)
		},
	};

	 Reflection_Runtime_Struct Material_Asset_Runtime_Runtime_Struct =
	{
		.name = "Material_Asset_Runtime",
		.fields = Material_Asset_Runtime_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_Asset_Runtime)
	};

	reflection_registry_add_struct(reflection_registry, Material_Asset_Runtime_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Batch_Fields[] =
	{
		{
			.name = "material_asset_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Batch, material_asset_uuid)
		},
		{
			.name = "material_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Asset",
			.offset = offsetof(Material_Batch, material_asset)
		},
	};

	 Reflection_Runtime_Struct Material_Batch_Runtime_Struct =
	{
		.name = "Material_Batch",
		.fields = Material_Batch_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_Batch)
	};

	reflection_registry_add_struct(reflection_registry, Material_Batch_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Animation_vec3_Fields[] =
	{
		{
			.name = "time",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Particle_Animation_vec3, time)
		},
		{
			.name = "value",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Animation_vec3, value)
		},
	};

	 Reflection_Runtime_Struct Particle_Animation_vec3_Runtime_Struct =
	{
		.name = "Particle_Animation_vec3",
		.fields = Particle_Animation_vec3_Fields,
		.field_count = 2,
		.struct_size = sizeof(Particle_Animation_vec3)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Animation_vec3_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Animation_vec2_Fields[] =
	{
		{
			.name = "time",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Particle_Animation_vec2, time)
		},
		{
			.name = "value",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Particle_Animation_vec2, value)
		},
	};

	 Reflection_Runtime_Struct Particle_Animation_vec2_Runtime_Struct =
	{
		.name = "Particle_Animation_vec2",
		.fields = Particle_Animation_vec2_Fields,
		.field_count = 2,
		.struct_size = sizeof(Particle_Animation_vec2)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Animation_vec2_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Animation_float_Fields[] =
	{
		{
			.name = "time",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Particle_Animation_float, time)
		},
		{
			.name = "value",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Particle_Animation_float, value)
		},
	};

	 Reflection_Runtime_Struct Particle_Animation_float_Runtime_Struct =
	{
		.name = "Particle_Animation_float",
		.fields = Particle_Animation_float_Fields,
		.field_count = 2,
		.struct_size = sizeof(Particle_Animation_float)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Animation_float_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Emitter_Fields[] =
	{
		{
			.name = "nothingl",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Particle_Emitter, nothingl)
		},
	};

	 Reflection_Runtime_Struct Particle_Emitter_Runtime_Struct =
	{
		.name = "Particle_Emitter",
		.fields = Particle_Emitter_Fields,
		.field_count = 1,
		.struct_size = sizeof(Particle_Emitter)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Emitter_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Fields[] =
	{
		{
			.name = "position",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle, position)
		},
		{
			.name = "rotation",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Particle, rotation)
		},
		{
			.name = "scale",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Particle, scale)
		},
		{
			.name = "life_left",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Particle, life_left)
		},
		{
			.name = "velocity",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle, velocity)
		},
		{
			.name = "texture_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Texture_Handle",
			.offset = offsetof(Particle, texture_handle)
		},
		{
			.name = "tex_offset",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Particle, tex_offset)
		},
		{
			.name = "tex_size",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Particle, tex_size)
		},
	};

	 Reflection_Runtime_Struct Particle_Runtime_Struct =
	{
		.name = "Particle",
		.fields = Particle_Fields,
		.field_count = 8,
		.struct_size = sizeof(Particle)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Mesh_Fields[] =
	{
		{
			.name = "position",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3",
			.offset = offsetof(Particle_Mesh, position)
		},
		{
			.name = "rotation",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Mesh, rotation)
		},
		{
			.name = "scale",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Mesh, scale)
		},
		{
			.name = "lifetime_left",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Particle_Mesh, lifetime_left)
		},
		{
			.name = "Mesh",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Mesh_Handle",
			.offset = offsetof(Particle_Mesh, Mesh)
		},
		{
			.name = "Textures",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Texture_Handle",
			.offset = offsetof(Particle_Mesh, Textures)
		},
		{
			.name = "tex_offset",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Particle_Mesh, tex_offset)
		},
	};

	 Reflection_Runtime_Struct Particle_Mesh_Runtime_Struct =
	{
		.name = "Particle_Mesh",
		.fields = Particle_Mesh_Fields,
		.field_count = 7,
		.struct_size = sizeof(Particle_Mesh)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Mesh_Runtime_Struct);

	Reflection_Runtime_Struct_Field Mesh_Indirect_Draw_Fields[] =
	{
		{
			.name = "vertex_count_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_Indirect_Draw, vertex_count_offset)
		},
		{
			.name = "index_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_Indirect_Draw, index_offset)
		},
		{
			.name = "index_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_Indirect_Draw, index_count)
		},
	};

	 Reflection_Runtime_Struct Mesh_Indirect_Draw_Runtime_Struct =
	{
		.name = "Mesh_Indirect_Draw",
		.fields = Mesh_Indirect_Draw_Fields,
		.field_count = 3,
		.struct_size = sizeof(Mesh_Indirect_Draw)
	};

	reflection_registry_add_struct(reflection_registry, Mesh_Indirect_Draw_Runtime_Struct);

	Reflection_Runtime_Struct_Field Skinned_Draw_Data_Fields[] =
	{
		{
			.name = "joint_idx",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Skinned_Draw_Data, joint_idx)
		},
		{
			.name = "weight_idx",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Skinned_Draw_Data, weight_idx)
		},
		{
			.name = "skinned_matrix_idx",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Skinned_Draw_Data, skinned_matrix_idx)
		},
	};

	 Reflection_Runtime_Struct Skinned_Draw_Data_Runtime_Struct =
	{
		.name = "Skinned_Draw_Data",
		.fields = Skinned_Draw_Data_Fields,
		.field_count = 3,
		.struct_size = sizeof(Skinned_Draw_Data)
	};

	reflection_registry_add_struct(reflection_registry, Skinned_Draw_Data_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Skinned_Submesh_Instance_Fields[] =
	{
		{
			.name = "mesh_indirect_draw",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Mesh_Indirect_Draw",
			.offset = offsetof(Madness_Skinned_Submesh_Instance, mesh_indirect_draw)
		},
		{
			.name = "skinned_draw_data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Skinned_Draw_Data",
			.offset = offsetof(Madness_Skinned_Submesh_Instance, skinned_draw_data)
		},
		{
			.name = "material_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Handle",
			.offset = offsetof(Madness_Skinned_Submesh_Instance, material_handle)
		},
		{
			.name = "parent_transform_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Transform_Handle",
			.offset = offsetof(Madness_Skinned_Submesh_Instance, parent_transform_handle)
		},
	};

	 Reflection_Runtime_Struct Madness_Skinned_Submesh_Instance_Runtime_Struct =
	{
		.name = "Madness_Skinned_Submesh_Instance",
		.fields = Madness_Skinned_Submesh_Instance_Fields,
		.field_count = 4,
		.struct_size = sizeof(Madness_Skinned_Submesh_Instance)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Skinned_Submesh_Instance_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Animation_Fields[] =
	{
		{
			.name = "gpu_matrix",
			.type = REFLECTION_TYPE_MAT4,
			.type_name = "mat4s",
			.offset = offsetof(Madness_Animation, gpu_matrix)
		},
		{
			.name = "skinned_matrix_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Animation, skinned_matrix_offset)
		},
		{
			.name = "local_translation",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Madness_Animation, local_translation)
		},
		{
			.name = "local_scale",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Madness_Animation, local_scale)
		},
		{
			.name = "joint_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Animation, joint_count)
		},
		{
			.name = "current_animation_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Animation, current_animation_index)
		},
		{
			.name = "current_time",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Madness_Animation, current_time)
		},
		{
			.name = "looping",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Madness_Animation, looping)
		},
	};

	 Reflection_Runtime_Struct Madness_Animation_Runtime_Struct =
	{
		.name = "Madness_Animation",
		.fields = Madness_Animation_Fields,
		.field_count = 8,
		.struct_size = sizeof(Madness_Animation)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Animation_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Skinned_Mesh_Instance_Fields[] =
	{
		{
			.name = "mesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Skinned_Mesh_Instance, mesh_count)
		},
		{
			.name = "submesh_instances",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Skinned_Submesh_Instance",
			.offset = offsetof(Madness_Skinned_Mesh_Instance, submesh_instances)
		},
		{
			.name = "skinned_mesh_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SkMesh_Handle_Internal",
			.offset = offsetof(Madness_Skinned_Mesh_Instance, skinned_mesh_asset)
		},
		{
			.name = "transform_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Transform_Handle",
			.offset = offsetof(Madness_Skinned_Mesh_Instance, transform_handle)
		},
		{
			.name = "animation_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Animation_Handle",
			.offset = offsetof(Madness_Skinned_Mesh_Instance, animation_handle)
		},
		{
			.name = "skinned_matrix_count_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Skinned_Mesh_Instance, skinned_matrix_count_offset)
		},
	};

	 Reflection_Runtime_Struct Madness_Skinned_Mesh_Instance_Runtime_Struct =
	{
		.name = "Madness_Skinned_Mesh_Instance",
		.fields = Madness_Skinned_Mesh_Instance_Fields,
		.field_count = 6,
		.struct_size = sizeof(Madness_Skinned_Mesh_Instance)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Skinned_Mesh_Instance_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_SubMesh_Instance_Fields[] =
	{
		{
			.name = "mesh_indirect_draw",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Mesh_Indirect_Draw",
			.offset = offsetof(Madness_SubMesh_Instance, mesh_indirect_draw)
		},
		{
			.name = "material_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Handle",
			.offset = offsetof(Madness_SubMesh_Instance, material_handle)
		},
		{
			.name = "parent_transform_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Transform_Handle",
			.offset = offsetof(Madness_SubMesh_Instance, parent_transform_handle)
		},
	};

	 Reflection_Runtime_Struct Madness_SubMesh_Instance_Runtime_Struct =
	{
		.name = "Madness_SubMesh_Instance",
		.fields = Madness_SubMesh_Instance_Fields,
		.field_count = 3,
		.struct_size = sizeof(Madness_SubMesh_Instance)
	};

	reflection_registry_add_struct(reflection_registry, Madness_SubMesh_Instance_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Mesh_Instance_Fields[] =
	{
		{
			.name = "mesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh_Instance, mesh_count)
		},
		{
			.name = "mesh_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Mesh_Handle_Internal",
			.offset = offsetof(Madness_Mesh_Instance, mesh_asset)
		},
		{
			.name = "transform_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Transform_Handle",
			.offset = offsetof(Madness_Mesh_Instance, transform_handle)
		},
		{
			.name = "submesh_instances",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SubMesh_Instance",
			.offset = offsetof(Madness_Mesh_Instance, submesh_instances)
		},
	};

	 Reflection_Runtime_Struct Madness_Mesh_Instance_Runtime_Struct =
	{
		.name = "Madness_Mesh_Instance",
		.fields = Madness_Mesh_Instance_Fields,
		.field_count = 4,
		.struct_size = sizeof(Madness_Mesh_Instance)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Mesh_Instance_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Skinned_SubMesh_Fields[] =
	{
		{
			.name = "joint_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Skinned_SubMesh, joint_bytes)
		},
		{
			.name = "weight_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Skinned_SubMesh, weight_bytes)
		},
		{
			.name = "joint_offset_vec4",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Skinned_SubMesh, joint_offset_vec4)
		},
		{
			.name = "joint_offset_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Skinned_SubMesh, joint_offset_bytes)
		},
		{
			.name = "weight_offset_vec4",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Skinned_SubMesh, weight_offset_vec4)
		},
		{
			.name = "weight_offset_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Skinned_SubMesh, weight_offset_bytes)
		},
	};

	 Reflection_Runtime_Struct Madness_Skinned_SubMesh_Runtime_Struct =
	{
		.name = "Madness_Skinned_SubMesh",
		.fields = Madness_Skinned_SubMesh_Fields,
		.field_count = 6,
		.struct_size = sizeof(Madness_Skinned_SubMesh)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Skinned_SubMesh_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_SubMesh_Fields[] =
	{
		{
			.name = "tangent_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_SubMesh, tangent_bytes)
		},
		{
			.name = "vertex_color_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_SubMesh, vertex_color_bytes)
		},
		{
			.name = "vertex_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_SubMesh, vertex_bytes)
		},
		{
			.name = "normal_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_SubMesh, normal_bytes)
		},
		{
			.name = "uv_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_SubMesh, uv_bytes)
		},
		{
			.name = "indices_bytes",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_SubMesh, indices_bytes)
		},
		{
			.name = "vertex_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh, vertex_count)
		},
		{
			.name = "index_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh, index_count)
		},
		{
			.name = "index_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Index_Type",
			.offset = offsetof(Madness_SubMesh, index_type)
		},
		{
			.name = "vertex_count_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh, vertex_count_offset)
		},
		{
			.name = "index_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh, index_offset)
		},
		{
			.name = "vertex_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh, vertex_offset)
		},
		{
			.name = "tangent_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh, tangent_offset)
		},
		{
			.name = "vertex_color_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh, vertex_color_offset)
		},
		{
			.name = "normal_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh, normal_offset)
		},
		{
			.name = "uv_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh, uv_offset)
		},
	};

	 Reflection_Runtime_Struct Madness_SubMesh_Runtime_Struct =
	{
		.name = "Madness_SubMesh",
		.fields = Madness_SubMesh_Fields,
		.field_count = 16,
		.struct_size = sizeof(Madness_SubMesh)
	};

	reflection_registry_add_struct(reflection_registry, Madness_SubMesh_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Mesh_Fields[] =
	{
		{
			.name = "mesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh, mesh_count)
		},
		{
			.name = "mesh_data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SubMesh",
			.offset = offsetof(Madness_Mesh, mesh_data)
		},
		{
			.name = "material_instance",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Instance",
			.offset = offsetof(Madness_Mesh, material_instance)
		},
		{
			.name = "material_handles",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Handle",
			.offset = offsetof(Madness_Mesh, material_handles)
		},
		{
			.name = "path_hash",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Madness_Mesh, path_hash)
		},
		{
			.name = "engine_path",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Madness_Mesh, engine_path)
		},
		{
			.name = "generation",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh, generation)
		},
		{
			.name = "reference_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh, reference_count)
		},
	};

	 Reflection_Runtime_Struct Madness_Mesh_Runtime_Struct =
	{
		.name = "Madness_Mesh",
		.fields = Madness_Mesh_Fields,
		.field_count = 8,
		.struct_size = sizeof(Madness_Mesh)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Mesh_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Skinned_Mesh_Fields[] =
	{
		{
			.name = "mesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Skinned_Mesh, mesh_count)
		},
		{
			.name = "mesh_data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SubMesh",
			.offset = offsetof(Madness_Skinned_Mesh, mesh_data)
		},
		{
			.name = "material_instance",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Instance",
			.offset = offsetof(Madness_Skinned_Mesh, material_instance)
		},
		{
			.name = "skinned_mesh_data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Skinned_SubMesh",
			.offset = offsetof(Madness_Skinned_Mesh, skinned_mesh_data)
		},
	};

	 Reflection_Runtime_Struct Madness_Skinned_Mesh_Runtime_Struct =
	{
		.name = "Madness_Skinned_Mesh",
		.fields = Madness_Skinned_Mesh_Fields,
		.field_count = 4,
		.struct_size = sizeof(Madness_Skinned_Mesh)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Skinned_Mesh_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Mesh_GPU_Data_Fields[] =
	{
		{
			.name = "tangent",
			.type = REFLECTION_TYPE_VEC4,
			.type_name = "vec4s",
			.offset = offsetof(Madness_Mesh_GPU_Data, tangent)
		},
		{
			.name = "vertex_color",
			.type = REFLECTION_TYPE_VEC4,
			.type_name = "vec4s",
			.offset = offsetof(Madness_Mesh_GPU_Data, vertex_color)
		},
		{
			.name = "vertex",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Madness_Mesh_GPU_Data, vertex)
		},
		{
			.name = "normal",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Madness_Mesh_GPU_Data, normal)
		},
		{
			.name = "uv",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Madness_Mesh_GPU_Data, uv)
		},
		{
			.name = "indices",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Mesh_GPU_Data, indices)
		},
	};

	 Reflection_Runtime_Struct Madness_Mesh_GPU_Data_Runtime_Struct =
	{
		.name = "Madness_Mesh_GPU_Data",
		.fields = Madness_Mesh_GPU_Data_Fields,
		.field_count = 6,
		.struct_size = sizeof(Madness_Mesh_GPU_Data)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Mesh_GPU_Data_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_SkMesh_GPU_Data_Fields[] =
	{
		{
			.name = "joints",
			.type = REFLECTION_TYPE_VEC4,
			.type_name = "vec4s",
			.offset = offsetof(Madness_SkMesh_GPU_Data, joints)
		},
		{
			.name = "weights",
			.type = REFLECTION_TYPE_VEC4,
			.type_name = "vec4s",
			.offset = offsetof(Madness_SkMesh_GPU_Data, weights)
		},
	};

	 Reflection_Runtime_Struct Madness_SkMesh_GPU_Data_Runtime_Struct =
	{
		.name = "Madness_SkMesh_GPU_Data",
		.fields = Madness_SkMesh_GPU_Data_Fields,
		.field_count = 2,
		.struct_size = sizeof(Madness_SkMesh_GPU_Data)
	};

	reflection_registry_add_struct(reflection_registry, Madness_SkMesh_GPU_Data_Runtime_Struct);

	Reflection_Runtime_Struct_Field Mesh_GPU_Upload_Fields[] =
	{
		{
			.name = "submesh",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SubMesh",
			.offset = offsetof(Mesh_GPU_Upload, submesh)
		},
		{
			.name = "gpu_data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Mesh_GPU_Data",
			.offset = offsetof(Mesh_GPU_Upload, gpu_data)
		},
	};

	 Reflection_Runtime_Struct Mesh_GPU_Upload_Runtime_Struct =
	{
		.name = "Mesh_GPU_Upload",
		.fields = Mesh_GPU_Upload_Fields,
		.field_count = 2,
		.struct_size = sizeof(Mesh_GPU_Upload)
	};

	reflection_registry_add_struct(reflection_registry, Mesh_GPU_Upload_Runtime_Struct);

	Reflection_Runtime_Struct_Field Skinned_Mesh_GPU_Upload_Fields[] =
	{
		{
			.name = "skinned_submesh",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Skinned_SubMesh",
			.offset = offsetof(Skinned_Mesh_GPU_Upload, skinned_submesh)
		},
		{
			.name = "skinned_gpu_data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SkMesh_GPU_Data",
			.offset = offsetof(Skinned_Mesh_GPU_Upload, skinned_gpu_data)
		},
	};

	 Reflection_Runtime_Struct Skinned_Mesh_GPU_Upload_Runtime_Struct =
	{
		.name = "Skinned_Mesh_GPU_Upload",
		.fields = Skinned_Mesh_GPU_Upload_Fields,
		.field_count = 2,
		.struct_size = sizeof(Skinned_Mesh_GPU_Upload)
	};

	reflection_registry_add_struct(reflection_registry, Skinned_Mesh_GPU_Upload_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Mesh_Runtime_Fields[] =
	{
		{
			.name = "version",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh_Runtime, version)
		},
		{
			.name = "mesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh_Runtime, mesh_count)
		},
		{
			.name = "submeshes",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SubMesh",
			.offset = offsetof(Madness_Mesh_Runtime, submeshes)
		},
		{
			.name = "mesh_gpu_upload",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Mesh_GPU_Data",
			.offset = offsetof(Madness_Mesh_Runtime, mesh_gpu_upload)
		},
		{
			.name = "material_instance",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Instance",
			.offset = offsetof(Madness_Mesh_Runtime, material_instance)
		},
	};

	 Reflection_Runtime_Struct Madness_Mesh_Runtime_Runtime_Struct =
	{
		.name = "Madness_Mesh_Runtime",
		.fields = Madness_Mesh_Runtime_Fields,
		.field_count = 5,
		.struct_size = sizeof(Madness_Mesh_Runtime)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Mesh_Runtime_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_SkMesh_Runtime_Fields[] =
	{
		{
			.name = "version",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SkMesh_Runtime, version)
		},
		{
			.name = "mesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SkMesh_Runtime, mesh_count)
		},
		{
			.name = "submeshes",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SubMesh",
			.offset = offsetof(Madness_SkMesh_Runtime, submeshes)
		},
		{
			.name = "mesh_gpu_upload",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Mesh_GPU_Data",
			.offset = offsetof(Madness_SkMesh_Runtime, mesh_gpu_upload)
		},
		{
			.name = "material_instance",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Instance",
			.offset = offsetof(Madness_SkMesh_Runtime, material_instance)
		},
		{
			.name = "skinned_submeshes",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Skinned_SubMesh",
			.offset = offsetof(Madness_SkMesh_Runtime, skinned_submeshes)
		},
		{
			.name = "skmesh_gpu_upload",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SkMesh_GPU_Data",
			.offset = offsetof(Madness_SkMesh_Runtime, skmesh_gpu_upload)
		},
	};

	 Reflection_Runtime_Struct Madness_SkMesh_Runtime_Runtime_Struct =
	{
		.name = "Madness_SkMesh_Runtime",
		.fields = Madness_SkMesh_Runtime_Fields,
		.field_count = 7,
		.struct_size = sizeof(Madness_SkMesh_Runtime)
	};

	reflection_registry_add_struct(reflection_registry, Madness_SkMesh_Runtime_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_System_Fields[] =
	{
		{
			.name = "material_batch",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Batch",
			.offset = offsetof(Material_System, material_batch)
		},
		{
			.name = "material_batch_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_System, material_batch_count)
		},
		{
			.name = "material_madness_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Asset",
			.offset = offsetof(Material_System, material_madness_asset)
		},
		{
			.name = "material_madness_asset_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_System, material_madness_asset_count)
		},
	};

	 Reflection_Runtime_Struct Material_System_Runtime_Struct =
	{
		.name = "Material_System",
		.fields = Material_System_Fields,
		.field_count = 4,
		.struct_size = sizeof(Material_System)
	};

	reflection_registry_add_struct(reflection_registry, Material_System_Runtime_Struct);

	Reflection_Runtime_Struct_Field Sprite_System_Fields[] =
	{
		{
			.name = "screen_size",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Sprite_System, screen_size)
		},
		{
			.name = "sprite_indices",
			.type = REFLECTION_TYPE_U16,
			.type_name = "u16",
			.offset = offsetof(Sprite_System, sprite_indices)
		},
	};

	 Reflection_Runtime_Struct Sprite_System_Runtime_Struct =
	{
		.name = "Sprite_System",
		.fields = Sprite_System_Fields,
		.field_count = 2,
		.struct_size = sizeof(Sprite_System)
	};

	reflection_registry_add_struct(reflection_registry, Sprite_System_Runtime_Struct);

	Reflection_Runtime_Struct_Field Texture_System_Fields[] =
	{
		{
			.name = "default_texture_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Texture_Handle",
			.offset = offsetof(Texture_System, default_texture_handle)
		},
		{
			.name = "textures",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Texture",
			.offset = offsetof(Texture_System, textures)
		},
		{
			.name = "font_textures",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Font",
			.offset = offsetof(Texture_System, font_textures)
		},
		{
			.name = "in_use_textures_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Texture_System, in_use_textures_count)
		},
		{
			.name = "max_textures",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Texture_System, max_textures)
		},
		{
			.name = "texture_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Asset",
			.offset = offsetof(Texture_System, texture_asset)
		},
		{
			.name = "texture_asset_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Texture_System, texture_asset_count)
		},
	};

	 Reflection_Runtime_Struct Texture_System_Runtime_Struct =
	{
		.name = "Texture_System",
		.fields = Texture_System_Fields,
		.field_count = 7,
		.struct_size = sizeof(Texture_System)
	};

	reflection_registry_add_struct(reflection_registry, Texture_System_Runtime_Struct);

	Reflection_Runtime_Struct_Field Scene_Fields[] =
	{
		{
			.name = "scene_name",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Scene, scene_name)
		},
		{
			.name = "asset_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Scene, asset_uuid)
		},
		{
			.name = "uuid_counts",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Scene, uuid_counts)
		},
		{
			.name = "transform_count",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Scene, transform_count)
		},
		{
			.name = "world_transforms",
			.type = REFLECTION_TYPE_MAT4,
			.type_name = "mat4s",
			.offset = offsetof(Scene, world_transforms)
		},
	};

	 Reflection_Runtime_Struct Scene_Runtime_Struct =
	{
		.name = "Scene",
		.fields = Scene_Fields,
		.field_count = 5,
		.struct_size = sizeof(Scene)
	};

	reflection_registry_add_struct(reflection_registry, Scene_Runtime_Struct);

	Reflection_Runtime_Struct_Field Mesh_System_Fields[] =
	{
		{
			.name = "madness_mesh",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Mesh",
			.offset = offsetof(Mesh_System, madness_mesh)
		},
		{
			.name = "madness_mesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_System, madness_mesh_count)
		},
		{
			.name = "madness_skinned_mesh",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Skinned_Mesh",
			.offset = offsetof(Mesh_System, madness_skinned_mesh)
		},
		{
			.name = "madness_sk_mesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_System, madness_sk_mesh_count)
		},
		{
			.name = "mesh_instance",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Mesh_Instance",
			.offset = offsetof(Mesh_System, mesh_instance)
		},
		{
			.name = "mesh_instance_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_System, mesh_instance_count)
		},
		{
			.name = "skinned_mesh_instance",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Skinned_Mesh_Instance",
			.offset = offsetof(Mesh_System, skinned_mesh_instance)
		},
		{
			.name = "skinned_mesh_instance_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_System, skinned_mesh_instance_count)
		},
		{
			.name = "vertex_byte_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, vertex_byte_size)
		},
		{
			.name = "vertex_count_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, vertex_count_size)
		},
		{
			.name = "index_byte_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, index_byte_size)
		},
		{
			.name = "index_count_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, index_count_size)
		},
		{
			.name = "normals_byte_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, normals_byte_size)
		},
		{
			.name = "tangent_byte_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, tangent_byte_size)
		},
		{
			.name = "uv_byte_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, uv_byte_size)
		},
		{
			.name = "vertex_color_byte_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, vertex_color_byte_size)
		},
		{
			.name = "joints_byte_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, joints_byte_size)
		},
		{
			.name = "weight_byte_size",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Mesh_System, weight_byte_size)
		},
		{
			.name = "skinned_madness_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Asset",
			.offset = offsetof(Mesh_System, skinned_madness_asset)
		},
		{
			.name = "skinned_madness_asset_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_System, skinned_madness_asset_count)
		},
	};

	 Reflection_Runtime_Struct Mesh_System_Runtime_Struct =
	{
		.name = "Mesh_System",
		.fields = Mesh_System_Fields,
		.field_count = 20,
		.struct_size = sizeof(Mesh_System)
	};

	reflection_registry_add_struct(reflection_registry, Mesh_System_Runtime_Struct);

	Reflection_Runtime_Struct_Field Animation_System_Fields[] =
	{
		{
			.name = "skinned_matrix_offset_count",
			.type = REFLECTION_TYPE_SIZE_T,
			.type_name = "size_t",
			.offset = offsetof(Animation_System, skinned_matrix_offset_count)
		},
		{
			.name = "animation_data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Animation",
			.offset = offsetof(Animation_System, animation_data)
		},
		{
			.name = "animation_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Animation_System, animation_count)
		},
	};

	 Reflection_Runtime_Struct Animation_System_Runtime_Struct =
	{
		.name = "Animation_System",
		.fields = Animation_System_Fields,
		.field_count = 3,
		.struct_size = sizeof(Animation_System)
	};

	reflection_registry_add_struct(reflection_registry, Animation_System_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_System_Fields[] =
	{
		{
			.name = "particles",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle",
			.offset = offsetof(Particle_System, particles)
		},
		{
			.name = "particles_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_System, particles_count)
		},
	};

	 Reflection_Runtime_Struct Particle_System_Runtime_Struct =
	{
		.name = "Particle_System",
		.fields = Particle_System_Fields,
		.field_count = 2,
		.struct_size = sizeof(Particle_System)
	};

	reflection_registry_add_struct(reflection_registry, Particle_System_Runtime_Struct);

	Reflection_Runtime_Struct_Field Render_Packet_3D_Fields[] =
	{
		{
			.name = "material_batch",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Batch",
			.offset = offsetof(Render_Packet_3D, material_batch)
		},
		{
			.name = "material_batch_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Render_Packet_3D, material_batch_count)
		},
		{
			.name = "mesh_instances",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Mesh_Instance",
			.offset = offsetof(Render_Packet_3D, mesh_instances)
		},
		{
			.name = "mesh_instances_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Render_Packet_3D, mesh_instances_count)
		},
		{
			.name = "skinned_instances",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Skinned_Mesh_Instance",
			.offset = offsetof(Render_Packet_3D, skinned_instances)
		},
		{
			.name = "skinned_instances_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Render_Packet_3D, skinned_instances_count)
		},
		{
			.name = "world_space_matrix_array",
			.type = REFLECTION_TYPE_MAT4,
			.type_name = "mat4s",
			.offset = offsetof(Render_Packet_3D, world_space_matrix_array)
		},
		{
			.name = "world_space_matrix_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Render_Packet_3D, world_space_matrix_count)
		},
	};

	 Reflection_Runtime_Struct Render_Packet_3D_Runtime_Struct =
	{
		.name = "Render_Packet_3D",
		.fields = Render_Packet_3D_Fields,
		.field_count = 8,
		.struct_size = sizeof(Render_Packet_3D)
	};

	reflection_registry_add_struct(reflection_registry, Render_Packet_3D_Runtime_Struct);

	Reflection_Runtime_Struct_Field Render_Packet_UI_Fields[] =
	{
	};

	 Reflection_Runtime_Struct Render_Packet_UI_Runtime_Struct =
	{
		.name = "Render_Packet_UI",
		.fields = Render_Packet_UI_Fields,
		.field_count = 0,
		.struct_size = sizeof(Render_Packet_UI)
	};

	reflection_registry_add_struct(reflection_registry, Render_Packet_UI_Runtime_Struct);

	Reflection_Runtime_Struct_Field Render_Packet_Sprite_Fields[] =
	{
		{
			.name = "system_name",
			.type = REFLECTION_TYPE_CHAR,
			.type_name = "char",
			.offset = offsetof(Render_Packet_Sprite, system_name)
		},
		{
			.name = "sprite_indices",
			.type = REFLECTION_TYPE_U16,
			.type_name = "u16",
			.offset = offsetof(Render_Packet_Sprite, sprite_indices)
		},
	};

	 Reflection_Runtime_Struct Render_Packet_Sprite_Runtime_Struct =
	{
		.name = "Render_Packet_Sprite",
		.fields = Render_Packet_Sprite_Fields,
		.field_count = 2,
		.struct_size = sizeof(Render_Packet_Sprite)
	};

	reflection_registry_add_struct(reflection_registry, Render_Packet_Sprite_Runtime_Struct);

	Reflection_Runtime_Struct_Field Render_Packet_Particle_Fields[] =
	{
		{
			.name = "particles",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle",
			.offset = offsetof(Render_Packet_Particle, particles)
		},
		{
			.name = "particle_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Render_Packet_Particle, particle_count)
		},
	};

	 Reflection_Runtime_Struct Render_Packet_Particle_Runtime_Struct =
	{
		.name = "Render_Packet_Particle",
		.fields = Render_Packet_Particle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Render_Packet_Particle)
	};

	reflection_registry_add_struct(reflection_registry, Render_Packet_Particle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Render_Packet_Fields[] =
	{
		{
			.name = "sprite_data_packet",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Render_Packet_Sprite",
			.offset = offsetof(Render_Packet, sprite_data_packet)
		},
		{
			.name = "ui_data_packet",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Render_Packet_UI",
			.offset = offsetof(Render_Packet, ui_data_packet)
		},
		{
			.name = "draw_3d_data_packet",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Render_Packet_3D",
			.offset = offsetof(Render_Packet, draw_3d_data_packet)
		},
		{
			.name = "particle_packet",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Render_Packet_Particle",
			.offset = offsetof(Render_Packet, particle_packet)
		},
	};

	 Reflection_Runtime_Struct Render_Packet_Runtime_Struct =
	{
		.name = "Render_Packet",
		.fields = Render_Packet_Fields,
		.field_count = 4,
		.struct_size = sizeof(Render_Packet)
	};

	reflection_registry_add_struct(reflection_registry, Render_Packet_Runtime_Struct);

	Reflection_Runtime_Struct_Field Asset_Registry_Fields[] =
	{
	};

	 Reflection_Runtime_Struct Asset_Registry_Runtime_Struct =
	{
		.name = "Asset_Registry",
		.fields = Asset_Registry_Fields,
		.field_count = 0,
		.struct_size = sizeof(Asset_Registry)
	};

	reflection_registry_add_struct(reflection_registry, Asset_Registry_Runtime_Struct);

	Reflection_Runtime_Struct_Field Asset_System_Fields[] =
	{
		{
			.name = "sprite_system",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Sprite_System",
			.offset = offsetof(Asset_System, sprite_system)
		},
		{
			.name = "mesh_system",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Mesh_System",
			.offset = offsetof(Asset_System, mesh_system)
		},
		{
			.name = "texture_system",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Texture_System",
			.offset = offsetof(Asset_System, texture_system)
		},
		{
			.name = "material_system",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_System",
			.offset = offsetof(Asset_System, material_system)
		},
		{
			.name = "scene",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Scene",
			.offset = offsetof(Asset_System, scene)
		},
		{
			.name = "animation_system",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Animation_System",
			.offset = offsetof(Asset_System, animation_system)
		},
		{
			.name = "particle_system",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_System",
			.offset = offsetof(Asset_System, particle_system)
		},
		{
			.name = "render_packet",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Render_Packet",
			.offset = offsetof(Asset_System, render_packet)
		},
		{
			.name = "asset_registry",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Asset_Registry",
			.offset = offsetof(Asset_System, asset_registry)
		},
	};

	 Reflection_Runtime_Struct Asset_System_Runtime_Struct =
	{
		.name = "Asset_System",
		.fields = Asset_System_Fields,
		.field_count = 9,
		.struct_size = sizeof(Asset_System)
	};

	reflection_registry_add_struct(reflection_registry, Asset_System_Runtime_Struct);

}
