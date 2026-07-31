#ifndef ASSET_CONVERTER_H
#define ASSET_CONVERTER_H

#include "resource_types.h"


//takes in any file and checks its extension type, and calls the appropriate function
MAPI bool asset_convert_file(Asset_System* asset_system, const char* file_path, String* out_engine_path);


MAPI bool asset_converter_texture(Asset_System* asset_system, const char* file_path, String* out_engine_path);

MAPI bool asset_converter_font(Asset_System* asset_system, const char* file_path);

MAPI bool asset_converter_msdf_font(Asset_System* asset_system, const char* file_path);


// gltf format if we want, take what we need -> create a default version of that asset on disk,
// load it up and assign any texture or material data from the file, material data might have to be dynamic/reflection
//

//the source asset

MAPI bool asset_converter_gltf_mesh(Asset_System* asset_system, const char* gltf_path);


bool asset_converter_material_asset(Asset_System* asset_system, Material_Info* material_info,
                                    Reflection_Registry* reflection_registry_material, String* out_engine_path);

bool asset_converter_material_instance(Asset_System* asset_system, const char* material_name, const char* asset_name,
                                       const char* material_asset_path, void* material_data, u32 material_size);
#endif
