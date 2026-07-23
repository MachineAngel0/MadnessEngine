#ifndef MESH_H
#define MESH_H

#include "cgltf.h"


Mesh_System* mesh_system_init(Asset_System* resource_system, Memory_System* memory_system);

bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system);


void mesh_system_load_mesh(Asset_System* asset_system, Madness_Mesh_Runtime* mesh_asset);
void mesh_system_load_skinned_mesh(Asset_System* resource_system, Madness_SkMesh_Runtime* skmesh_asset);

bool mesh_system_exists_mesh(Asset_System* resource_system, Madness_Mesh_Handle* out_handle, u64 hash);
bool mesh_system_exists_skmesh(Asset_System* resource_system, Madness_SkMesh_Handle* out_handle, u64 hash);


//animation system
GLTF_Animation_Data* sk_mesh_parent_instance_get_animation_data(Mesh_System* mesh_system, Madness_SkMesh_Instance* sk_mesh_inst);

//load the mesh, which we have

typedef struct skeletal_mesh_handle
{
    u32 handle;
} skeletal_mesh_handle;

//TODO: look into how animations and frame time work together
// also look into animation blending, since that will be pretty important for the turn based game

void skinned_mesh_play_animation_id(Mesh_System* mesh_system, skeletal_mesh_handle handle, u32 animation_id, bool loop);
// void skinned_mesh_play_animation_id2(Mesh_System* mesh_system, skeletal_mesh_handle handle, u32 animation_id, float start_time. bool loop);



void skinned_mesh_play_animation_name(Mesh_System* mesh_system, skeletal_mesh_handle handle, const char* anim_name,
                                      bool loop);
void skinned_mesh_stop_current_animation(Mesh_System* mesh_system, skeletal_mesh_handle handle);

u32 skinned_mesh_get_playing_animation_id(Mesh_System* mesh_system, skeletal_mesh_handle handle);
float skinned_mesh_get_current_animation_time(Mesh_System* mesh_system, skeletal_mesh_handle handle);


void animation_update(Mesh_System* mesh_system, float delta_time, Frame_Allocator* frame_allocator);


#endif
