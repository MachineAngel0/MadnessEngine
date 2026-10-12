#ifndef SHADER_SYSTEM_H
#define SHADER_SYSTEM_H

#include "resource_types.h"


//Shader what is it?
// vertex/frag/compute file
// state needed for a pipeline
// contains a material definition

//Material what is it
// gpu data
// some way to know what shader it is using, a handle is fine


bool shader_system_init(Shader_System* shader_system, Asset_System* asset_system, Memory_System* memory_system);
bool shader_system_deinit(Shader_System* shader_system);

void shader_system_update(Shader_System* shader_system);

void shader_system_render_packet(Shader_System* shader_system, Render_Packet* render_packet);

//load
void shader_acquire(Shader_System* shader_system, Asset_Record* record, FILE* file, Shader_Handle* out_handle);
void shader_release(Shader_System* shader_system, Shader_Handle handle);
bool shader_is_loaded(Shader_System* shader_system, Asset_Record* asset_record, Shader_Handle* out_handle);

//serialize
bool shader_asset_serialize(Shader_Asset* shader_asset, FILE* fptr);
bool shader_asset_deserialize(Shader_Asset* shader_asset, FILE* fptr, Allocator* allocator);
String_Builder* shader_asset_construct_path(Shader_Asset* asset, Allocator* allocator);

//shader asset
/**
 * @NOTE creates the shader if not already loaded, and passes back a shader handle
 */
void shader_create_or_get(Asset_System* asset_system, Shader_PSO* shader_pso,
                   Shader_Handle* out_handle);

Shader_Asset* shader_asset_get(Shader_System* shader_system,
                               Shader_Handle shader_handle);

Shader_Pipeline_Hash shader_pipeline_hash_create(Shader_PSO* shader_pso);
Shader_Key shader_key_create(Shader_PSO* shader_info);


//mapping
void shader_create_material_definition_struct(Reflection_Registry* reflection_registry,
                                              Material_Definition* material_definition,
                                              const char* material_name, Allocator* allocator);
void shader_mapping_add(Shader_System* shader_system, const char* shader_name, const char* material_name);

bool shader_mapping_get_definition(Shader_System* shader_system, const char* shader_name,
                                   Material_Definition* out_defintion);

bool shader_mapping_get_definition_from_shader_asset(Shader_System* shader_system, Shader_Asset* shader_asset,
                                   Material_Definition* out_defintion);

bool shader_mapping_get_definition_from_shader_asset(Shader_System* shader_system, Shader_Asset* shader_asset,
    Material_Definition* out_defintion);


//TODO: basically scan all our material instances and remove any shaders we dont find
// void shader_purge_unused(Asset_System* asset_system,
//                          Shader_Info* material_info,
//                          Shader_Handle* out_handle);


#endif //SHADER_SYSTEM_H
