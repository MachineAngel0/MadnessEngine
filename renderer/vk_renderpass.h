#ifndef VK_RENDERP_H
#define VK_RENDERP_H

#include <stdbool.h>
#include "vulkan_types.h"

struct Command_Buffer_Context;
struct Swapchain_Context;
struct Graphics_Context;
struct vulkan_context;

enum Renderpass_Type
{
    RENDERPASS_WORLD = 1 << 0,
    RENDERPASS_UI = 1 << 1,
};


enum Renderpass_Clear_Flag
{
    RENDER_PASS_CLEAR_NONE_FLAG = 1 << 0,
    RENDER_PASS_CLEAR_COLOR_BUFFER_FLAG = 1 << 1,
    RENDER_PASS_CLEAR_DEPTH_BUFFER_FLAG = 1 << 2, //not in use
    RENDER_PASS_CLEAR_STENCIL_BUFFER_FLAG = 1 << 3, // not in use
};

void vulkan_renderpass_create(
    vulkan_context* context,
    vulkan_renderpass* out_renderpass,
    f32 x, f32 y, f32 w, f32 h,
    f32 r, f32 g, f32 b, f32 a,
    f32 depth,
    u32 stencil);


void vulkan_renderpass_destroy(vulkan_context* context, vulkan_renderpass* renderpass);


void vulkan_renderpass_begin(
    vulkan_command_buffer* command_buffer,
    vulkan_renderpass* renderpass,
    VkFramebuffer frame_buffer);


void vulkan_renderpass_end(vulkan_command_buffer* command_buffer, vulkan_renderpass* renderpass);


// void renderpass_create(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context,
//                        Graphics_Context& graphics_context, unsigned char clear_flags, bool has_prev_pass,
//                        bool has_next_pass);
//
//
// void renderpass_begin(vulkan_context& vulkan_context, Swapchain_Context& swapchain_context,
//                       Command_Buffer_Context* command_buffer,
//                       Graphics_Context& graphics_context, uint32_t image_index, unsigned char clear_flags);
//
// void renderpass_end(Command_Buffer_Context* command_buffer, uint32_t frame);


#endif //VK_RENDERP_H
