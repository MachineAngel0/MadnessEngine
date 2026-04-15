#ifndef RENDER_RESOURCE_BACKEND_H
#define RENDER_RESOURCE_BACKEND_H
#include "vulkan_types.h"

typedef struct Renderer_Resource_Backend
{
    Sprite_Renderer sprite_backend;
    UI_Renderer_Backend ui_backend;

    //queue based data
    //ring_queue texture_upload; //reference, does not own
} Renderer_Resource_Backend;

bool renderer_resource_backend_init(Renderer* renderer, Resource_System* resource_system);
bool renderer_resource_backend_shutdown(Renderer* renderer, Resource_System* resource_system);

//Sprite Renderer
Sprite_Renderer* sprite_render_init(Renderer* renderer, Resource_System* resource_system);
MAPI void sprite_upload_draw_data(Renderer* renderer,
                                  Sprite_Renderer* sprite_backend, Render_Packet_Sprite* sprite_render_packet);
MAPI void sprite_renderer_draw(Renderer* renderer, Sprite_Renderer* sprite_backend,
                      vulkan_command_buffer* command_buffer);

//UI Renderer
MAPI UI_Renderer_Backend* ui_render_init(Renderer* renderer);

MAPI void ui_renderer_upload_draw_data(UI_Renderer_Backend* ui_renderer, Renderer* renderer,
                                       Render_Packet* render_packet);
MAPI void ui_renderer_draw(UI_Renderer_Backend* ui_renderer, Renderer* renderer, vulkan_command_buffer* command_buffer,
                           Render_Packet* render_packet);


//Mesh Renderer
MAPI Mesh_Renderer* mesh_renderer_init(Renderer* renderer, Resource_System* resource_system);

MAPI void mesh_renderer_upload_draw_data_new(Renderer* renderer, vulkan_command_buffer* command_buffer, Mesh_Renderer* mesh_renderer, Render_Packet* render_packet);
void mesh_renderer_construct_indirect_draw(Renderer* renderer, vulkan_command_buffer* cmd_buffer, Mesh_Renderer* mesh_renderer, Render_Packet* render_packet);

MAPI void mesh_renderer_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer, vulkan_command_buffer* command_buffer,
                      vulkan_shader_pipeline* pipeline);



#endif //RENDER_RESOURCE_BACKEND_H
