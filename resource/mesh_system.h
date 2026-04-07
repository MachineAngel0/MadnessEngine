#ifndef MESH_H
#define MESH_H


Mesh_System* mesh_system_init(Memory_System* memory_system);
bool mesh_system_generate_render_packet(Mesh_System* mesh_system, Render_Packet_Mesh* out_mesh_packet);


submesh* submesh_init(Arena* arena);
void sub_mesh_free(submesh* m);

static_mesh* static_mesh_init(Arena* arena, u32 mesh_size);
void static_mesh_free(static_mesh* static_mesh);


void mesh_load_gltf(Mesh_System* mesh_system, const char* gltf_path, Arena* arena, Frame_Arena* frame_arena, Resource_System*
                    resource_system);

void mesh_load_anim_gltf(Mesh_System* mesh_system, const char* gltf_path, Arena* arena, Frame_Arena* frame_arena, Resource_System*
                    resource_system);

void mesh_load_fbx(Mesh_System* mesh_system, const char* fbx_path, Arena* arena, Frame_Arena* frame_arena);

void mesh_load_obj(const char* obj_path, Renderer* renderer);




#endif
