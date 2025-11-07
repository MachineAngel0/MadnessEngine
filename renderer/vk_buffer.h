//
// Created by Adams Humbert on 9/7/2025.
//

#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include "vulkan/vulkan.h"
#include "vulkan_types.h"


typedef struct Buffer_Context
{
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;

    VkBuffer vertex_staging_buffer;
    VkDeviceMemory vertex_staging_buffer_memory;

    VkBuffer index_buffer;
    VkDeviceMemory index_buffer_memory;

    VkBuffer index_staging_buffer;
    VkDeviceMemory index_staging_buffer_memory;

    void* data_vertex;
    VkDeviceSize vertex_buffer_capacity;
    void* data_index;
    VkDeviceSize index_buffer_capacity;

}Buffer_Context;

uint32_t find_memory_type(vulkan_context* vulkan_context, uint32_t typeFilter, VkMemoryPropertyFlags properties);



//TODO: clean up/ refactor
/*
void buffer_create(vulkan_context* vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);


void buffer_destroy_free(vulkan_context* vulkan_context, Buffer_Context* buffer_context);;

void buffer_copy(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_index, VkBuffer srcBuffer,
                 VkBuffer dstBuffer, VkDeviceSize size);;
*/

#endif //VK_BUFFER_H
