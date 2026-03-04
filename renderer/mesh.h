#ifndef MESH_H
#define MESH_H


submesh* submesh_init(Arena* arena);
void sub_mesh_free(submesh* m);

static_mesh* static_mesh_init(Arena* arena, u32 mesh_size);
void static_mesh_free(static_mesh* static_mesh);


void mesh_load_gltf(Renderer* renderer, const char* gltf_path);

void mesh_load_fbx(Renderer* renderer, const char* fbx_path);

void mesh_load_obj(const char* obj_path, Renderer* renderer);



Mesh_System* mesh_system_init(Renderer* renderer);

void mesh_system_generate_draw_data(Renderer* renderer, Mesh_System* mesh_system);

void mesh_system_draw(Renderer* renderer, Mesh_System* mesh_system, vulkan_command_buffer* command_buffer,
                      vulkan_shader_pipeline* pipeline);

void static_mesh_to_madness_mesh(static_mesh* s_mesh, const char* file_name, Frame_Arena* frame_arena);

#endif
