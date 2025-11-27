//
// Created by Adams Humbert on 9/7/2025.

#ifndef VK_VERTEX_BUFFER_H
#define VK_VERTEX_BUFFER_H

#include "camera.h"
#include "vulkan_types.h"
#include "vk_buffer.h"
#include "vk_vertex_buffer.h"


// static uint32_t max_object_count = 10000;
#define vertices_per_object 4;
#define indices_per_object 6;


void create_vertex_buffer(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                          vertex_buffer* vertex_buffer, vertex_info* vertex_info, u32 object_count)
{
    uint32_t MAX_VERTICES = object_count * vertices_per_object;

    // Create buffer large enough for maximum vertices, not just initial vertices
    vertex_buffer->vertex_buffer_capacity = sizeof(vertex_3d) * MAX_VERTICES;
    // Use MAX_VERTICES instead of vertices.size()


    buffer_create(vulkan_context, vertex_buffer->vertex_buffer_capacity, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  &vertex_buffer->vertex_staging_buffer, &vertex_buffer->vertex_staging_buffer_memory);

    // Only copy initial vertices data, but allocate full buffer
    VkDeviceSize initial_data_size = sizeof(vertex_info->vertices) * vertex_info->vertices_size;

    vkMapMemory(vulkan_context->device.logical_device, vertex_buffer->vertex_staging_buffer_memory, 0,
                vertex_buffer->vertex_buffer_capacity, 0,
                &vertex_buffer->data_vertex);

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


void create_index_buffer_new(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
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

    INFO("CREATED VERTEX BUFFER SUCCESS (Size: %d bytes for %d vertices)", vertex_buffer->index_buffer_capacity,
         MAX_INDICES);
}

void create_vertex_and_indices_buffer(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                                      vertex_buffer* vertex_buffer,
                                      vertex_info* vertex_info)
{
    u32 max_object_counts = 1000; // TODO: temporary for now
    create_vertex_buffer(vulkan_context, command_buffer_context, vertex_buffer, vertex_info, max_object_counts);
    create_index_buffer_new(vulkan_context, command_buffer_context, vertex_buffer, vertex_info, max_object_counts);
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

void create_vertex_and_indices_destroy(vulkan_context* vulkan_context, vertex_buffer* vertex_buffer)
{
    vkDestroyBuffer(vulkan_context->device.logical_device, vertex_buffer->index_buffer, 0);
    vkFreeMemory(vulkan_context->device.logical_device, vertex_buffer->index_buffer_memory, 0);

    vkDestroyBuffer(vulkan_context->device.logical_device, vertex_buffer->vertex_buffer, 0);
    vkFreeMemory(vulkan_context->device.logical_device, vertex_buffer->vertex_buffer_memory, 0);
}

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
    uniform_buffer->uniformBuffers = darray_create_reserve(VkBuffer, max_frames);
    uniform_buffer->uniformBuffersMemory = darray_create_reserve(VkDeviceMemory, max_frames);
    uniform_buffer->uniformBuffersMapped = darray_create_reserve(void*, max_frames);


    for (size_t i = 0; i < max_frames; i++)
    {
        buffer_create(context, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      &uniform_buffer->uniformBuffers[i],
                      &uniform_buffer->uniformBuffersMemory[i]);

        vkMapMemory(context->device.logical_device, uniform_buffer->uniformBuffersMemory[i], 0, buffer_size, 0,
                    &uniform_buffer->uniformBuffersMapped[i]);
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
    ubo.view = camera_get_view_matrix_bad();
    // Perspective
    float aspect = (float) context->framebuffer_width / context->framebuffer_height;
    ubo.proj = mat4_perspective(deg_to_rad(90.0f), aspect, 0.1f, 1000.0f);

    memcpy(uniform_buffers->uniformBuffersMapped[image_index], &ubo, sizeof(ubo));
}

void uniform_buffers_destroy(vulkan_context* context, vulkan_uniform_buffer* uniform_buffer)
{
    for (size_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        vkDestroyBuffer(context->device.logical_device, uniform_buffer->uniformBuffers[i], 0);
        vkFreeMemory(context->device.logical_device, uniform_buffer->uniformBuffersMemory[i], 0);
    }
}


void descriptor_pool_create(vulkan_context* context, vulkan_shader_default* shader)
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
                                             &shader->descriptor_pool);
    VK_CHECK(result);
}

void descriptor_set_create(vulkan_context* context, vulkan_shader_default* shader)
{
    //set each layout for the frame to our descriptor set, since in this case it gets updated per frame
    for (u64 i = 0; i < context->swapchain.max_frames_in_flight; ++i)
    {
        shader->per_frame_set_layouts[i] = shader->default_shader_descriptor_set_layout;
    }

    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = shader->descriptor_pool;
    allocInfo.descriptorSetCount = (uint32_t) context->swapchain.max_frames_in_flight;
    allocInfo.pSetLayouts = shader->per_frame_set_layouts;

    if (vkAllocateDescriptorSets(context->device.logical_device, &allocInfo, shader->descriptor_sets) != VK_SUCCESS)
    {
        FATAL("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < context->swapchain.max_frames_in_flight; i++)
    {
        VkDescriptorBufferInfo bufferInfo = {0};
        bufferInfo.buffer = shader->global_uniform_buffers.uniformBuffers[i];
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


#endif //VK_VERTEX_BUFFER_H
