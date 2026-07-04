#ifndef MESH_H
#define MESH_H


Mesh_System* mesh_system_init(Memory_System* memory_system);
bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system);

bool mesh_system_generate_render_packet(Mesh_System* mesh_system, Render_Packet_Mesh* out_mesh_packet);


//when loading in a
void mesh_load_gltf(Resource_System* resource_system, const char* gltf_path);

void mesh_load_anim_gltf(Resource_System* resource_system, const char* gltf_path);


void _gltf_load_skin_data(Resource_System* resource_system);
void _gltf_load_material(Resource_System* resource_system);
void _gltf_load_mesh_data(Resource_System* resource_system);



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



Skinned_Mesh_Instance* skinned_mesh_instance_init(Mesh_System* mesh_system, Animation_Handle animation_handle,
                                                  Heap_Allocator* allocator);




void skinned_mesh_play_animation_id(Mesh_System* mesh_system, skeletal_mesh_handle handle, u32 animation_id, bool loop);
void skinned_mesh_play_animation_name(Mesh_System* mesh_system, skeletal_mesh_handle handle, const char* anim_name, bool loop);
void skinned_mesh_stop_current_animation(Mesh_System* mesh_system, skeletal_mesh_handle handle);

void animation_update_single_test(Mesh_System* mesh_system, float delta_time, Frame_Allocator* frame_allocator);
void animation_update(Mesh_System* mesh_system);



#endif
