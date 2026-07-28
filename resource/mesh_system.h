#ifndef MESH_H
#define MESH_H



Mesh_System* mesh_system_init(Asset_System* resource_system, Memory_System* memory_system);

bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system);


void mesh_system_load_mesh(Asset_System* asset_system, Madness_Mesh_Runtime* mesh_asset, MADNESS_UUID uuid, u64 hash);
void mesh_system_load_skinned_mesh(Asset_System* asset_system, Madness_SkMesh_Runtime* skmesh_asset, MADNESS_UUID uuid, u64 hash);

bool mesh_system_exists_mesh(Asset_System* asset_system, Madness_Mesh_Handle* out_handle, MADNESS_UUID uuid, u64 hash);
bool mesh_system_exists_skmesh(Asset_System* asset_system, Madness_SkMesh_Handle* out_handle, MADNESS_UUID uuid, u64 hash);


//animation system
GLTF_Animation_Data* sk_mesh_parent_instance_get_animation_data(Mesh_System* mesh_system, Madness_Skinned_Mesh_Instance* sk_mesh_inst);



#endif
