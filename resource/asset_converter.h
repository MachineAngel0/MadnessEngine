#ifndef ASSET_CONVERTER_H
#define ASSET_CONVERTER_H

#include "resource_types.h"
#include "resource_import_types.h"


//takes in any file and checks its extension type, and calls the appropriate function
MAPI bool asset_convert_file(Asset_System* asset_system, const char* file_path, MADNESS_UUID* out_uuid);


MAPI bool asset_converter_texture(Asset_System* asset_system, const char* file_path, MADNESS_UUID* out_uuid);

MAPI bool asset_converter_font(Asset_System* asset_system, const char* file_path);

MAPI bool asset_converter_msdf_font(Asset_System* asset_system, const char* file_path);


// gltf format if we want, take what we need -> create a default version of that asset on disk,
// load it up and assign any texture or material data from the file, material data might have to be dynamic/reflection
//

//the source asset

MAPI bool asset_converter_gltf_mesh(Asset_System* asset_system, const char* gltf_path);


MAPI bool asset_converter_material(Asset_System* asset_system, const char* material_name, const char* asset_name,
                                           GLTF_Material* gltf_material, Reflection_Registry* reflection_registry_material,
                                           Asset_Type asset_type, MADNESS_UUID* out_uuid);



#endif
