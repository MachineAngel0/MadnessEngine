//
// Created by Adams Humbert on 9/7/2025.
//

#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include "vulkan/vulkan.h"


struct Command_Buffer_Context;
struct Vulkan_Context;

struct Buffer_Context
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
    VkDeviceSize vertex_buffer_capacity = 0;
    void* data_index;
    VkDeviceSize index_buffer_capacity = 0;

};

uint32_t findMemoryType(Vulkan_Context& vulkan_context, uint32_t typeFilter, VkMemoryPropertyFlags properties);


void buffer_create(Vulkan_Context& vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);


void buffer_destroy_free(Vulkan_Context& vulkan_context, Buffer_Context& buffer_context);;

void buffer_copy(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_index, VkBuffer srcBuffer,
                 VkBuffer dstBuffer, VkDeviceSize size);;


#endif //VK_BUFFER_H
