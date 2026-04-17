#ifndef MATERIAL_RENDER_H
#define MATERIAL_RENDER_H

#include "../vulkan_types.h"

MAPI Material_Renderer* material_renderer_init(Renderer* renderer, Resource_System* resource_system);

MAPI void material_renderer_upload_data(Renderer* renderer, Material_Renderer* material_renderer, Render_Packet_Material* render_packet_material, vulkan_command_buffer* command_buffer);



#endif //MATERIAL_RENDER_H
