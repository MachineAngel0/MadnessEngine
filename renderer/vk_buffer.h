//
// Created by Adams Humbert on 9/7/2025.
//

#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include "vulkan/vulkan.h"
#include "vulkan_types.h"


uint32_t find_memory_type(vulkan_context* vulkan_context, uint32_t typeFilter, VkMemoryPropertyFlags properties);

bool buffer_create(vulkan_context* vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);


bool buffer_destroy_free(vulkan_context* vulkan_context, vertex_buffer* vertex_buffer);;

void buffer_copy(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                        VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void buffer_copy_region(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                        VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                        VkDeviceSize srcOffset, VkDeviceSize dstOffset);

#endif //VK_BUFFER_H
