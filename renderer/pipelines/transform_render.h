#ifndef TRANSFORM_RENDER_H
#define TRANSFORM_RENDER_H

#include "../vulkan_types.h"

MAPI Transform_Renderer* transform_renderer_init(Renderer* renderer, Resource_System* resource_system);

MAPI void transform_renderer_upload_data(Renderer* renderer, Transform_Renderer* transform_renderer, Render_Packet* render_packet, vulkan_command_buffer* command_buffer);



#endif //TRANSFORM_RENDER_H
