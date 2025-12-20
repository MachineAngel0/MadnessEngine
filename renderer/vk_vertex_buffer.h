//
// Created by Adams Humbert on 9/7/2025.

#ifndef VK_VERTEX_BUFFER_H
#define VK_VERTEX_BUFFER_H

#include "camera.h"
#include "vulkan_types.h"
#include "vk_buffer.h"
#include "vk_descriptors.h"
#include "vk_vertex_buffer.h"


// static uint32_t max_object_count = 10000;
#define vertices_per_object 4;
#define indices_per_object 6;

void createVertexBuffer(vulkan_context* vulkan_context, vulkan_buffer* vertex_buffer, vulkan_buffer* index_buffer,
                        vertex_info* vertex_info)
{
    // A note on memory management in Vulkan in general:
    //	This is a complex topic and while it's fine for an example application to small individual memory allocations that is not
    //	what should be done a real-world application, where you should allocate large chunks of memory at once instead.

    VkDevice device = vulkan_context->device.logical_device;

    // Setup vertices
    const vertex_3d vertices[] = {
        {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };
    vertex_info->vertices_size = ARRAY_SIZE(vertices);
    u32 vertexBufferSize = vertex_info->vertices_size * sizeof(vertex_3d);

    // Setup indices
    // We do this for demonstration purposes, a triangle doesn't require indices to be rendered (because of the 1:1 mapping), but more complex shapes usually make use of indices
    u32 indices[] = {0, 1, 2};
    vertex_info->indices_size = ARRAY_SIZE(indices);
    uint32_t indexBufferSize = vertex_info->indices_size * sizeof(uint32_t);

    VkMemoryAllocateInfo memAlloc = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    VkMemoryRequirements memReqs;

    // Static data like vertex and index buffer should be stored on the device memory for optimal (and fastest) access by the GPU
    //
    // To achieve this we use so-called "staging buffers" :
    // - Create a buffer that's visible to the host (and can be mapped)
    // - Copy the data to this buffer
    // - Create another buffer that's local on the device (VRAM) with the same size
    // - Copy the data from the host to the device using a command buffer
    // - Delete the host visible (staging) buffer
    // - Use the device local buffers for rendering
    //
    // Note: On unified memory architectures where host (CPU) and GPU share the same memory, staging is not necessary
    // To keep this sample easy to follow, there is no check for that in place

    // Create the host visible staging buffer that we copy vertices and indices too, and from which we copy to the device
    vulkan_buffer staging_buffer;
    VkBufferCreateInfo stagingBufferCI = {0};
    stagingBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferCI.size = vertexBufferSize + indexBufferSize;
    // Buffer is used as the copy source
    stagingBufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    // Create a host-visible buffer to copy the vertex data to (staging buffer)
    VK_CHECK(vkCreateBuffer(device, &stagingBufferCI, 0, &staging_buffer.handle));
    vkGetBufferMemoryRequirements(device, staging_buffer.handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    // Request a host visible memory type that can be used to copy our data to
    // Also request it to be coherent, so that writes are visible to the GPU right after unmapping the buffer
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, 0, &staging_buffer.memory));
    VK_CHECK(vkBindBufferMemory(device, staging_buffer.handle, staging_buffer.memory, 0));
    // Map the buffer and copy vertices and indices into it, this way we can use a single buffer as the source for both vertex and index GPU buffers
    uint8_t* data = {0};
    VK_CHECK(vkMapMemory(device, staging_buffer.memory, 0, memAlloc.allocationSize, 0, (void**)&data));
    memcpy(data, vertices, vertexBufferSize);
    memcpy(((char *) data) + vertexBufferSize, indices, indexBufferSize);


    // Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
    VkBufferCreateInfo vertexbufferCI = {0};
    vertexbufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexbufferCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vertexbufferCI.size = vertexBufferSize;
    VK_CHECK(vkCreateBuffer(device, &vertexbufferCI, 0, &vertex_buffer->handle));
    vkGetBufferMemoryRequirements(device, vertex_buffer->handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, vulkan_context->allocator, &vertex_buffer->memory));
    VK_CHECK(vkBindBufferMemory(device, vertex_buffer->handle, vertex_buffer->memory, 0));

    // Create a device local buffer to which the (host local) index data will be copied and which will be used for rendering
    VkBufferCreateInfo indexbufferCI = {0};
    indexbufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexbufferCI.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    indexbufferCI.size = indexBufferSize;
    VK_CHECK(vkCreateBuffer(device, &indexbufferCI, vulkan_context->allocator, &index_buffer->handle));
    vkGetBufferMemoryRequirements(device, index_buffer->handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, vulkan_context->allocator, &index_buffer->memory));
    VK_CHECK(vkBindBufferMemory(device, index_buffer->handle, index_buffer->memory, 0));

    // Buffer copies have to be submitted to a queue, so we need a command buffer for them
    VkCommandBuffer copyCmd;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {0};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = vulkan_context->graphics_command_pool;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;
    VK_CHECK(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &copyCmd));

    VkCommandBufferBeginInfo cmdBufInfo = {0};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));
    // Copy vertex and index buffers to the device
    VkBufferCopy copyRegion = {0};
    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, staging_buffer.handle, vertex_buffer->handle, 1, &copyRegion);
    copyRegion.size = indexBufferSize;
    // Indices are stored after the vertices in the source buffer, so we need to add an offset
    copyRegion.srcOffset = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, staging_buffer.handle, index_buffer->handle, 1, &copyRegion);
    VK_CHECK(vkEndCommandBuffer(copyCmd));

    // Submit the command buffer to the queue to finish the copy
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCI = {0};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK(vkCreateFence(device, &fenceCI, vulkan_context->allocator, &fence));
    // Submit copies to the queue
    VK_CHECK(vkQueueSubmit(vulkan_context->device.graphics_queue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX));
    vkDestroyFence(device, fence, 0);
    vkFreeCommandBuffers(device, vulkan_context->graphics_command_pool, 1, &copyCmd);

    // The fence made sure copies are finished, so we can safely delete the staging buffer
    vkDestroyBuffer(device, staging_buffer.handle, vulkan_context->allocator);
    vkFreeMemory(device, staging_buffer.memory, vulkan_context->allocator);
}

void createVertexBufferTexture(vulkan_context* vulkan_context, vulkan_shader_texture* shader_texture)
{
    // A note on memory management in Vulkan in general:
    //	This is a complex topic and while it's fine for an example application to small individual memory allocations that is not
    //	what should be done a real-world application, where you should allocate large chunks of memory at once instead.

    VkDevice device = vulkan_context->device.logical_device;

    // Setup vertices
    // const vertex_tex vertices[] = {
    //     {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},{1.0f, 1.0f}},
    //     {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {-1.0f, 1.0f}},
    //     {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f}}
    // };

    const vertex_tex vertices[] = {
        {{-1.0f + 2.0f, -1.0f + 2.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{1.0f + 2.0f, -1.0f + 2.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f + 2.0f, 1.0f + 2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-1.0f + 2.0f, 1.0f + 2.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };


    // const vertex vertices[] = {
    //     {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    //     {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    //     {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    // };

    shader_texture->vertex_info.vertices_size = ARRAY_SIZE(vertices);
    u32 vertexBufferSize = shader_texture->vertex_info.vertices_size * sizeof(vertex_tex);

    // Setup indices
    // We do this for demonstration purposes, a triangle doesn't require indices to be rendered (because of the 1:1 mapping), but more complex shapes usually make use of indices
    u32 indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    shader_texture->vertex_info.indices_size = ARRAY_SIZE(indices);
    uint32_t indexBufferSize = shader_texture->vertex_info.indices_size * sizeof(uint32_t);

    VkMemoryAllocateInfo memAlloc = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    VkMemoryRequirements memReqs;

    // Static data like vertex and index buffer should be stored on the device memory for optimal (and fastest) access by the GPU
    //
    // To achieve this we use so-called "staging buffers" :
    // - Create a buffer that's visible to the host (and can be mapped)
    // - Copy the data to this buffer
    // - Create another buffer that's local on the device (VRAM) with the same size
    // - Copy the data from the host to the device using a command buffer
    // - Delete the host visible (staging) buffer
    // - Use the device local buffers for rendering
    //
    // Note: On unified memory architectures where host (CPU) and GPU share the same memory, staging is not necessary
    // To keep this sample easy to follow, there is no check for that in place

    // Create the host visible staging buffer that we copy vertices and indices too, and from which we copy to the device
    vulkan_buffer staging_buffer;
    VkBufferCreateInfo stagingBufferCI = {0};
    stagingBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferCI.size = vertexBufferSize + indexBufferSize;
    // Buffer is used as the copy source
    stagingBufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    // Create a host-visible buffer to copy the vertex data to (staging buffer)
    VK_CHECK(vkCreateBuffer(device, &stagingBufferCI, 0, &staging_buffer.handle));
    vkGetBufferMemoryRequirements(device, staging_buffer.handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    // Request a host visible memory type that can be used to copy our data to
    // Also request it to be coherent, so that writes are visible to the GPU right after unmapping the buffer
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, 0, &staging_buffer.memory));
    VK_CHECK(vkBindBufferMemory(device, staging_buffer.handle, staging_buffer.memory, 0));
    // Map the buffer and copy vertices and indices into it, this way we can use a single buffer as the source for both vertex and index GPU buffers
    uint8_t* data = {0};
    VK_CHECK(vkMapMemory(device, staging_buffer.memory, 0, memAlloc.allocationSize, 0, (void**)&data));
    memcpy(data, vertices, vertexBufferSize);
    memcpy(((char *) data) + vertexBufferSize, indices, indexBufferSize);


    // Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
    VkBufferCreateInfo vertexbufferCI = {0};
    vertexbufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexbufferCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vertexbufferCI.size = vertexBufferSize;
    VK_CHECK(vkCreateBuffer(device, &vertexbufferCI, 0, &shader_texture->vertex_buffer.handle));
    vkGetBufferMemoryRequirements(device, shader_texture->vertex_buffer.handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, vulkan_context->allocator, &shader_texture->vertex_buffer.memory));
    VK_CHECK(vkBindBufferMemory(device, shader_texture->vertex_buffer.handle, shader_texture->vertex_buffer.memory, 0));

    // Create a device local buffer to which the (host local) index data will be copied and which will be used for rendering
    VkBufferCreateInfo indexbufferCI = {0};
    indexbufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexbufferCI.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    indexbufferCI.size = indexBufferSize;
    VK_CHECK(vkCreateBuffer(device, &indexbufferCI, vulkan_context->allocator, &shader_texture->index_buffer.handle));
    vkGetBufferMemoryRequirements(device, shader_texture->index_buffer.handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, vulkan_context->allocator, &shader_texture->index_buffer.memory));
    VK_CHECK(vkBindBufferMemory(device, shader_texture->index_buffer.handle, shader_texture->index_buffer.memory, 0));

    // Buffer copies have to be submitted to a queue, so we need a command buffer for them
    VkCommandBuffer copyCmd;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {0};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = vulkan_context->graphics_command_pool;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;
    VK_CHECK(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &copyCmd));

    VkCommandBufferBeginInfo cmdBufInfo = {0};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));
    // Copy vertex and index buffers to the device
    VkBufferCopy copyRegion = {0};
    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, staging_buffer.handle, shader_texture->vertex_buffer.handle, 1, &copyRegion);
    copyRegion.size = indexBufferSize;
    // Indices are stored after the vertices in the source buffer, so we need to add an offset
    copyRegion.srcOffset = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, staging_buffer.handle, shader_texture->index_buffer.handle, 1, &copyRegion);
    VK_CHECK(vkEndCommandBuffer(copyCmd));

    // Submit the command buffer to the queue to finish the copy
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCI = {0};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK(vkCreateFence(device, &fenceCI, vulkan_context->allocator, &fence));
    // Submit copies to the queue
    VK_CHECK(vkQueueSubmit(vulkan_context->device.graphics_queue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX));
    vkDestroyFence(device, fence, 0);
    vkFreeCommandBuffers(device, vulkan_context->graphics_command_pool, 1, &copyCmd);

    // The fence made sure copies are finished, so we can safely delete the staging buffer
    vkDestroyBuffer(device, staging_buffer.handle, vulkan_context->allocator);
    vkFreeMemory(device, staging_buffer.memory, vulkan_context->allocator);
}

void createVertexBufferMesh(vulkan_context* vulkan_context, vulkan_mesh_default* default_mesh, mesh* test_mesh)
{
    // A note on memory management in Vulkan in general:
    //	This is a complex topic and while it's fine for an example application to small individual memory allocations that is not
    //	what should be done a real-world application, where you should allocate large chunks of memory at once instead.

    VkDevice device = vulkan_context->device.logical_device;

    u64 pos_size = darray_get_size(test_mesh->vertices.pos);
    default_mesh->vertex_info.vertices_size = pos_size;
    u32 vertexBufferSize = default_mesh->vertex_info.vertices_size * (sizeof(vec3)); //vec3

    // Setup indices
    // We do this for demonstration purposes, a triangle doesn't require indices to be rendered (because of the 1:1 mapping), but more complex shapes usually make use of indices

    default_mesh->vertex_info.indices_size = test_mesh->indices_size;
    uint32_t indexBufferSize;
    if (test_mesh->index_type == VK_INDEX_TYPE_UINT32)
    {
        indexBufferSize = default_mesh->vertex_info.indices_size * sizeof(uint32_t);
    }
    else if (test_mesh->index_type == VK_INDEX_TYPE_UINT16)
    {
        indexBufferSize = default_mesh->vertex_info.indices_size * sizeof(uint16_t);
    }

    VkMemoryAllocateInfo memAlloc = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    VkMemoryRequirements memReqs;

    // Static data like vertex and index buffer should be stored on the device memory for optimal (and fastest) access by the GPU
    //
    // To achieve this we use so-called "staging buffers" :
    // - Create a buffer that's visible to the host (and can be mapped)
    // - Copy the data to this buffer
    // - Create another buffer that's local on the device (VRAM) with the same size
    // - Copy the data from the host to the device using a command buffer
    // - Delete the host visible (staging) buffer
    // - Use the device local buffers for rendering
    //
    // Note: On unified memory architectures where host (CPU) and GPU share the same memory, staging is not necessary
    // To keep this sample easy to follow, there is no check for that in place

    // Create the host visible staging buffer that we copy vertices and indices too, and from which we copy to the device
    vulkan_buffer staging_buffer;
    VkBufferCreateInfo stagingBufferCI = {0};
    stagingBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferCI.size = vertexBufferSize + indexBufferSize;
    // Buffer is used as the copy source
    stagingBufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    // Create a host-visible buffer to copy the vertex data to (staging buffer)
    VK_CHECK(vkCreateBuffer(device, &stagingBufferCI, 0, &staging_buffer.handle));
    vkGetBufferMemoryRequirements(device, staging_buffer.handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    // Request a host visible memory type that can be used to copy our data to
    // Also request it to be coherent, so that writes are visible to the GPU right after unmapping the buffer
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, 0, &staging_buffer.memory));
    VK_CHECK(vkBindBufferMemory(device, staging_buffer.handle, staging_buffer.memory, 0));
    // Map the buffer and copy vertices and indices into it, this way we can use a single buffer as the source for both vertex and index GPU buffers
    uint8_t* data = {0};
    VK_CHECK(vkMapMemory(device, staging_buffer.memory, 0, memAlloc.allocationSize, 0, (void**)&data));
    memcpy(data, test_mesh->vertices.pos, vertexBufferSize);
    memcpy(((char *) data) + vertexBufferSize, test_mesh->indices, indexBufferSize);


    // Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
    VkBufferCreateInfo vertexbufferCI = {0};
    vertexbufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexbufferCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vertexbufferCI.size = vertexBufferSize;
    VK_CHECK(vkCreateBuffer(device, &vertexbufferCI, 0, &default_mesh->vertex_buffer.handle));
    vkGetBufferMemoryRequirements(device, default_mesh->vertex_buffer.handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, vulkan_context->allocator, &default_mesh->vertex_buffer.memory));
    VK_CHECK(vkBindBufferMemory(device, default_mesh->vertex_buffer.handle, default_mesh->vertex_buffer.memory, 0));

    // Create a device local buffer to which the (host local) index data will be copied and which will be used for rendering
    VkBufferCreateInfo indexbufferCI = {0};
    indexbufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexbufferCI.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    indexbufferCI.size = indexBufferSize;
    VK_CHECK(vkCreateBuffer(device, &indexbufferCI, vulkan_context->allocator, &default_mesh->index_buffer.handle));
    vkGetBufferMemoryRequirements(device, default_mesh->index_buffer.handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, vulkan_context->allocator, &default_mesh->index_buffer.memory));
    VK_CHECK(vkBindBufferMemory(device, default_mesh->index_buffer.handle, default_mesh->index_buffer.memory, 0));

    // Buffer copies have to be submitted to a queue, so we need a command buffer for them
    VkCommandBuffer copyCmd;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {0};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = vulkan_context->graphics_command_pool;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;
    VK_CHECK(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &copyCmd));

    VkCommandBufferBeginInfo cmdBufInfo = {0};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));
    // Copy vertex and index buffers to the device
    VkBufferCopy copyRegion = {0};
    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, staging_buffer.handle, default_mesh->vertex_buffer.handle, 1, &copyRegion);
    copyRegion.size = indexBufferSize;
    // Indices are stored after the vertices in the source buffer, so we need to add an offset
    copyRegion.srcOffset = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, staging_buffer.handle, default_mesh->index_buffer.handle, 1, &copyRegion);
    VK_CHECK(vkEndCommandBuffer(copyCmd));

    // Submit the command buffer to the queue to finish the copy
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCI = {0};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK(vkCreateFence(device, &fenceCI, vulkan_context->allocator, &fence));
    // Submit copies to the queue
    VK_CHECK(vkQueueSubmit(vulkan_context->device.graphics_queue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX));
    vkDestroyFence(device, fence, 0);
    vkFreeCommandBuffers(device, vulkan_context->graphics_command_pool, 1, &copyCmd);

    // The fence made sure copies are finished, so we can safely delete the staging buffer
    vkDestroyBuffer(device, staging_buffer.handle, vulkan_context->allocator);
    vkFreeMemory(device, staging_buffer.memory, vulkan_context->allocator);
}

void createVertexBufferMesh_no_index_buffer(vulkan_context* vulkan_context, vulkan_mesh_default* default_mesh,
                                            mesh* test_mesh)
{
    // A note on memory management in Vulkan in general:
    //	This is a complex topic and while it's fine for an example application to small individual memory allocations that is not
    //	what should be done a real-world application, where you should allocate large chunks of memory at once instead.

    VkDevice device = vulkan_context->device.logical_device;

    // Setup vertices
    // const vertex_tex vertices[] = {
    //     {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},{1.0f, 1.0f}},
    //     {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {-1.0f, 1.0f}},
    //     {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f}}
    // };


    // const vertex vertices[] = {
    //     {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    //     {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    //     {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    // };

    u64 pos_size = darray_get_size(test_mesh->vertices.pos);

    default_mesh->vertex_info.vertices_size = pos_size;
    // u32 vertexBufferSize = default_mesh->vertex_info.vertices_size * sizeof(vertex_mesh);
    u32 vertexBufferSize = default_mesh->vertex_info.vertices_size * sizeof(vec3);

    // Setup indices
    // We do this for demonstration purposes, a triangle doesn't require indices to be rendered (because of the 1:1 mapping), but more complex shapes usually make use of indices

    VkMemoryAllocateInfo memAlloc = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    VkMemoryRequirements memReqs;

    // Static data like vertex and index buffer should be stored on the device memory for optimal (and fastest) access by the GPU
    //
    // To achieve this we use so-called "staging buffers" :
    // - Create a buffer that's visible to the host (and can be mapped)
    // - Copy the data to this buffer
    // - Create another buffer that's local on the device (VRAM) with the same size
    // - Copy the data from the host to the device using a command buffer
    // - Delete the host visible (staging) buffer
    // - Use the device local buffers for rendering
    //
    // Note: On unified memory architectures where host (CPU) and GPU share the same memory, staging is not necessary
    // To keep this sample easy to follow, there is no check for that in place

    // Create the host visible staging buffer that we copy vertices and indices too, and from which we copy to the device
    vulkan_buffer staging_buffer;
    VkBufferCreateInfo stagingBufferCI = {0};
    stagingBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferCI.size = vertexBufferSize;
    // Buffer is used as the copy source
    stagingBufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    // Create a host-visible buffer to copy the vertex data to (staging buffer)
    VK_CHECK(vkCreateBuffer(device, &stagingBufferCI, 0, &staging_buffer.handle));
    vkGetBufferMemoryRequirements(device, staging_buffer.handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    // Request a host visible memory type that can be used to copy our data to
    // Also request it to be coherent, so that writes are visible to the GPU right after unmapping the buffer
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, 0, &staging_buffer.memory));
    VK_CHECK(vkBindBufferMemory(device, staging_buffer.handle, staging_buffer.memory, 0));
    // Map the buffer and copy vertices and indices into it, this way we can use a single buffer as the source for both vertex and index GPU buffers
    uint8_t* data = {0};
    VK_CHECK(vkMapMemory(device, staging_buffer.memory, 0, memAlloc.allocationSize, 0, (void**)&data));
    memcpy(data, test_mesh->vertices.pos, vertexBufferSize);


    // Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
    VkBufferCreateInfo vertexbufferCI = {0};
    vertexbufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexbufferCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vertexbufferCI.size = vertexBufferSize;
    VK_CHECK(vkCreateBuffer(device, &vertexbufferCI, 0, &default_mesh->vertex_buffer.handle));
    vkGetBufferMemoryRequirements(device, default_mesh->vertex_buffer.handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = find_memory_type(vulkan_context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK(vkAllocateMemory(device, &memAlloc, vulkan_context->allocator, &default_mesh->vertex_buffer.memory));
    VK_CHECK(vkBindBufferMemory(device, default_mesh->vertex_buffer.handle, default_mesh->vertex_buffer.memory, 0));

    // Buffer copies have to be submitted to a queue, so we need a command buffer for them
    VkCommandBuffer copyCmd;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {0};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = vulkan_context->graphics_command_pool;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;
    VK_CHECK(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &copyCmd));

    VkCommandBufferBeginInfo cmdBufInfo = {0};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));
    // Copy vertex and index buffers to the device
    VkBufferCopy copyRegion = {0};
    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, staging_buffer.handle, default_mesh->vertex_buffer.handle, 1, &copyRegion);
    VK_CHECK(vkEndCommandBuffer(copyCmd));

    // Submit the command buffer to the queue to finish the copy
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCI = {0};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK(vkCreateFence(device, &fenceCI, vulkan_context->allocator, &fence));
    // Submit copies to the queue
    VK_CHECK(vkQueueSubmit(vulkan_context->device.graphics_queue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX));
    vkDestroyFence(device, fence, 0);
    vkFreeCommandBuffers(device, vulkan_context->graphics_command_pool, 1, &copyCmd);

    // The fence made sure copies are finished, so we can safely delete the staging buffer
    vkDestroyBuffer(device, staging_buffer.handle, vulkan_context->allocator);
    vkFreeMemory(device, staging_buffer.memory, vulkan_context->allocator);
}


/*
void create_vertex_buffer(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                         vertex_buffer* vertex_buffer, vertex_info* vertex_info, u32 object_count)
{
   uint32_t MAX_VERTICES = object_count * vertices_per_object;

   // Create buffer large enough for maximum vertices, not just initial vertices
   vertex_buffer->vertex_buffer_capacity = sizeof(vertex) * MAX_VERTICES;
   // Use MAX_VERTICES instead of vertices.size()


   buffer_create(vulkan_context, vertex_buffer->vertex_buffer_capacity, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &vertex_buffer->vertex_staging_buffer, &vertex_buffer->vertex_staging_buffer_memory);

   // Only copy initial vertices data, but allocate full buffer
   VkDeviceSize initial_data_size = sizeof(vertex_info->vertices) * vertex_info->vertices_size;

   VK_CHECK(vkMapMemory(vulkan_context->device.logical_device, vertex_buffer->vertex_staging_buffer_memory, 0,
       vertex_buffer->vertex_buffer_capacity, 0,
       &vertex_buffer->data_vertex));

   // Zero out the entire buffer first
   memset(vertex_buffer->data_vertex, 0, vertex_buffer->vertex_buffer_capacity);
   // Then copy initial data
   memcpy(vertex_buffer->data_vertex, vertex_info->vertices, initial_data_size);
   vkUnmapMemory(vulkan_context->device.logical_device, vertex_buffer->vertex_staging_buffer_memory);

   // Create device local buffer with full size
   buffer_create(vulkan_context, vertex_buffer->vertex_buffer_capacity,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertex_buffer->vertex_buffer,
                 &vertex_buffer->vertex_buffer_memory);

   // Copy entire buffer (including zeros for unused space)
   buffer_copy(vulkan_context, command_buffer_context, vertex_buffer->vertex_staging_buffer,
               vertex_buffer->vertex_buffer,
               vertex_buffer->vertex_buffer_capacity);


   INFO("CREATED VERTEX BUFFER SUCCESS (Size: %d bytes for %d vertices)", vertex_buffer->vertex_buffer_capacity,
        MAX_VERTICES);
}


void create_index_buffer(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                        vertex_buffer* vertex_buffer, vertex_info* vertex_info, u32 object_count)
{
   uint32_t MAX_INDICES = object_count * indices_per_object;

   // Create buffer large enough for maximum indices, not just initial indices
   vertex_buffer->index_buffer_capacity = sizeof(uint16_t) * MAX_INDICES; // Use MAX_INDICES instead of indices.size()

   // Create staging buffer
   buffer_create(vulkan_context, vertex_buffer->index_buffer_capacity, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &vertex_buffer->index_staging_buffer, &vertex_buffer->index_staging_buffer_memory);

   // Only copy initial indices data, but allocate full buffer
   VkDeviceSize initial_data_size = sizeof(vertex_info->indices[0]) * vertex_info->indices_size;

   vkMapMemory(vulkan_context->device.logical_device, vertex_buffer->index_staging_buffer_memory, 0,
               vertex_buffer->index_buffer_capacity, 0, &vertex_buffer->data_index);

   // Zero out the entire buffer first
   memset(vertex_buffer->data_index, 0, vertex_buffer->index_buffer_capacity);
   // Then copy initial data
   memcpy(vertex_buffer->data_index, vertex_info->indices, initial_data_size);

   vkUnmapMemory(vulkan_context->device.logical_device, vertex_buffer->index_staging_buffer_memory);

   // Create device local buffer with full size
   buffer_create(vulkan_context, vertex_buffer->index_buffer_capacity,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertex_buffer->index_buffer,
                 &vertex_buffer->index_buffer_memory);

   // Copy entire buffer
   buffer_copy(vulkan_context, command_buffer_context, vertex_buffer->index_staging_buffer,
               vertex_buffer->index_buffer,
               vertex_buffer->index_buffer_capacity);

   INFO("CREATED INDEX BUFFER SUCCESS (Size: %d bytes for %d vertices)", vertex_buffer->index_buffer_capacity,
        MAX_INDICES);
}

void create_vertex_and_indices_buffer(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                                     vertex_buffer* vertex_buffer,
                                     vertex_info* vertex_info)
{
   u32 max_object_counts = 1000; // TODO: temporary for now
   create_vertex_buffer(vulkan_context, command_buffer_context, vertex_buffer, vertex_info, max_object_counts);
   create_index_buffer(vulkan_context, command_buffer_context, vertex_buffer, vertex_info, max_object_counts);
}

void vertex_buffer_update(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                         vertex_buffer* vertex_buffer, vertex_info* vertex_info)
{
   //TODO: so while not happening rn, we can use an offset to only copy the new data, instead of the whole buffer over
   //so instead of copy buffer we can use copy_buffer_region, which ill have to double check how it works
   // if (!vertex_buffer.vertex_buffer_should_update) return;

   // Only copy we are currently using vertices data
   VkDeviceSize current_vertex_data_size = sizeof(vertex_info->vertices[0]) * (vertex_info->vertices_size);

   //map and copy data
   //NOTE: YOU TECHNICALLY DONT NEED TO MAP AND UNMAP THE DATA, YOU CAN JUST COPY THE DATA OVER

   //vkMapMemory(vulkan_context.logical_device, command_buffer_context.vertex_staging_buffer_memory, 0, vertex_buffer_capacity, 0,
   //           &data_vertex);
   memcpy(vertex_buffer->data_vertex, vertex_info->vertices, current_vertex_data_size);
   //vkUnmapMemory(vulkan_context.logical_device, command_buffer_context.vertex_staging_buffer_memory);

   //transfer from storage buffer/cpu buffer to gpu buffer
   buffer_copy(vulkan_context, command_buffer_context, vertex_buffer->vertex_staging_buffer,
               vertex_buffer->vertex_buffer,
               current_vertex_data_size);
   //last param in copy_buffer used to be vertex_buffer_capacity and index_buffer_capacity respectively


   VkDeviceSize current_index_data_size = sizeof(vertex_info->indices[0]) * vertex_info->indices_size;

   //vkMapMemory(vulkan_context.logical_device, command_buffer_context.index_staging_buffer_memory, 0, index_buffer_capacity, 0, &data_index);
   memcpy(vertex_buffer->data_index, vertex_info->indices, current_index_data_size);
   //vkUnmapMemory(vulkan_context.logical_device, command_buffer_context.index_staging_buffer_memory);

   buffer_copy(vulkan_context, command_buffer_context, vertex_buffer->index_staging_buffer,
               vertex_buffer->index_buffer,
               current_index_data_size);
}
*/
void vertex_info_allocate(vertex_info* vertex_info)
{
    darray_clear(vertex_info->vertices);
    darray_clear(vertex_info->indices);
}


void vertex_info_clear(vertex_info* vertex_info)
{
    darray_clear(vertex_info->vertices);
    darray_clear(vertex_info->indices);
}


void uniform_buffers_create(vulkan_context* context, vulkan_uniform_buffer* uniform_buffer)
{
    u8 max_frames = context->swapchain.max_frames_in_flight;
    VkDeviceSize buffer_size = sizeof(uniform_buffer_object);
    uniform_buffer->uniform_buffers = darray_create_reserve(VkBuffer, max_frames);
    uniform_buffer->uniform_buffers_memory = darray_create_reserve(VkDeviceMemory, max_frames);
    uniform_buffer->uniform_buffers_mapped = darray_create_reserve(void*, max_frames);


    for (size_t i = 0; i < max_frames; i++)
    {
        buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      &uniform_buffer->uniform_buffers[i],
                      &uniform_buffer->uniform_buffers_memory[i]);

        vkMapMemory(context->device.logical_device, uniform_buffer->uniform_buffers_memory[i], 0, buffer_size, 0,
                    &uniform_buffer->uniform_buffers_mapped[i]);
    }
}


void uniform_buffer_update(vulkan_context* context, vulkan_uniform_buffer* uniform_buffers, u32 image_index, float time,
                           camera* camera)
{
    uniform_buffer_object ubo = {0};

    // Rotate around Z (up) for simplicity
    quat q = quat_from_axis_angle(vec3_up(), deg_to_rad(90.0f) * time, true);
    ubo.model = quat_to_rotation_matrix(q, (vec3){0.0f, 0.0f, 0.0f});
    // ubo.view =  camera_get_view_matrix(camera);
    // ubo.view = camera_get_view_matrix_bad();
    // Perspective
    float aspect = (float) context->framebuffer_width / context->framebuffer_height;
    ubo.proj = mat4_perspective(deg_to_rad(90.0f), aspect, 0.1f, 1000.0f);

    memcpy(uniform_buffers->uniform_buffers_mapped[image_index], &ubo, sizeof(ubo));
}

void uniform_buffers_destroy(vulkan_context* context, vulkan_uniform_buffer* uniform_buffer)
{
    for (size_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        vkDestroyBuffer(context->device.logical_device, uniform_buffer->uniform_buffers[i], 0);
        vkFreeMemory(context->device.logical_device, uniform_buffer->uniform_buffers_memory[i], 0);
    }
}


void descriptor_pool_create(vulkan_context* context, descriptor_pool_allocator* descriptor_pool,
                            vulkan_shader_default* shader)
{
    VkDescriptorPoolSize poolSize = {0};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = (uint32_t) context->swapchain.max_frames_in_flight;

    VkDescriptorPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = (uint32_t) context->swapchain.max_frames_in_flight;

    VkResult result = vkCreateDescriptorPool(context->device.logical_device, &poolInfo, context->allocator,
                                             &descriptor_pool->descriptor_pool);
    VK_CHECK(result);
}

void descriptor_set_create(vulkan_context* context, descriptor_pool_allocator* descriptor_pool,
                           vulkan_shader_default* shader)
{
    shader->descriptor_sets = darray_create_reserve(VkDescriptorSet, context->swapchain.max_frames_in_flight);

    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptor_pool->descriptor_pool;
    allocInfo.descriptorSetCount = (uint32_t) context->swapchain.max_frames_in_flight;
    allocInfo.pSetLayouts = &shader->descriptor_set_layout;

    if (vkAllocateDescriptorSets(context->device.logical_device, &allocInfo, shader->descriptor_sets) != VK_SUCCESS)
    {
        FATAL("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        VkDescriptorBufferInfo bufferInfo = {0};
        bufferInfo.buffer = context->global_uniform_buffers.uniform_buffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(uniform_buffer_object);

        VkWriteDescriptorSet descriptorWrite = {0};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = shader->descriptor_sets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(context->device.logical_device, 1, &descriptorWrite, 0, NULL);
    }
}

void descriptor_pool_destroy(vulkan_context* context, VkDescriptorPool* descriptorPool)
{
    vkDestroyDescriptorPool(context->device.logical_device, *descriptorPool, 0);
}

void descriptor_set_layout_destroy(vulkan_context* context, VkDescriptorSetLayout* descriptor_set_layout)
{
    vkDestroyDescriptorSetLayout(context->device.logical_device, *descriptor_set_layout, 0);
}

void create_global_uniform_buffer(vulkan_context* context)
{
    u8 max_frames = context->swapchain.max_frames_in_flight;
    context->global_uniform_buffers.uniform_buffers = darray_create_reserve(VkBuffer, max_frames);
    context->global_uniform_buffers.uniform_buffers_memory = darray_create_reserve(
        VkDeviceMemory, max_frames);
    context->global_uniform_buffers.uniform_buffers_mapped = darray_create_reserve(
        void*, max_frames);

    // Prepare and initialize the per-frame uniform buffer blocks containing shader uniforms
    // Single uniforms like in OpenGL are no longer present in Vulkan. All shader uniforms are passed via uniform buffer blocks
    VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = sizeof(uniform_buffer_object);
    // This buffer will be used as a uniform buffer
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;


    // Create the buffers
    for (uint32_t i = 0; i < max_frames; i++)
    {
        VK_CHECK(
            vkCreateBuffer(context->device.logical_device, &bufferInfo, 0, &context->global_uniform_buffers.
                uniform_buffers[i]));
        // Get memory requirements including size, alignment and memory type based on the buffer type we request (uniform buffer)
        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(context->device.logical_device,
                                      context->global_uniform_buffers.uniform_buffers[i], &memReqs);
        VkMemoryAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        // Note that we use the size we got from the memory requirements and not the actual buffer size, as the former may be larger due to alignment requirements of the device
        allocInfo.allocationSize = memReqs.size;
        // Get the memory type index that supports host visible memory access
        // Most implementations offer multiple memory types and selecting the correct one to allocate memory from is crucial
        // We also want the buffer to be host coherent so we don't have to flush (or sync after every update).
        allocInfo.memoryTypeIndex = find_memory_type(context, memReqs.memoryTypeBits,
                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        // Allocate memory for the uniform buffer
        VK_CHECK(
            vkAllocateMemory(context->device.logical_device, &allocInfo, 0,
                &( context->global_uniform_buffers.uniform_buffers_memory[i])));
        // Bind memory to buffer
        VK_CHECK(
            vkBindBufferMemory(context->device.logical_device, context->global_uniform_buffers.uniform_buffers[i],
                context->global_uniform_buffers.uniform_buffers_memory[i], 0));
        // We map the buffer once, so we can update it without having to map it again
        VK_CHECK(vkMapMemory(context->device.logical_device, context->global_uniform_buffers.uniform_buffers_memory[i],
            0, sizeof(uniform_buffer_object), 0,
            &context->global_uniform_buffers.uniform_buffers_mapped[i]));
    }
}

// Descriptors are used to pass data to shaders, for our sample we use a descriptor to pass parameters like matrices to the shader
void createDescriptors(vulkan_context* context, descriptor_pool_allocator* descriptor_pool)
{
    context->default_shader_info.descriptor_sets = darray_create_reserve(
        VkDescriptorSet, context->swapchain.max_frames_in_flight);
    context->default_shader_info.descriptor_set_count = (u32) context->swapchain.max_frames_in_flight;

    // Descriptor set layouts define the interface between our application and the shader
    // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout binding
    // Binding 0: Uniform buffer (Vertex shader)
    VkDescriptorSetLayoutBinding layoutBinding = {0};
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    // layoutBinding.binding

    VkDescriptorSetLayoutCreateInfo descriptorLayoutCI = {0};
    descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutCI.bindingCount = 1;
    descriptorLayoutCI.pBindings = &layoutBinding;

    VK_CHECK(vkCreateDescriptorSetLayout(context->device.logical_device, &descriptorLayoutCI, 0,
        &context->default_shader_info.descriptor_set_layout));

    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight
    for (uint32_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        u32 set_count = 1;
        descriptor_pool_alloc(context, descriptor_pool, &context->default_shader_info.descriptor_set_layout, &set_count,
                              &context->default_shader_info.descriptor_sets[i]);

        // Update the descriptor set determining the shader binding points
        // For every binding point used in a shader there needs to be one
        // descriptor set matching that binding point
        VkWriteDescriptorSet writeDescriptorSet = {0};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        // The buffer's information is passed using a descriptor info structure
        VkDescriptorBufferInfo bufferInfo = {0};
        bufferInfo.buffer = context->global_uniform_buffers.uniform_buffers[i];
        bufferInfo.range = sizeof(uniform_buffer_object);
        bufferInfo.offset = 0;

        // Binding 0 : Uniform buffer
        writeDescriptorSet.dstSet = context->default_shader_info.descriptor_sets[i];
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.pBufferInfo = &bufferInfo;
        writeDescriptorSet.dstBinding = 0;
        vkUpdateDescriptorSets(context->device.logical_device, 1, &writeDescriptorSet, 0, 0);
    }
}


void createDescriptorsTexture(vulkan_context* context, vulkan_shader_texture* shader_texture)
{
    shader_texture->descriptor_sets = darray_create_reserve(
        VkDescriptorSet, context->swapchain.max_frames_in_flight);
    shader_texture->descriptor_set_count = (u32) context->swapchain.max_frames_in_flight;

    // Descriptors are allocated from a pool, that tells the implementation how many and what types of descriptors we are going to use (at maximum)
    VkDescriptorPoolSize descriptor_pools[2] = {0};
    // This example only one descriptor type (uniform buffer)
    descriptor_pools[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // We have one buffer (and as such descriptor) per frame
    descriptor_pools[0].descriptorCount = context->swapchain.max_frames_in_flight;
    //TEXTURE
    descriptor_pools[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_pools[1].descriptorCount = context->swapchain.max_frames_in_flight;

    // Create the global descriptor pool
    // All descriptors used in this example are allocated from this pool
    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    descriptor_pool_create_info.poolSizeCount = 2;
    descriptor_pool_create_info.pPoolSizes = descriptor_pools;
    // Set the max. number of descriptor sets that can be requested from this pool (requesting beyond this limit will result in an error)
    // Our sample will create one set per uniform buffer per frame
    descriptor_pool_create_info.maxSets = (uint32_t) context->swapchain.max_frames_in_flight;
    VK_CHECK(
        vkCreateDescriptorPool(context->device.logical_device, &descriptor_pool_create_info, 0,
            &context->global_descriptor_pool.descriptor_pool));

    // Descriptor set layouts define the interface between our application and the shader
    // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout binding

    // Binding 0: Uniform buffer (Vertex shader)
    VkDescriptorSetLayoutBinding layoutBinding[2] = {0};
    //uniform buffer
    layoutBinding[0].binding = 0;
    layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding[0].descriptorCount = 1;
    layoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    //Binding 1: Sampler/Texture
    layoutBinding[1].binding = 1;
    layoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBinding[1].descriptorCount = 1;
    layoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorLayoutCI = {0};
    descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutCI.bindingCount = 2;
    descriptorLayoutCI.pBindings = layoutBinding;


    VK_CHECK(
        vkCreateDescriptorSetLayout(context->device.logical_device, &descriptorLayoutCI, 0, &shader_texture->
            descriptor_set_layout));

    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight
    for (uint32_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        VkDescriptorSetAllocateInfo allocInfo = {0};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = context->global_descriptor_pool.descriptor_pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &shader_texture->descriptor_set_layout;
        VK_CHECK(
            vkAllocateDescriptorSets(context->device.logical_device, &allocInfo, &shader_texture->descriptor_sets[i]));

        // Update the descriptor set determining the shader binding points
        // For every binding point used in a shader there needs to be one
        // descriptor set matching that binding point
        VkWriteDescriptorSet writeDescriptorSet[2] = {0};
        // The buffer's information is passed using a descriptor info structure
        VkDescriptorBufferInfo bufferInfo = {0};
        bufferInfo.buffer = context->global_uniform_buffers.uniform_buffers[i];
        bufferInfo.range = sizeof(uniform_buffer_object);
        bufferInfo.offset = 0;

        // Binding 0 : Uniform buffer
        writeDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet[0].dstSet = shader_texture->descriptor_sets[i];
        writeDescriptorSet[0].descriptorCount = 1;
        writeDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet[0].pBufferInfo = &bufferInfo;
        writeDescriptorSet[0].dstBinding = 0;
        writeDescriptorSet[0].dstArrayElement = 0;

        VkDescriptorImageInfo image_info = {0};
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.imageView = shader_texture->texture_test_object.texture_image_view;
        image_info.sampler = shader_texture->texture_test_object.texture_sampler;
        // Binding 1 : TEXTURE
        writeDescriptorSet[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet[1].dstSet = shader_texture->descriptor_sets[i];
        writeDescriptorSet[1].descriptorCount = 1;
        writeDescriptorSet[1].dstBinding = 1;
        writeDescriptorSet[1].dstArrayElement = 0;
        writeDescriptorSet[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet[1].pImageInfo = &image_info;

        vkUpdateDescriptorSets(context->device.logical_device, 2, writeDescriptorSet, 0, 0);
    }
}


void createDescriptorsTexture_with_bindless(vulkan_context* context,
                                            vulkan_shader_texture* shader_texture)
{
    shader_texture->descriptor_sets = darray_create_reserve(
        VkDescriptorSet, context->swapchain.max_frames_in_flight);
    shader_texture->descriptor_set_count = (u32) context->swapchain.max_frames_in_flight;


    // Descriptor set layouts define the interface between our application and the shader
    // Basically connects the different shader stages to descriptors for binding uniform buffers, image samplers, etc.
    // So every shader binding should map to one descriptor set layout binding

    // Binding 0: Uniform buffer (Vertex shader)
    VkDescriptorSetLayoutBinding layoutBinding[1] = {0};
    //uniform buffer
    layoutBinding[0].binding = 0;
    layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding[0].descriptorCount = 1;
    layoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;


    VkDescriptorSetLayoutCreateInfo descriptorLayoutCI = {0};
    descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutCI.bindingCount = 1;
    descriptorLayoutCI.pBindings = layoutBinding;


    VK_CHECK(
        vkCreateDescriptorSetLayout(context->device.logical_device, &descriptorLayoutCI, 0, &shader_texture->
            descriptor_set_layout));

    // Where the descriptor set layout is the interface, the descriptor set points to actual data
    // Descriptors that are changed per frame need to be multiplied, so we can update descriptor n+1 while n is still used by the GPU, so we create one per max frame in flight
    for (uint32_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        VkDescriptorSetAllocateInfo allocInfo = {0};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = context->global_descriptor_pool.descriptor_pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &shader_texture->descriptor_set_layout;
        VK_CHECK(
            vkAllocateDescriptorSets(context->device.logical_device, &allocInfo, &shader_texture->descriptor_sets[i]));

        // Update the descriptor set determining the shader binding points
        // For every binding point used in a shader there needs to be one
        // descriptor set matching that binding point
        VkWriteDescriptorSet writeDescriptorSet[1] = {0};
        // The buffer's information is passed using a descriptor info structure
        VkDescriptorBufferInfo bufferInfo = {0};
        bufferInfo.buffer = context->global_uniform_buffers.uniform_buffers[i];
        bufferInfo.range = sizeof(uniform_buffer_object);
        bufferInfo.offset = 0;

        // Binding 0 : Uniform buffer
        writeDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet[0].dstSet = shader_texture->descriptor_sets[i];
        writeDescriptorSet[0].descriptorCount = 1;
        writeDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet[0].pBufferInfo = &bufferInfo;
        writeDescriptorSet[0].dstBinding = 0;
        writeDescriptorSet[0].dstArrayElement = 0;


        vkUpdateDescriptorSets(context->device.logical_device, 1, writeDescriptorSet, 0, 0);
    }
}


#endif //VK_VERTEX_BUFFER_H
