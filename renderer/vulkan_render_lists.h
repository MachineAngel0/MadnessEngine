#ifndef VULKAN_RENDER_LIST_H
#define VULKAN_RENDER_LIST_H


#include "vk_buffer.h"
#include "vulkan_struct_draw_types.h"
#include "vulkan_struct_types.h"


void vulkan_render_list_init(Renderer* renderer, Vulkan_Mesh_Render_List* render_list)
{

    //the draw count is temporary
    render_list->indirect_buffer = vulkan_buffer_create_frame(renderer, renderer->buffer_system, BUFFER_TYPE_INDIRECT,
                                                              sizeof(VkDrawIndexedIndirectCommand) * 1000, "Render List Indirect");

    render_list->mesh_draw_data = vulkan_buffer_create_frame(renderer, renderer->buffer_system, BUFFER_TYPE_STORAGE_GPU,
                                                             sizeof(Vulkan_Mesh_GPU_Renderable) * 1000, "Render List Mesh Draw");



    // vkCmdDrawIndirectCount() // the count is in a buffer
}




#endif
