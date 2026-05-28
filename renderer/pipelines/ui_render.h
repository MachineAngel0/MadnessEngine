#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include "../vulkan_enum_types.h"
#include "../../resource/ui_insanity.h"

MAPI UI_Renderer_Backend* ui_render_init(Renderer* renderer, vulkan_command_buffer* command_buffer);

void ui_renderer_madness_upload_draw_data(UI_Renderer_Backend* ui_renderer, Renderer* renderer,
                                  Render_Packet* render_packet, vulkan_command_buffer* command_buffer);

void ui_renderer_madness_draw(UI_Renderer_Backend* ui_renderer, Renderer* renderer, vulkan_command_buffer* command_buffer);

#endif //UI_RENDERER_H
