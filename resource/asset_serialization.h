#ifndef ASSET_SERIALIZATION_H
#define ASSET_SERIALIZATION_H

#include "resource_types.h"

//
MAPI bool asset_texture_serialize(Madness_Texture_Runtime* runtime, FILE* fptr);
MAPI bool asset_texture_deserialize(Madness_Texture_Runtime* runtime, FILE* fptr, Heap_Allocator* texture_memory_allocator);


MAPI bool asset_font_serialize(Madness_Font_Runtime* runtime, FILE* fptr);
MAPI bool asset_font_deserialize(Madness_Font_Runtime* runtime, FILE* fptr, Heap_Allocator* texture_memory_allocator);


//
//material defines the layout and shader definition of the object, instance is the specific data/params of that material
MAPI bool asset_material_serialize(Material_Asset_Runtime* runtime, FILE* fptr);
MAPI bool asset_material_deserialize(Material_Asset_Runtime* runtime, FILE* fptr,
                                          Heap_Allocator* allocator);

MAPI bool asset_material_instance_serialize(Material_Instance* instance, FILE* fptr);
MAPI bool asset_material_instance_deserialize(Material_Instance* instance, FILE* fptr, Heap_Allocator* allocator);


//
MAPI bool asset_mesh_serialize(Madness_Mesh_Runtime* runtime, FILE* fptr);
MAPI bool asset_mesh_deserialize(Madness_Mesh_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator);

//
MAPI bool asset_skmesh_serialize(Madness_SkMesh_Runtime* runtime, FILE* fptr);
MAPI bool asset_skmesh_deserialize(Madness_SkMesh_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator);


#endif
