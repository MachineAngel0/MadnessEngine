//
// Created by Adams Humbert on 9/7/2025.
//

#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include "vulkan_types.h"


//TODO: at some point it would make sense to allocate something between 32-128mb of vertex buffers
// at any one time and keep a bunch of them allocated in some sort of free list ready to be used or make them on the fly


typedef enum buffer_type
{
    UNIFORM,
    STORAGE, // meant to be used in a descriptor set
    VERTEX, // meant to only be used with vkCmdBindVertexBuffers
    INDEX, // meant to only be used as part of a vkCmdBindIndexBuffer or  vkCmdBindIndexBuffer2
    // meant to used as part of a vkCmdDrawIndirect, vkCmdDrawIndexedIndirect, vkCmdDrawMeshTasksIndirectNV, vkCmdDrawMeshTasksIndirectCountNV, vkCmdDrawMeshTasksIndirectEXT, vkCmdDrawMeshTasksIndirectCountEXT,
    INDIRECT,


    //idk what these are used for yet, but they will probably be useful later
    //UNIFORM_TEXEL,
    //STORAGE_TEXEL,
} buffer_type;

typedef struct buffer_system
{
    //an array of them
    //NOTE: if we run out we can always allocate more, for now we just keep one of each
    vulkan_buffer* vertex_buffers;
    vulkan_buffer* index_buffers;
    vulkan_buffer* storage_buffers;
    vulkan_buffer* uniform_buffers;

    //how many have been allocated
    u32 vertex_buffer_count;
    u32 index_buffer_count;
    u32 storage_buffer_count;
    u32 uniform_buffer_count;


    //NOTE: when we multithread, it would make sense to have more than one of these in something like a ring buffer
    vulkan_staging_buffer vertex_staging_buffer;
    vulkan_staging_buffer index_staging_buffer;
    vulkan_staging_buffer storage_staging_buffer;
    vulkan_staging_buffer uniform_staging_buffer;


    //TODO queries for size
    /*
    u64 temp = vulkan_context->device.properties.limits.maxStorageBufferRange;
    u64 temp1 = vulkan_context->device.properties.limits.maxUniformBufferRange;
    u64 temp3 = vulkan_context->device.properties.limits.maxMemoryAllocationCount;
    */
} buffer_system;

buffer_system* buffer_system_init(renderer* renderer);;
buffer_system* buffer_system_free(renderer* renderer);;


uint32_t find_memory_type(vulkan_context* context, uint32_t type_filter, VkMemoryPropertyFlags properties);

bool buffer_create(vulkan_context* vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);


bool buffer_destroy_free(vulkan_context* vulkan_context, vulkan_buffer* vk_buffer);

void buffer_copy(vulkan_context* vulkan_context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void buffer_copy_region(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                        VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                        VkDeviceSize srcOffset, VkDeviceSize dstOffset);

//new API
void vulkan_buffer_vertex_bda_create(renderer* renderer, vulkan_context* vulkan_context,
                                     buffer_type type, vulkan_buffer* out_buffer, const u64 data_size);

void vulkan_staging_buffer_create(renderer* renderer, vulkan_staging_buffer* out_buffer, const u64 data_size);

//for inserting data into a specific memory region of the buffer
void vulkan_buffer_data_insert_from_offset(renderer* renderer, vulkan_buffer* buffer,
                                           vulkan_staging_buffer* staging_buffer, void* data,
                                           u64 data_size);

/* TODO: dont need rn but could use in the future
void vulkan_buffer_data_insert_specify_offset(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                               vulkan_buffer* buffer, vulkan_buffer* staging_buffer, void* data, u64 data_size, u64 offset);
*/

#endif //VK_BUFFER_H
