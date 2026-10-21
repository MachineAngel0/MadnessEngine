#ifndef VULKAN_RENDER_LIST_H
#define VULKAN_RENDER_LIST_H


#include "vk_buffer.h"
#include "vulkan_struct_draw_types.h"
#include "vulkan_struct_types.h"


void vulkan_render_list_init(Renderer* renderer, Vulkan_Mesh_Render_List* render_list, Vulkan_Skinned_Mesh_Render_List* skinned_render_list)
{

    //the draw count is temporary
    render_list->indirect_buffer = vulkan_buffer_create_frame(renderer, renderer->buffer_system, BUFFER_TYPE_INDIRECT,
                                                              sizeof(VkDrawIndexedIndirectCommand) * MAX_MESH_COUNT);

    render_list->mesh_draw_data = vulkan_buffer_create_frame(renderer, renderer->buffer_system, BUFFER_TYPE_STORAGE_GPU,
                                                             sizeof(Mesh_GPU_Renderable) * MAX_MESH_COUNT);


    skinned_render_list->skinned_indirect_buffer = vulkan_buffer_create_frame(renderer, renderer->buffer_system, BUFFER_TYPE_INDIRECT,
                                                              sizeof(VkDrawIndexedIndirectCommand) * MAX_SKINNED_MESH_COUNT);

    skinned_render_list->skinned_draw_data = vulkan_buffer_create_frame(renderer, renderer->buffer_system, BUFFER_TYPE_STORAGE_GPU,
                                                             sizeof(Mesh_GPU_Renderable) * MAX_SKINNED_MESH_COUNT);
    // vkCmdDrawIndirectCount() // the count is in a buffer
}




#endif //VULKAN_RENDER_LIST_H


