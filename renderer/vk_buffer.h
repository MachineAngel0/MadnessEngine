//
// Created by Adams Humbert on 9/7/2025.
//

#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include "vulkan_types.h"


//TODO: at some point it would make sense to allocate something between 32-128mb of vertex buffers
// at any one time and keep a bunch of them allocated in some sort of free list ready to be used or make them on the fly

buffer_system* buffer_system_init(renderer* renderer);
buffer_system* buffer_system_free(renderer* renderer);


uint32_t find_memory_type(vulkan_context* context, uint32_t type_filter, VkMemoryPropertyFlags properties);

bool buffer_create(vulkan_context* vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);



void buffer_copy(vulkan_context* vulkan_context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void buffer_copy_region(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                        VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                        VkDeviceSize srcOffset, VkDeviceSize dstOffset);

//new API
void vulkan_buffer_vertex_bda_create(renderer* renderer, vulkan_buffer* out_buffer, buffer_type buffer_type,
                                     u64 data_size);
bool vulkan_buffer_free(renderer* renderer, vulkan_buffer* vk_buffer);

void vulkan_staging_buffer_create(renderer* renderer, vulkan_staging_buffer* out_buffer, u64 data_size);

void vulkan_staging_buffer_free(renderer* renderer, vulkan_staging_buffer* staging_buffer);

//for inserting data into a specific memory region of the buffer
void vulkan_buffer_data_insert_from_offset(renderer* renderer, vulkan_buffer* buffer,
                                           void* data, u64 data_size);

/* TODO: dont need rn but could use in the future
void vulkan_buffer_data_insert_specify_offset(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                               vulkan_buffer* buffer, vulkan_buffer* staging_buffer, void* data, u64 data_size, u64 offset);
*/

#endif //VK_BUFFER_H
