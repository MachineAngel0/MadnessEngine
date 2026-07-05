#ifndef MESH_H
#define MESH_H

#include "cgltf.h"


Mesh_System* mesh_system_init(Memory_System* memory_system);
bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system);

bool mesh_system_generate_render_packet(Mesh_System* mesh_system, Render_Packet_Mesh* out_mesh_packet);


//when loading in a
void mesh_load_gltf(Resource_System* resource_system, const char* gltf_path);


void _gltf_load_mesh_data(Resource_System* resource_system,
                          const char* gltf_path,
                          cgltf_data* data,
                          u32* mesh_index_array,
                          Mesh_Upload_Data* mesh_upload_data_array);

void _gltf_load_anim_data(Resource_System* resource_system,
                          const char* gltf_path,
                          cgltf_data* data,
                          u32* skinned_mesh_indexs,
                          Skinned_Mesh_Meta_Data* skinned_mesh_meta_data,
                          Skinned_Mesh_Upload_Data* skinned_mesh_upload_data_array);


void mesh_load_fbx(Mesh_System* mesh_system, const char* fbx_path, Allocator* arena, Frame_Allocator* frame_arena);

void mesh_load_obj(const char* obj_path, Renderer* renderer);


//TODO: just one gltf load function, you will need to seperate the data into static and skinned meshes
// skinned meshes shares the same base data for static meshes, just a different array probably


//animation system

//load the mesh, which we have

typedef struct skeletal_mesh_handle
{
    u32 handle;
} skeletal_mesh_handle;


bool skinned_mesh_instance_fill_out(Mesh_System* mesh_system, Skinned_Mesh_Instance* skinned_mesh_inst,  Animation_Handle animation_handle,
                                    Heap_Allocator* allocator);
Skinned_Mesh_Instance* skinned_mesh_instance_init(Mesh_System* mesh_system, Animation_Handle animation_handle,
                                                  Heap_Allocator* allocator);

void skinned_mesh_play_animation_id(Mesh_System* mesh_system, skeletal_mesh_handle handle, u32 animation_id, bool loop);
void skinned_mesh_play_animation_name(Mesh_System* mesh_system, skeletal_mesh_handle handle, const char* anim_name,
                                      bool loop);
void skinned_mesh_stop_current_animation(Mesh_System* mesh_system, skeletal_mesh_handle handle);

void animation_update_single_test(Mesh_System* mesh_system, float delta_time, Frame_Allocator* frame_allocator);
void animation_update(Mesh_System* mesh_system, float delta_time, Frame_Allocator* frame_allocator);


#endif
