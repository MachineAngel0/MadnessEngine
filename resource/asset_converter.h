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


bool asset_converter_material_asset(Asset_System* asset_system, Material_Info* material_info,
                                    MADNESS_UUID* out_uuid);


bool asset_converter_material_instance_from_material_asset(Asset_System* asset_system,
                                                           Material_Instance* mat_inst,
                                                           Material_Info* material_info,
                                                           const char* asset_name);

bool asset_converter_material_instance_from_material_info(Asset_System* asset_system,
                                                          Material_Info* material_info,
                                                          const char* asset_name,
                                                          void* material_data,
                                                          MADNESS_UUID mat_asset_uuid);

bool asset_converter_reload_textures(Asset_System* asset_system, Memory_System* memory_system);


//helper functions
String_Builder* asset_converter_create_file_path(Scratch_Allocator scratch_allocator, const char* file_path,
                                                 const char* engine_path, const char* engine_ext);
void asset_converter_create_directory_for_engine_asset(String_Builder* str_builder_output_path);


#endif
