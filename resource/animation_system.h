#ifndef ANIMATOIN_SYSTEM_H
#define ANIMATOIN_SYSTEM_H


#include "resource_import_types.h"
#include "resource_types.h"


Animation_System* animation_init(Memory_System* memory_system);
bool animation_deinit(Animation_System* animation_system, Memory_System* memory_system);


void animation_system_update(Animation_System* animation_system, float delta_time);


void animation_add_data(Asset_System* asset_system, Madness_Skinned_Mesh* madness_mesh, Animation_Handle* out_handle,
                        u32* out_skinned_idx);


Madness_Animation* animation_system_get_animation_data(Animation_System* animation_system,
                                                       Animation_Handle* animation_handle);


//TODO: look into how animations and frame time work together
// also look into animation blending, since that will be pretty important for the turn based game

void skinned_mesh_play_animation_id(Asset_System* asset_system, Animation_Handle handle, u32 animation_id, bool loop);
// void skinned_mesh_play_animation_id2(Mesh_System* mesh_system, skeletal_mesh_handle handle, u32 animation_id, float start_time. bool loop);


void skinned_mesh_play_animation_name(Asset_System* asset_system, Animation_Handle handle, const char* anim_name,
                                      bool loop);
void skinned_mesh_stop_current_animation(Asset_System* asset_system, Animation_Handle handle);

u32 skinned_mesh_get_playing_animation_id(Asset_System* asset_system, Animation_Handle handle);
float skinned_mesh_get_current_animation_time(Asset_System* asset_system, Animation_Handle handle);


#endif
