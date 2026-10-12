#include <stddef.h>
#include "runtime_registry.h"
#include "../resource/resource_types.h"

const char* resources_struct_string_list[] = {
	"Texture_Handle", 
	"Material_Handle", 
	"Material_Record_Handle", 
	"Shader_Handle", 
	"Mesh_Handle", 
	"Skinned_Mesh_Handle", 
	"Madness_SkMesh_Handle_Internal", 
	"Transform_Handle", 
	"Sprite_Handle", 
	"Animation_Handle", 
	"Particle_Emitter_Handle", 
	"Particle_Effect_Handle", 
	"Asset_Record", 
	"Asset_Runtime", 
	"Texture_Sampler", 
	"Madness_Texture", 
	"Texture_GPU_Upload", 
	"Madness_Texture_Runtime", 
	"Glyph", 
	"Madness_Font", 
	"Madness_Font_Runtime", 
	"PC_General", 
	"PC_Particle", 
	"PC_Shadow_Mapping", 
	"Shader_PSO", 
	"Shader_Asset", 
	"Material_GPU_Definition", 
	"Material_Definition", 
	"Material_Asset", 
	"Madness_Material", 
	"Particle", 
	"Particle_Mesh", 
	"Particle_Animation_vec3", 
	"Particle_Animation_vec2", 
	"Particle_Animation_float", 
	"Particle_Emitter_Data", 
	"Particle_Emitter", 
	"Particle_Effect", 
	"Madness_Animation", 
	"Madness_Skinned_Submesh_Instance", 
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
	"Shader_Material_Mapping", 
	"Shader_System", 
	"Material_Record", 
	"Sprite_System", 
	"Texture_System", 
	"Scene", 
	"Mesh_System", 
	"Animation_System", 
	"Particle_System", 
	"Material_System2", 
	"Render_Packet_3D", 
	"Render_Packet_Material", 
	"Render_Packet_UI", 
	"Render_Packet_Sprite", 
	"Render_Packet_Particle", 
	"Render_Packet", 
	"Asset_Registry", 
	"Asset_System", 
};

void generate_runtime_structs_resources(Reflection_Registry* reflection_registry)
{
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

	Reflection_Runtime_Struct_Field Material_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Handle, handle)
		},
		{
			.name = "generation",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Handle, generation)
		},
	};

	 Reflection_Runtime_Struct Material_Handle_Runtime_Struct =
	{
		.name = "Material_Handle",
		.fields = Material_Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Material_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Record_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Record_Handle, handle)
		},
		{
			.name = "generation",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Record_Handle, generation)
		},
	};

	 Reflection_Runtime_Struct Material_Record_Handle_Runtime_Struct =
	{
		.name = "Material_Record_Handle",
		.fields = Material_Record_Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_Record_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Material_Record_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Shader_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Shader_Handle, handle)
		},
	};

	 Reflection_Runtime_Struct Shader_Handle_Runtime_Struct =
	{
		.name = "Shader_Handle",
		.fields = Shader_Handle_Fields,
		.field_count = 1,
		.struct_size = sizeof(Shader_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Shader_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Mesh_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_Handle, handle)
		},
		{
			.name = "generation",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_Handle, generation)
		},
	};

	 Reflection_Runtime_Struct Mesh_Handle_Runtime_Struct =
	{
		.name = "Mesh_Handle",
		.fields = Mesh_Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Mesh_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Mesh_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Skinned_Mesh_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Skinned_Mesh_Handle, handle)
		},
	};

	 Reflection_Runtime_Struct Skinned_Mesh_Handle_Runtime_Struct =
	{
		.name = "Skinned_Mesh_Handle",
		.fields = Skinned_Mesh_Handle_Fields,
		.field_count = 1,
		.struct_size = sizeof(Skinned_Mesh_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Skinned_Mesh_Handle_Runtime_Struct);

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

	Reflection_Runtime_Struct_Field Particle_Emitter_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_Emitter_Handle, handle)
		},
		{
			.name = "gen",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_Emitter_Handle, gen)
		},
	};

	 Reflection_Runtime_Struct Particle_Emitter_Handle_Runtime_Struct =
	{
		.name = "Particle_Emitter_Handle",
		.fields = Particle_Emitter_Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Particle_Emitter_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Emitter_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Effect_Handle_Fields[] =
	{
		{
			.name = "handle",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_Effect_Handle, handle)
		},
		{
			.name = "gen",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_Effect_Handle, gen)
		},
	};

	 Reflection_Runtime_Struct Particle_Effect_Handle_Runtime_Struct =
	{
		.name = "Particle_Effect_Handle",
		.fields = Particle_Effect_Handle_Fields,
		.field_count = 2,
		.struct_size = sizeof(Particle_Effect_Handle)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Effect_Handle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Asset_Record_Fields[] =
	{
		{
			.name = "uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Asset_Record, uuid)
		},
		{
			.name = "type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Asset_Type",
			.offset = offsetof(Asset_Record, type)
		},
		{
			.name = "source_file",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Asset_Record, source_file)
		},
		{
			.name = "engine_path",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Asset_Record, engine_path)
		},
	};

	 Reflection_Runtime_Struct Asset_Record_Runtime_Struct =
	{
		.name = "Asset_Record",
		.fields = Asset_Record_Fields,
		.field_count = 4,
		.struct_size = sizeof(Asset_Record)
	};

	reflection_registry_add_struct(reflection_registry, Asset_Record_Runtime_Struct);

	Reflection_Runtime_Struct_Field Asset_Runtime_Fields[] =
	{
		{
			.name = "uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Asset_Runtime, uuid)
		},
		{
			.name = "generation",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Asset_Runtime, generation)
		},
		{
			.name = "reference_count",
			.type = REFLECTION_TYPE_S32,
			.type_name = "s32",
			.offset = offsetof(Asset_Runtime, reference_count)
		},
		{
			.name = "type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Asset_Type",
			.offset = offsetof(Asset_Runtime, type)
		},
		{
			.name = "engine_path",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Asset_Runtime, engine_path)
		},
	};

	 Reflection_Runtime_Struct Asset_Runtime_Runtime_Struct =
	{
		.name = "Asset_Runtime",
		.fields = Asset_Runtime_Fields,
		.field_count = 5,
		.struct_size = sizeof(Asset_Runtime)
	};

	reflection_registry_add_struct(reflection_registry, Asset_Runtime_Runtime_Struct);

	Reflection_Runtime_Struct_Field Texture_Sampler_Fields[] =
	{
		{
			.name = "mag",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Texture_Filter",
			.offset = offsetof(Texture_Sampler, mag)
		},
		{
			.name = "min",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Texture_Filter",
			.offset = offsetof(Texture_Sampler, min)
		},
		{
			.name = "u_wrap",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Texture_Wrap",
			.offset = offsetof(Texture_Sampler, u_wrap)
		},
		{
			.name = "v_wrap",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Texture_Wrap",
			.offset = offsetof(Texture_Sampler, v_wrap)
		},
	};

	 Reflection_Runtime_Struct Texture_Sampler_Runtime_Struct =
	{
		.name = "Texture_Sampler",
		.fields = Texture_Sampler_Fields,
		.field_count = 4,
		.struct_size = sizeof(Texture_Sampler)
	};

	reflection_registry_add_struct(reflection_registry, Texture_Sampler_Runtime_Struct);

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
			.type_name = "Texture_Type",
			.offset = offsetof(Madness_Texture, type)
		},
		{
			.name = "font_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Texture, font_index)
		},
	};

	 Reflection_Runtime_Struct Madness_Texture_Runtime_Struct =
	{
		.name = "Madness_Texture",
		.fields = Madness_Texture_Fields,
		.field_count = 7,
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
		{
			.name = "texture_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Texture_GPU_Upload, texture_index)
		},
	};

	 Reflection_Runtime_Struct Texture_GPU_Upload_Runtime_Struct =
	{
		.name = "Texture_GPU_Upload",
		.fields = Texture_GPU_Upload_Fields,
		.field_count = 3,
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

	Reflection_Runtime_Struct_Field Shader_PSO_Fields[] =
	{
		{
			.name = "shader_name",
			.type = REFLECTION_TYPE_PATH_STRING,
			.type_name = "Path_String",
			.offset = offsetof(Shader_PSO, shader_name)
		},
		{
			.name = "blend_mode",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Shader_Blend_Mode",
			.offset = offsetof(Shader_PSO, blend_mode)
		},
		{
			.name = "two_sided",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Shader_PSO, two_sided)
		},
	};

	 Reflection_Runtime_Struct Shader_PSO_Runtime_Struct =
	{
		.name = "Shader_PSO",
		.fields = Shader_PSO_Fields,
		.field_count = 3,
		.struct_size = sizeof(Shader_PSO)
	};

	reflection_registry_add_struct(reflection_registry, Shader_PSO_Runtime_Struct);

	Reflection_Runtime_Struct_Field Shader_Asset_Fields[] =
	{
		{
			.name = "version",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Shader_Asset, version)
		},
		{
			.name = "uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Shader_Asset, uuid)
		},
		{
			.name = "shader_pipeline_state",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Shader_PSO",
			.offset = offsetof(Shader_Asset, shader_pipeline_state)
		},
	};

	 Reflection_Runtime_Struct Shader_Asset_Runtime_Struct =
	{
		.name = "Shader_Asset",
		.fields = Shader_Asset_Fields,
		.field_count = 3,
		.struct_size = sizeof(Shader_Asset)
	};

	reflection_registry_add_struct(reflection_registry, Shader_Asset_Runtime_Struct);

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

	Reflection_Runtime_Struct_Field Material_Definition_Fields[] =
	{
		{
			.name = "runtime_reflection_hash",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Definition, runtime_reflection_hash)
		},
		{
			.name = "material_gpu_definition",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_GPU_Definition",
			.offset = offsetof(Material_Definition, material_gpu_definition)
		},
	};

	 Reflection_Runtime_Struct Material_Definition_Runtime_Struct =
	{
		.name = "Material_Definition",
		.fields = Material_Definition_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_Definition)
	};

	reflection_registry_add_struct(reflection_registry, Material_Definition_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Asset_Fields[] =
	{
		{
			.name = "shader_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Asset, shader_uuid)
		},
		{
			.name = "material_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Material_Asset, material_uuid)
		},
		{
			.name = "reflection_hash",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_Asset, reflection_hash)
		},
		{
			.name = "material_name",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Material_Asset, material_name)
		},
		{
			.name = "name",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Material_Asset, name)
		},
		{
			.name = "data_size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Material_Asset, data_size)
		},
	};

	 Reflection_Runtime_Struct Material_Asset_Runtime_Struct =
	{
		.name = "Material_Asset",
		.fields = Material_Asset_Fields,
		.field_count = 6,
		.struct_size = sizeof(Material_Asset)
	};

	reflection_registry_add_struct(reflection_registry, Material_Asset_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Material_Fields[] =
	{
		{
			.name = "material_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Madness_Material, material_uuid)
		},
		{
			.name = "shader_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Shader_Handle",
			.offset = offsetof(Madness_Material, shader_handle)
		},
		{
			.name = "reflection_hash",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Material, reflection_hash)
		},
		{
			.name = "material_record_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Record_Handle",
			.offset = offsetof(Madness_Material, material_record_handle)
		},
		{
			.name = "material_name",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Madness_Material, material_name)
		},
		{
			.name = "name",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Madness_Material, name)
		},
	};

	 Reflection_Runtime_Struct Madness_Material_Runtime_Struct =
	{
		.name = "Madness_Material",
		.fields = Madness_Material_Fields,
		.field_count = 6,
		.struct_size = sizeof(Madness_Material)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Material_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Fields[] =
	{
		{
			.name = "pos_x",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, pos_x)
		},
		{
			.name = "pos_y",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, pos_y)
		},
		{
			.name = "pos_z",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, pos_z)
		},
		{
			.name = "rot_x",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, rot_x)
		},
		{
			.name = "rot_y",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, rot_y)
		},
		{
			.name = "scale_x",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, scale_x)
		},
		{
			.name = "scale_y",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, scale_y)
		},
		{
			.name = "color",
			.type = REFLECTION_TYPE_VEC4,
			.type_name = "vec4s",
			.offset = offsetof(Particle, color)
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
		{
			.name = "life_left",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, life_left)
		},
		{
			.name = "vel_x",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, vel_x)
		},
		{
			.name = "vel_y",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, vel_y)
		},
		{
			.name = "vel_z",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, vel_z)
		},
		{
			.name = "gravity_x",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, gravity_x)
		},
		{
			.name = "gravity_y",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, gravity_y)
		},
		{
			.name = "gravity_z",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle, gravity_z)
		},
		{
			.name = "material_key",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Particle, material_key)
		},
		{
			.name = "material_id",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle, material_id)
		},
	};

	 Reflection_Runtime_Struct Particle_Runtime_Struct =
	{
		.name = "Particle",
		.fields = Particle_Fields,
		.field_count = 20,
		.struct_size = sizeof(Particle)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Mesh_Fields[] =
	{
		{
			.name = "pos_x",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, pos_x)
		},
		{
			.name = "pos_y",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, pos_y)
		},
		{
			.name = "pos_z",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, pos_z)
		},
		{
			.name = "rot_x",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, rot_x)
		},
		{
			.name = "rot_y",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, rot_y)
		},
		{
			.name = "rot_z",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, rot_z)
		},
		{
			.name = "scale_x",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, scale_x)
		},
		{
			.name = "scale_y",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, scale_y)
		},
		{
			.name = "scale_z",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, scale_z)
		},
		{
			.name = "texture_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Texture_Handle",
			.offset = offsetof(Particle_Mesh, texture_handle)
		},
		{
			.name = "tex_offset",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Particle_Mesh, tex_offset)
		},
		{
			.name = "tex_size",
			.type = REFLECTION_TYPE_VEC2,
			.type_name = "vec2s",
			.offset = offsetof(Particle_Mesh, tex_size)
		},
		{
			.name = "lifetime_left",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Mesh, lifetime_left)
		},
		{
			.name = "mesh_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Mesh_Handle",
			.offset = offsetof(Particle_Mesh, mesh_handle)
		},
	};

	 Reflection_Runtime_Struct Particle_Mesh_Runtime_Struct =
	{
		.name = "Particle_Mesh",
		.fields = Particle_Mesh_Fields,
		.field_count = 14,
		.struct_size = sizeof(Particle_Mesh)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Mesh_Runtime_Struct);

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

	Reflection_Runtime_Struct_Field Particle_Emitter_Data_Fields[] =
	{
		{
			.name = "emission_rate",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Emitter_Data, emission_rate)
		},
		{
			.name = "spawn_trigger",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Emitter_Data, spawn_trigger)
		},
		{
			.name = "particle_lifetime",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Emitter_Data, particle_lifetime)
		},
		{
			.name = "particle_lifetime_variance",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Particle_Emitter_Data, particle_lifetime_variance)
		},
		{
			.name = "particle_color",
			.type = REFLECTION_TYPE_VEC4,
			.type_name = "vec4s",
			.offset = offsetof(Particle_Emitter_Data, particle_color)
		},
		{
			.name = "position_variance",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Emitter_Data, position_variance)
		},
		{
			.name = "scale",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Emitter_Data, scale)
		},
		{
			.name = "scale_variance",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Emitter_Data, scale_variance)
		},
		{
			.name = "rotation",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Emitter_Data, rotation)
		},
		{
			.name = "velocity",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Emitter_Data, velocity)
		},
		{
			.name = "velocity_variance",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Emitter_Data, velocity_variance)
		},
		{
			.name = "gravity",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Emitter_Data, gravity)
		},
	};

	 Reflection_Runtime_Struct Particle_Emitter_Data_Runtime_Struct =
	{
		.name = "Particle_Emitter_Data",
		.fields = Particle_Emitter_Data_Fields,
		.field_count = 12,
		.struct_size = sizeof(Particle_Emitter_Data)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Emitter_Data_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Emitter_Fields[] =
	{
		{
			.name = "data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_Emitter_Data",
			.offset = offsetof(Particle_Emitter, data)
		},
		{
			.name = "name",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Particle_Emitter, name)
		},
		{
			.name = "material_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Particle_Emitter, material_uuid)
		},
		{
			.name = "material_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Handle",
			.offset = offsetof(Particle_Emitter, material_handle)
		},
		{
			.name = "position",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Emitter, position)
		},
		{
			.name = "in_use",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Particle_Emitter, in_use)
		},
	};

	 Reflection_Runtime_Struct Particle_Emitter_Runtime_Struct =
	{
		.name = "Particle_Emitter",
		.fields = Particle_Emitter_Fields,
		.field_count = 6,
		.struct_size = sizeof(Particle_Emitter)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Emitter_Runtime_Struct);

	Reflection_Runtime_Struct_Field Particle_Effect_Fields[] =
	{
		{
			.name = "emmiter_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Particle_Effect, emmiter_uuid)
		},
		{
			.name = "emitter_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_Effect, emitter_count)
		},
		{
			.name = "emitters_start",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_Effect, emitters_start)
		},
		{
			.name = "emitters_end",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_Effect, emitters_end)
		},
		{
			.name = "effect_current_time",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_Effect, effect_current_time)
		},
		{
			.name = "effect_length",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_Effect, effect_length)
		},
		{
			.name = "emitter_position",
			.type = REFLECTION_TYPE_VEC3,
			.type_name = "vec3s",
			.offset = offsetof(Particle_Effect, emitter_position)
		},
		{
			.name = "name",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Particle_Effect, name)
		},
		{
			.name = "emitters",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_Emitter",
			.offset = offsetof(Particle_Effect, emitters)
		},
		{
			.name = "infinite",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Particle_Effect, infinite)
		},
		{
			.name = "is_visible",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Particle_Effect, is_visible)
		},
	};

	 Reflection_Runtime_Struct Particle_Effect_Runtime_Struct =
	{
		.name = "Particle_Effect",
		.fields = Particle_Effect_Fields,
		.field_count = 11,
		.struct_size = sizeof(Particle_Effect)
	};

	reflection_registry_add_struct(reflection_registry, Particle_Effect_Runtime_Struct);

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

	Reflection_Runtime_Struct_Field Madness_Skinned_Submesh_Instance_Fields[] =
	{
		{
			.name = "skinned_id",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Skinned_Submesh_Instance, skinned_id)
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
		.field_count = 3,
		.struct_size = sizeof(Madness_Skinned_Submesh_Instance)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Skinned_Submesh_Instance_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Skinned_Mesh_Instance_Fields[] =
	{
		{
			.name = "mesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Skinned_Mesh_Instance, mesh_count)
		},
		{
			.name = "start_submesh_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Skinned_Mesh_Instance, start_submesh_index)
		},
		{
			.name = "mesh_asset_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Skinned_Mesh_Instance, mesh_asset_index)
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
			.name = "mesh_asset_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh_Instance, mesh_asset_index)
		},
		{
			.name = "parent_instance_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_SubMesh_Instance, parent_instance_index)
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
		.field_count = 4,
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
			.name = "start_submesh_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh_Instance, start_submesh_index)
		},
		{
			.name = "mesh_asset_index",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Mesh_Instance, mesh_asset_index)
		},
		{
			.name = "transform_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Transform_Handle",
			.offset = offsetof(Madness_Mesh_Instance, transform_handle)
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
	};

	 Reflection_Runtime_Struct Madness_SubMesh_Runtime_Struct =
	{
		.name = "Madness_SubMesh",
		.fields = Madness_SubMesh_Fields,
		.field_count = 9,
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
			.name = "material_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Madness_Mesh, material_uuid)
		},
		{
			.name = "material_handles",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Handle",
			.offset = offsetof(Madness_Mesh, material_handles)
		},
	};

	 Reflection_Runtime_Struct Madness_Mesh_Runtime_Struct =
	{
		.name = "Madness_Mesh",
		.fields = Madness_Mesh_Fields,
		.field_count = 4,
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
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Madness_Skinned_Mesh, material_instance)
		},
		{
			.name = "material_handles",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Handle",
			.offset = offsetof(Madness_Skinned_Mesh, material_handles)
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
		.field_count = 5,
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
			.name = "submesh_id",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_GPU_Upload, submesh_id)
		},
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
		.field_count = 3,
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
			.name = "material_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Madness_Mesh_Runtime, material_uuid)
		},
		{
			.name = "mesh_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Madness_Mesh_Runtime, mesh_uuid)
		},
	};

	 Reflection_Runtime_Struct Madness_Mesh_Runtime_Runtime_Struct =
	{
		.name = "Madness_Mesh_Runtime",
		.fields = Madness_Mesh_Runtime_Fields,
		.field_count = 6,
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
			.name = "material_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Madness_SkMesh_Runtime, material_uuid)
		},
		{
			.name = "skinned_mesh_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Madness_SkMesh_Runtime, skinned_mesh_uuid)
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
		.field_count = 8,
		.struct_size = sizeof(Madness_SkMesh_Runtime)
	};

	reflection_registry_add_struct(reflection_registry, Madness_SkMesh_Runtime_Runtime_Struct);

	Reflection_Runtime_Struct_Field Shader_Material_Mapping_Fields[] =
	{
		{
			.name = "shader_name",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Shader_Material_Mapping, shader_name)
		},
		{
			.name = "material_name",
			.type = REFLECTION_TYPE_STRING,
			.type_name = "String",
			.offset = offsetof(Shader_Material_Mapping, material_name)
		},
		{
			.name = "material_definition",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Definition",
			.offset = offsetof(Shader_Material_Mapping, material_definition)
		},
		{
			.name = "count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Shader_Material_Mapping, count)
		},
	};

	 Reflection_Runtime_Struct Shader_Material_Mapping_Runtime_Struct =
	{
		.name = "Shader_Material_Mapping",
		.fields = Shader_Material_Mapping_Fields,
		.field_count = 4,
		.struct_size = sizeof(Shader_Material_Mapping)
	};

	reflection_registry_add_struct(reflection_registry, Shader_Material_Mapping_Runtime_Struct);

	Reflection_Runtime_Struct_Field Shader_System_Fields[] =
	{
		{
			.name = "shader_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Shader_Asset",
			.offset = offsetof(Shader_System, shader_asset)
		},
		{
			.name = "shader_asset_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Shader_System, shader_asset_count)
		},
		{
			.name = "runtime_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Asset_Runtime",
			.offset = offsetof(Shader_System, runtime_asset)
		},
		{
			.name = "runtime_asset_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Shader_System, runtime_asset_count)
		},
		{
			.name = "shader_to_material_mapping",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Shader_Material_Mapping",
			.offset = offsetof(Shader_System, shader_to_material_mapping)
		},
	};

	 Reflection_Runtime_Struct Shader_System_Runtime_Struct =
	{
		.name = "Shader_System",
		.fields = Shader_System_Fields,
		.field_count = 5,
		.struct_size = sizeof(Shader_System)
	};

	reflection_registry_add_struct(reflection_registry, Shader_System_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_Record_Fields[] =
	{
		{
			.name = "offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Material_Record, offset)
		},
		{
			.name = "size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Material_Record, size)
		},
	};

	 Reflection_Runtime_Struct Material_Record_Runtime_Struct =
	{
		.name = "Material_Record",
		.fields = Material_Record_Fields,
		.field_count = 2,
		.struct_size = sizeof(Material_Record)
	};

	reflection_registry_add_struct(reflection_registry, Material_Record_Runtime_Struct);

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
			.name = "default_texture_uuid",
			.type = REFLECTION_TYPE_UUID,
			.type_name = "MADNESS_UUID",
			.offset = offsetof(Texture_System, default_texture_uuid)
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
			.type_name = "Asset_Runtime",
			.offset = offsetof(Texture_System, texture_asset)
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
			.name = "submesh_instances",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SubMesh_Instance",
			.offset = offsetof(Mesh_System, submesh_instances)
		},
		{
			.name = "submesh_instance_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_System, submesh_instance_count)
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
			.name = "skinned_submesh",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Skinned_Submesh_Instance",
			.offset = offsetof(Mesh_System, skinned_submesh)
		},
		{
			.name = "skinned_submesh_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Mesh_System, skinned_submesh_count)
		},
	};

	 Reflection_Runtime_Struct Mesh_System_Runtime_Struct =
	{
		.name = "Mesh_System",
		.fields = Mesh_System_Fields,
		.field_count = 12,
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
			.name = "default_effect",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_Effect",
			.offset = offsetof(Particle_System, default_effect)
		},
		{
			.name = "default_effect_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_Effect_Handle",
			.offset = offsetof(Particle_System, default_effect_handle)
		},
		{
			.name = "default_emitter",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_Emitter",
			.offset = offsetof(Particle_System, default_emitter)
		},
		{
			.name = "default_emitter_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_Emitter_Handle",
			.offset = offsetof(Particle_System, default_emitter_handle)
		},
		{
			.name = "particle_effects",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_Effect",
			.offset = offsetof(Particle_System, particle_effects)
		},
		{
			.name = "particle_effects_count_max",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_System, particle_effects_count_max)
		},
		{
			.name = "emitters",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_Emitter",
			.offset = offsetof(Particle_System, emitters)
		},
		{
			.name = "emitter_count_max",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_System, emitter_count_max)
		},
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
		{
			.name = "dead_particles",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_System, dead_particles)
		},
		{
			.name = "dead_particles_available",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_System, dead_particles_available)
		},
		{
			.name = "dead_particles_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Particle_System, dead_particles_count)
		},
		{
			.name = "particle_emitter_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Asset_Runtime",
			.offset = offsetof(Particle_System, particle_emitter_asset)
		},
		{
			.name = "particle_effect_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Asset_Runtime",
			.offset = offsetof(Particle_System, particle_effect_asset)
		},
	};

	 Reflection_Runtime_Struct Particle_System_Runtime_Struct =
	{
		.name = "Particle_System",
		.fields = Particle_System_Fields,
		.field_count = 15,
		.struct_size = sizeof(Particle_System)
	};

	reflection_registry_add_struct(reflection_registry, Particle_System_Runtime_Struct);

	Reflection_Runtime_Struct_Field Material_System2_Fields[] =
	{
		{
			.name = "material_assets",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Asset",
			.offset = offsetof(Material_System2, material_assets)
		},
		{
			.name = "materials",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_Material",
			.offset = offsetof(Material_System2, materials)
		},
		{
			.name = "material_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_System2, material_count)
		},
		{
			.name = "material_record",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Record",
			.offset = offsetof(Material_System2, material_record)
		},
		{
			.name = "material_record_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Material_System2, material_record_count)
		},
		{
			.name = "material_buffer",
			.type = REFLECTION_TYPE_U8,
			.type_name = "uint8_t",
			.offset = offsetof(Material_System2, material_buffer)
		},
		{
			.name = "material_buffer_offset",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Material_System2, material_buffer_offset)
		},
		{
			.name = "material_buffer_max_byte_size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Material_System2, material_buffer_max_byte_size)
		},
		{
			.name = "default_handle",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_Handle",
			.offset = offsetof(Material_System2, default_handle)
		},
		{
			.name = "default_data_blob",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Material_System2, default_data_blob)
		},
		{
			.name = "runtime_asset",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Asset_Runtime",
			.offset = offsetof(Material_System2, runtime_asset)
		},
		{
			.name = "runtime_asset_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Material_System2, runtime_asset_count)
		},
	};

	 Reflection_Runtime_Struct Material_System2_Runtime_Struct =
	{
		.name = "Material_System2",
		.fields = Material_System2_Fields,
		.field_count = 12,
		.struct_size = sizeof(Material_System2)
	};

	reflection_registry_add_struct(reflection_registry, Material_System2_Runtime_Struct);

	Reflection_Runtime_Struct_Field Render_Packet_3D_Fields[] =
	{
		{
			.name = "scene",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Scene",
			.offset = offsetof(Render_Packet_3D, scene)
		},
		{
			.name = "shader_assets",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Shader_Asset",
			.offset = offsetof(Render_Packet_3D, shader_assets)
		},
		{
			.name = "shader_asset_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Render_Packet_3D, shader_asset_count)
		},
		{
			.name = "material_buffer",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Render_Packet_3D, material_buffer)
		},
		{
			.name = "material_buffer_byte_size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Render_Packet_3D, material_buffer_byte_size)
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
			.name = "submesh_instances",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Madness_SubMesh_Instance",
			.offset = offsetof(Render_Packet_3D, submesh_instances)
		},
		{
			.name = "submesh_instances_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Render_Packet_3D, submesh_instances_count)
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
		.field_count = 13,
		.struct_size = sizeof(Render_Packet_3D)
	};

	reflection_registry_add_struct(reflection_registry, Render_Packet_3D_Runtime_Struct);

	Reflection_Runtime_Struct_Field Render_Packet_Material_Fields[] =
	{
		{
			.name = "material_buffer",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Render_Packet_Material, material_buffer)
		},
		{
			.name = "material_buffer_byte_size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u64",
			.offset = offsetof(Render_Packet_Material, material_buffer_byte_size)
		},
	};

	 Reflection_Runtime_Struct Render_Packet_Material_Runtime_Struct =
	{
		.name = "Render_Packet_Material",
		.fields = Render_Packet_Material_Fields,
		.field_count = 2,
		.struct_size = sizeof(Render_Packet_Material)
	};

	reflection_registry_add_struct(reflection_registry, Render_Packet_Material_Runtime_Struct);

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
		{
			.name = "emitters",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Particle_Emitter",
			.offset = offsetof(Render_Packet_Particle, emitters)
		},
		{
			.name = "emitter_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Render_Packet_Particle, emitter_count)
		},
	};

	 Reflection_Runtime_Struct Render_Packet_Particle_Runtime_Struct =
	{
		.name = "Render_Packet_Particle",
		.fields = Render_Packet_Particle_Fields,
		.field_count = 4,
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
		{
			.name = "material_packet",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Render_Packet_Material",
			.offset = offsetof(Render_Packet, material_packet)
		},
	};

	 Reflection_Runtime_Struct Render_Packet_Runtime_Struct =
	{
		.name = "Render_Packet",
		.fields = Render_Packet_Fields,
		.field_count = 5,
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
			.name = "material_system",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Material_System2",
			.offset = offsetof(Asset_System, material_system)
		},
		{
			.name = "shader_system",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Shader_System",
			.offset = offsetof(Asset_System, shader_system)
		},
		{
			.name = "texture_system",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Texture_System",
			.offset = offsetof(Asset_System, texture_system)
		},
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
		.field_count = 10,
		.struct_size = sizeof(Asset_System)
	};

	reflection_registry_add_struct(reflection_registry, Asset_System_Runtime_Struct);

}
