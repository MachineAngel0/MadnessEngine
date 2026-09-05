#ifndef ASSET_CONVERTER_H
#define ASSET_CONVERTER_H


#include "resource_types.h"

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


MAPI bool asset_converter_material_asset(Asset_System* asset_system,
                                         Material_Asset* material_asset);


MAPI bool asset_converter_material_instance(Asset_System* asset_system,
                                            Material_Instance* mat_inst);
/***
 * @note: only the material info and name are manditory, the rest will get auto created,
 * @note: it will fill out and give you a default material instance
 */
MAPI bool asset_converter_material(Asset_System* asset_system,
                                   Material_Info* material_info,
                                   Material_Asset* out_material_asset,
                                   Material_Instance* out_material_instance,
                                   const char* mat_inst_name);




//helper functions
String_Builder* asset_converter_create_file_path(Scratch_Allocator scratch_allocator, const char* file_path,
                                                 const char* engine_path, const char* engine_ext);
void asset_converter_create_directory_for_engine_asset(String_Builder* str_builder_output_path);


bool asset_converter_material_from_material_info(Asset_System* asset_system, Material_Info* material_info,
                                                 MADNESS_UUID* out_uuid);







void asset_converter_particle_emitter(Asset_System* asset_system,
                                      Particle_Emitter* particle_emitter,
                                      MADNESS_UUID* out_uuid);


void asset_converter_particle_effect(Asset_System* asset_system,
                                     Particle_Effect* particle_effect,
                                     MADNESS_UUID* out_uuid);


#endif
