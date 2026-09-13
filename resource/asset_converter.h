#ifndef ASSET_CONVERTER_H
#define ASSET_CONVERTER_H


#include "resource_types.h"

//NOTE: the converters are only responsible for serializing the asset to a file and handing you back a UUID,
// if you want the data, you have to load it in (w/ UUID), and then modify it however you wish


//takes in any file and checks its extension type, and calls the appropriate function
MAPI bool asset_convert_file_path(Asset_System* asset_system, const char* file_path, MADNESS_UUID* out_uuid);


MAPI bool asset_converter_texture(Asset_System* asset_system, const char* file_path, MADNESS_UUID* out_uuid);

MAPI bool asset_converter_font(Asset_System* asset_system, const char* file_path);

MAPI bool asset_converter_msdf_font(Asset_System* asset_system, const char* file_path);


// gltf format if we want, take what we need -> create a default version of that asset on disk,
// load it up and assign any texture or material data from the file, material data might have to be dynamic/reflection
//

//the source asset

MAPI bool asset_converter_mesh(Asset_System* asset_system, const char* gltf_path);
MAPI bool asset_converter_gltf_mesh(Asset_System* asset_system, const char* gltf_path);


MAPI bool asset_converter_shader_asset(Asset_System* asset_system, Shader_Asset* material_asset);

MAPI bool asset_converter_material(Asset_System* asset_system,
                                   Material* material);
MAPI bool asset_converter_material_and_generate_uuid(Asset_System* asset_system,
                                   Material* material, MADNESS_UUID* out_material_uuid);





MAPI bool asset_converter_material_from_shader_handle(Asset_System* asset_system,
                                                   Shader_Handle* shader_handle,
                                                   const char* mat_inst_name,
                                                   MADNESS_UUID* out_shader_uuid,
                                                   MADNESS_UUID* out_material_uuid);

MAPI bool asset_converter_material_from_data(Asset_System* asset_system,
                                                   Shader_Handle* shader_handle,
                                                   const char* mat_inst_name,
                                                   void* data,
                                                   MADNESS_UUID* out_shader_uuid,
                                                   MADNESS_UUID* out_material_uuid);




MAPI void asset_converter_particle_emitter(Asset_System* asset_system,
                                      Particle_Emitter* particle_emitter,
                                      MADNESS_UUID* out_uuid);


MAPI void asset_converter_particle_effect(Asset_System* asset_system,
                                     Particle_Effect* particle_effect,
                                     MADNESS_UUID* out_uuid);





//helper functions
String_Builder* asset_converter_create_file_path(Scratch_Allocator scratch_allocator, const char* file_path,
                                                 const char* engine_path, const char* engine_ext);
void asset_converter_create_directory_for_engine_asset(String_Builder* str_builder_output_path);












#endif
