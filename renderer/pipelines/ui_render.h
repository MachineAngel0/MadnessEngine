#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include "../vulkan_types.h"
#include "../../resource/insanity_ui.h"

MAPI UI_Renderer_Backend* ui_render_init(Renderer* renderer);

MAPI void ui_renderer_upload_draw_data(UI_Renderer_Backend* ui_renderer, Renderer* renderer,
                                       Render_Packet* render_packet, vulkan_command_buffer* command_buffer);
MAPI void ui_renderer_draw(UI_Renderer_Backend* ui_renderer, Renderer* renderer, vulkan_command_buffer* command_buffer);

//NEW
MAPI void ui_renderer_upload_insanity_draw_data(UI_Renderer_Backend* ui_renderer, Renderer* renderer,
                                           vulkan_command_buffer* command_buffer);


#endif //UI_RENDERER_H
