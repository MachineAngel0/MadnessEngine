#ifndef VK_RENDERPASS_H
#define VK_RENDERPASS_H


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
    vec4 screen_pos, vec4 clear_color,
    f32 depth,
    u32 stencil);


void vulkan_renderpass_destroy(vulkan_context* context, vulkan_renderpass* renderpass);


void vulkan_renderpass_begin(renderer* renderer, vulkan_command_buffer* command_buffer, u32 current_frame);
void vulkan_renderpass_end(renderer* renderer, vulkan_command_buffer* command_buffer, u32 current_frame);


void vulkan_renderpass_UI_begin(renderer* renderer, vulkan_command_buffer* command_buffer, u32 current_frame);
void vulkan_renderpass_UI_end(renderer* renderer, vulkan_command_buffer* command_buffer, u32 current_frame);



#endif //VK_RENDERPASS_H
