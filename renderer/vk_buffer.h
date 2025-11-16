//
// Created by Adams Humbert on 9/7/2025.
//

#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include "vulkan/vulkan.h"
#include "vulkan_types.h"


uint32_t find_memory_type(vulkan_context* vulkan_context, uint32_t typeFilter, VkMemoryPropertyFlags properties);


bool vulkan_buffer_create(
    vulkan_context* context,
    u64 size,
    VkBufferUsageFlagBits usage,
    u32 memory_property_flags,
    b8 bind_on_create,
    vulkan_buffer* out_buffer);


void vulkan_buffer_destroy(vulkan_context* context, vulkan_buffer* buffer);


bool vulkan_buffer_resize(
    vulkan_context* context,
    u64 new_size,
    vulkan_buffer* buffer,
    VkQueue queue,
    VkCommandPool pool);

void vulkan_buffer_bind(vulkan_context* context, vulkan_buffer* buffer, u64 offset);

void* vulkan_buffer_lock_memory(vulkan_context* context, vulkan_buffer* buffer, u64 offset, u64 size, u32 flags);

void vulkan_buffer_unlock_memory(vulkan_context* context, vulkan_buffer* buffer);

void vulkan_buffer_load_data(vulkan_context* context, vulkan_buffer* buffer, u64 offset, u64 size, u32 flags,
                             const void* data);

void vulkan_buffer_copy_to(
    vulkan_context* context,
    VkCommandPool pool,
    VkFence fence,
    VkQueue queue,
    VkBuffer source,
    u64 source_offset,
    VkBuffer dest,
    u64 dest_offset,
    u64 size);


//TODO: clean up/ refactor

// typedef struct Buffer_Context
// {
//     VkBuffer vertex_buffer;
//     VkDeviceMemory vertex_buffer_memory;
//
//     VkBuffer vertex_staging_buffer;
//     VkDeviceMemory vertex_staging_buffer_memory;
//
//     VkBuffer index_buffer;
//     VkDeviceMemory index_buffer_memory;
//
//     VkBuffer index_staging_buffer;
//     VkDeviceMemory index_staging_buffer_memory;
//
//     void* data_vertex;
//     VkDeviceSize vertex_buffer_capacity;
//     void* data_index;
//     VkDeviceSize index_buffer_capacity;
//
// }Buffer_Context;


/*
void buffer_create(vulkan_context* vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);


void buffer_destroy_free(vulkan_context* vulkan_context, Buffer_Context* buffer_context);;

void buffer_copy(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_index, VkBuffer srcBuffer,
                 VkBuffer dstBuffer, VkDeviceSize size);;
*/

#endif //VK_BUFFER_H
