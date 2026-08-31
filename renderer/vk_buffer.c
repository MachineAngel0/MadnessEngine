#include "vk_buffer.h"

#include "vk_command_buffer.h"
#include "vk_descriptors.h"

//NOTE: this should change so that its consistent with the amount given out in by the descriptor pools
#define max_buffers_available 1024u

Buffer_System* buffer_system_init(Renderer* renderer, const u32 frames_in_flight)
{
    Buffer_System* buffer_system = allocator_alloc(&renderer->allocator, sizeof(Buffer_System));
    buffer_system->frames_in_flight = frames_in_flight;
    renderer->buffer_system = buffer_system;


    //these get handed out as handles to other systems that need them
    buffer_system->buffers_size = max_buffers_available;
    buffer_system->buffer_current_count = 0;
    buffer_system->static_buffers = allocator_alloc(&renderer->allocator,
                                                    buffer_system->buffers_size * sizeof(Vulkan_Buffer));

    buffer_system->frame_buffers_memory_usage = 0;
    buffer_system->frame_buffer_current_count = 0;
    buffer_system->frame_buffer_max_count = max_buffers_available;
    buffer_system->per_frame_buffers = allocator_alloc(&renderer->allocator,
                                                       buffer_system->frame_buffer_max_count * sizeof(Vulkan_Buffer));

    buffer_system->frame_staging_buffer_count = renderer->max_frames_in_flight;
    buffer_system->per_frame_staging_buffers = allocator_alloc(&renderer->allocator,
                                                               buffer_system->frames_in_flight
                                                               * sizeof(Vulkan_Buffer));

    buffer_system->global_ubo = allocator_alloc(&renderer->allocator,
                                                buffer_system->frames_in_flight
                                                * sizeof(Vulkan_Buffer));


    //async upload buffer
    u64 upload_staging_buffer_size = MB(128);
    _vulkan_buffer_create_internal_new(renderer,
                                       BUFFER_TYPE_STAGING,
                                       upload_staging_buffer_size,
                                       &buffer_system->upload_staging_buffer);

    buffer_system->upload_staging_free_list = free_list_init(&renderer->allocator, upload_staging_buffer_size,
                                                             1024);
    buffer_system->staging_upload_pending_array = array_create(Vulkan_Staging_Buffer_Pending_Upload, 1024,
                                                               &renderer->allocator);


    //per frame staging buffers
    const u64 staging_buffer_size = MB(256);

    for (u32 i = 0; i < buffer_system->frames_in_flight; i++)
    {
        _vulkan_buffer_create_internal_new(renderer, BUFFER_TYPE_UNIFORM,
                                           sizeof(Global_Ubo), &buffer_system->global_ubo[i]);
        update_uniform_buffer_bindless_descriptor_set_explicit(
            renderer, renderer->descriptor_system, &buffer_system->global_ubo[i], 0);

        //create our global staging buffer
        _vulkan_buffer_create_internal_new(renderer,
                                           BUFFER_TYPE_STAGING,
                                           staging_buffer_size,
                                           &buffer_system->per_frame_staging_buffers[i]);
    }

    return buffer_system;
}

void buffer_system_frame_start(Renderer* renderer, Buffer_System* buffer_system, u32 current_frame)
{
    //rn just clears the staging buffer
    Vulkan_Buffer* current_frame_staging_buffer = &buffer_system->per_frame_staging_buffers[
        current_frame];
    current_frame_staging_buffer->current_offset = 0;

    for (u32 i = 0; i < buffer_system->frame_buffer_current_count; i++)
    {
        buffer_system->per_frame_buffers->current_offset = 0;
    }

    //TODO:
    ARRAY_TYPE(Vulkan_Transfer_Buffer_Pending_Upload)* staging_upload_pending_array;

    u32 array_idx = 0;
    while (array_idx < buffer_system->staging_upload_pending_array->num_items)
    {
        Vulkan_Staging_Buffer_Pending_Upload pending_upload =
            array_get(
                buffer_system->staging_upload_pending_array,
                Vulkan_Staging_Buffer_Pending_Upload,
                array_idx);

        if (timeline_semaphore_query_and_compare(
            renderer,
            vulkan_queue_system_get_transfer_semaphore(renderer),
            pending_upload.semaphore_wait_value))
        {
            free_list_free(buffer_system->upload_staging_free_list, pending_upload.staging_offset, pending_upload.size);

            array_remove_swap(buffer_system->staging_upload_pending_array, array_idx);
            // Don't increment i.
            // The swapped-in element now occupies index i.
            continue;
        }

        array_idx++;
    }

}

void buffer_system_frame_end(Buffer_System* buffer_system, u32 current_frame)
{
}


uint32_t find_memory_type(Renderer* renderer, uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(renderer->physical_device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((type_filter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    FATAL("failed to find suitable memory type");
    return -1;
}

Buffer_Handle vulkan_buffer_create(Renderer* renderer, Buffer_System* buffer_system, Vulkan_Buffer_Type buffer_type,
                                   u64 buffer_size)
{
    MASSERT(buffer_size > 0);

    //we pass out the index which we stared at
    Buffer_Handle out_handle = {buffer_system->buffer_current_count};
    //grab an available buffer
    Vulkan_Buffer* buffer_to_create = &buffer_system->static_buffers[buffer_system->buffer_current_count];
    _vulkan_buffer_create_internal_new(renderer, buffer_type, buffer_size, buffer_to_create);
    buffer_system->buffer_current_count++;
    buffer_system->buffers_size += buffer_size;

    return out_handle;
}

Buffer_Frame_Handle vulkan_buffer_create_frame(Renderer* renderer, Buffer_System* buffer_system,
                                               Vulkan_Buffer_Type buffer_type,
                                               u64 buffer_size)
{
    MASSERT(buffer_size > 0);

    //we pass out the index which we started at
    Buffer_Frame_Handle out_handle = {buffer_system->frame_buffer_current_count};
    for (u32 i = 0; i < renderer->max_frames_in_flight; i++)
    {
        Vulkan_Buffer* buffer_to_create = &buffer_system->per_frame_buffers[buffer_system->
            frame_buffer_current_count + i];
        _vulkan_buffer_create_internal_new(renderer, buffer_type, buffer_size, buffer_to_create);
    }

    buffer_system->frame_buffer_current_count += renderer->max_frames_in_flight;
    buffer_system->frame_buffers_memory_usage += buffer_size * renderer->max_frames_in_flight;

    return out_handle;
}

Vulkan_Buffer* vulkan_buffer_get(Renderer* renderer, Buffer_Handle buffer_handle)
{
    return &renderer->buffer_system->static_buffers[buffer_handle.handle];
}

Vulkan_Buffer* vulkan_buffer_get_frame(Renderer* renderer, Buffer_Frame_Handle frame_buffer_handle)
{
    return &renderer->buffer_system->per_frame_buffers[frame_buffer_handle.handle + renderer->current_frame];
}

void vulkan_buffer_reset(Renderer* renderer, Buffer_Handle buffer_handle)
{
    vulkan_buffer_get(renderer, buffer_handle)->current_offset = 0;
}

void vulkan_buffer_frame_reset(Renderer* renderer, Buffer_Frame_Handle frame_buffer_handle)
{
    vulkan_buffer_get_frame(renderer, frame_buffer_handle)->current_offset = 0;
}

bool vulkan_buffer_frame_staging_upload(Renderer* renderer, Buffer_Frame_Handle buffer_handle,
                                        Vulkan_Command_Buffer* command_buffer, void* data, u64 data_byte_size)
{
    if (data_byte_size <= 0)
    {
        // TRACE("vulkan_buffer_frame_staging_upload: 0 data size passed in")
        return false;
    }

    //get buffer from handle
    Vulkan_Buffer* device_local_buffer = vulkan_buffer_get_frame(renderer, buffer_handle);
    Vulkan_Buffer* staging_buffer = &renderer->buffer_system->per_frame_staging_buffers[renderer->current_frame];


    //make sure its a staging buffer
    MASSERT(staging_buffer->type == BUFFER_TYPE_STAGING);
    MASSERT(device_local_buffer->type != BUFFER_TYPE_STAGING);


    if (staging_buffer->current_offset + data_byte_size >= staging_buffer->capacity)
    {
        WARN("vulkan_buffer_frame_staging_upload: STAGING BUFFER OVERFLOW");
        return false;
    }


    //copy data into the staging buffer
    memcpy(staging_buffer->mapped_data + staging_buffer->current_offset, data, data_byte_size);


    //copy staging buffer data (host visible) into the buffer that for the GPU (device local)

    // Buffer copies have to be submitted to a queue, so we need a command buffer for them
    //Copy all the data in the staging buffer into the device local buffer
    VkBufferCopy copyRegion = {0};
    copyRegion.size = data_byte_size;
    copyRegion.srcOffset = staging_buffer->current_offset;
    copyRegion.dstOffset = device_local_buffer->current_offset;
    vkCmdCopyBuffer(command_buffer->handle, staging_buffer->handle, device_local_buffer->handle, 1, &copyRegion);

    staging_buffer->current_offset += data_byte_size;
    device_local_buffer->current_offset += data_byte_size;

    return true;
}


void vulkan_buffer_staging_copy_range(Vulkan_Command_Buffer* command_buffer,
                                      Vulkan_Buffer* buffer, Vulkan_Buffer* staging_buffer, void* data, u64 data_size,
                                      u64 staging_offset, u64 buffer_offset)
{
    //copy data into the staging buffer
    memcpy(staging_buffer->mapped_data + staging_offset, data, data_size);

    //copy staging buffer data (host visible) into the buffer that for the GPU (device local)

    // Buffer copies have to be submitted to a queue, so we need a command buffer for them
    //Copy all the data in the staging buffer into the device local buffer
    VkBufferCopy copyRegion = {0};
    copyRegion.size = data_size;
    copyRegion.srcOffset = staging_offset;
    copyRegion.dstOffset = buffer_offset;
    vkCmdCopyBuffer(command_buffer->handle, staging_buffer->handle, buffer->handle, 1, &copyRegion);
}


bool vulkan_buffer_upload_data_request(Renderer* renderer, u64 memory_request_size, u64* out_start_offset)
{
    Buffer_System* buffer_system = renderer->buffer_system;
    if (!free_list_alloc(buffer_system->upload_staging_free_list, memory_request_size, out_start_offset))
    {
        INFO("vulkan_buffer_upload_data_request: NOT ENOUGH MEMORY")
        return false;
    }

    return true;
}


bool vulkan_buffer_transfer_upload(Renderer* renderer, Vulkan_Command_Buffer* command_buffer, Buffer_Handle handle,
                                   void* data, u64 data_size, u64 semaphore_value)
{
    u64 staging_buffer_offset = 0;
    if (!vulkan_buffer_upload_data_request(renderer, data_size, &staging_buffer_offset))
    {
        INFO("vulkan_buffer_transfer_upload: not enough memory to satisfy request");
        return false;
    }

    Vulkan_Buffer* staging = &renderer->buffer_system->upload_staging_buffer;
    Vulkan_Buffer* gpu = vulkan_buffer_get(renderer, handle);

    vulkan_buffer_staging_copy_range(command_buffer, gpu,
                                     staging, data, data_size, staging_buffer_offset,
                                     gpu->current_offset);

    //TODO: temp code
    gpu->current_offset += data_size;

    Vulkan_Staging_Buffer_Pending_Upload upload = {
        .staging_offset = staging_buffer_offset,
        .size = data_size,
        .semaphore_wait_value = semaphore_value,
    };

    array_push(renderer->buffer_system->staging_upload_pending_array, &upload);

    return true;
}

bool vulkan_buffer_startup_uploads(Renderer* renderer, Buffer_Handle buffer_handle,
                                   void* data, u64 data_byte_size)
{
    Vulkan_Command_Buffer temp_cb;
    vulkan_command_buffer_begin_single_use(renderer, renderer->graphics_command_pool, &temp_cb);

    Vulkan_Buffer* buffer = vulkan_buffer_get(renderer, buffer_handle);

    Vulkan_Buffer temp_staging_buffer;
    _vulkan_buffer_create_internal_new(renderer, BUFFER_TYPE_STAGING, data_byte_size, &temp_staging_buffer);


    //copy data into the staging buffer
    memcpy(temp_staging_buffer.mapped_data + temp_staging_buffer.current_offset, data, data_byte_size);

    VkBufferCopy copyRegion = {0};
    copyRegion.size = data_byte_size;
    copyRegion.srcOffset = temp_staging_buffer.current_offset;
    copyRegion.dstOffset = buffer->current_offset;
    vkCmdCopyBuffer(temp_cb.handle, temp_staging_buffer.handle, buffer->handle, 1, &copyRegion);


    VkBufferMemoryBarrier2 barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;

    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

    barrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.buffer = buffer->handle;
    barrier.offset = buffer->current_offset;
    barrier.size = data_byte_size;

    VkDependencyInfo pDependencyInfo = {0};
    pDependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    pDependencyInfo.bufferMemoryBarrierCount = 1;
    pDependencyInfo.pBufferMemoryBarriers = &barrier;
    vkCmdPipelineBarrier2(temp_cb.handle, &pDependencyInfo);


    vulkan_command_buffer_end_single_use(renderer, renderer->graphics_command_pool, &temp_cb, renderer->graphics_queue);

    _vulkan_buffer_destroy_internal(renderer, &temp_staging_buffer);

    return true;
}


bool vulkan_buffer_free(Renderer* renderer, Vulkan_Buffer* vk_buffer)
{
    vkFreeMemory(renderer->logical_device, vk_buffer->memory, NULL);
    vkDestroyBuffer(renderer->logical_device, vk_buffer->handle, NULL);
    return true;
}

void _vulkan_buffer_create_internal_new(Renderer* renderer, Vulkan_Buffer_Type buffer_type, u64 buffer_size,
                                        Vulkan_Buffer* buffer_to_create)
{
    //do a large allocation upfront
    buffer_to_create->capacity = buffer_size;
    buffer_to_create->current_offset = 0;
    buffer_to_create->type = buffer_type;

    VkDevice device = renderer->logical_device;


    //for Buffer device addressing
    VkMemoryAllocateFlagsInfo memory_allocate_flags_info = {0};
    memory_allocate_flags_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
    memory_allocate_flags_info.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.pNext = &memory_allocate_flags_info;

    VkBufferCreateInfo out_buffer_create_info = {0};
    out_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    out_buffer_create_info.size = buffer_to_create->capacity;
    out_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // generally what you want


    // intended to be used as the destination of a copy from a staging buffer
    switch (buffer_type)
    {
    case BUFFER_TYPE_VERTEX:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case BUFFER_TYPE_INDEX:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        break;
    case BUFFER_TYPE_STORAGE_GPU:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case BUFFER_TYPE_INDIRECT:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case BUFFER_TYPE_STORAGE_CPU:
        /*might need a transfer source maybe??? but not really since we can just write into it*/
        out_buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case BUFFER_TYPE_STAGING:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        memory_allocate_info.pNext = NULL;
        break;
    case BUFFER_TYPE_UNIFORM:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    default:
        MASSERT(false)
        break;
    }


    // Create a host-visible buffer to copy the vertex data to (staging buffer)
    VkResult buffer_result = vkCreateBuffer(device, &out_buffer_create_info, renderer->vk_allocator_callback,
                                            &buffer_to_create->handle);
    VK_CHECK(buffer_result)
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device, buffer_to_create->handle, &memReqs);
    memory_allocate_info.allocationSize = memReqs.size;

    //cpu means that a staging buffer is needed to send data to the gpu, typically for large data sets
    //gpu means that no staging buffer is needed and can be  updated direct and will be mapped as well

    VkMemoryPropertyFlags gpu_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkMemoryPropertyFlags cpu_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkMemoryPropertyFlags mem_properties;
    if (buffer_type == BUFFER_TYPE_STORAGE_CPU || buffer_type == BUFFER_TYPE_STAGING || buffer_type ==
        BUFFER_TYPE_UNIFORM)
    {
        mem_properties = cpu_properties;
    }
    else
    {
        mem_properties = gpu_properties;
    }
    memory_allocate_info.memoryTypeIndex = find_memory_type(renderer, memReqs.memoryTypeBits,
                                                            mem_properties);

    VK_CHECK(
        vkAllocateMemory(device, &memory_allocate_info, renderer->vk_allocator_callback, &buffer_to_create->memory));
    VK_CHECK(vkBindBufferMemory(device, buffer_to_create->handle, buffer_to_create->memory, 0));

    //host visible should be mapped to a specific region of memory
    if (buffer_type == BUFFER_TYPE_STORAGE_CPU | buffer_type == BUFFER_TYPE_STAGING | buffer_type ==
        BUFFER_TYPE_UNIFORM)
    {
        VK_CHECK(
            vkMapMemory(device, buffer_to_create->memory, 0, memory_allocate_info.allocationSize, 0, (void**)&
                buffer_to_create-> mapped_data));
    }
}

void _vulkan_buffer_destroy_internal(Renderer* renderer, Vulkan_Buffer* buffer)
{
    vkDestroyBuffer(renderer->logical_device, buffer->handle, NULL);
    vkFreeMemory(renderer->logical_device, buffer->memory, NULL);
}


VkDeviceAddress get_buffer_device_address(VkDevice device, VkBuffer buffer)
{
    VkBufferDeviceAddressInfo info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .buffer = buffer
    };
    return vkGetBufferDeviceAddress(device, &info);
}


VkDeviceAddress vulkan_buffer_get_device_address(Renderer* renderer, Buffer_Handle buffer_handle)
{
    return get_buffer_device_address(renderer->logical_device,
                                     vulkan_buffer_get(renderer, buffer_handle)->handle);
}

VkDeviceAddress vulkan_buffer_get_frame_device_address(Renderer* renderer, Buffer_Frame_Handle buffer_handle)
{
    return get_buffer_device_address(renderer->logical_device,
                                     vulkan_buffer_get_frame(renderer, buffer_handle)->handle);
}
