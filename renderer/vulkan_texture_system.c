#include "vulkan_texture_system.h"

#include "vk_command_buffer.h"
#include "vk_image.h"
#include "vk_descriptors.h"


//first set of values reserved by vulkan for things like internal textures, and renderpass textures


//TODO: pass in texture config amount
Vulkan_Texture_System* vulkan_texture_system_init(Renderer* renderer)
{
    Vulkan_Texture_System* texture_system = allocator_alloc(&renderer->allocator, sizeof(Vulkan_Texture_System));
    renderer->texture_system = texture_system;


    texture_system->available_texture_indexes = 0;
    // texture_system->texture_file_to_handle = HASH_TABLE_CREATE(Texture_Handle, AVAILABLE_TEXTURES * 2);
    memset(texture_system->textures, 0, sizeof(Vulkan_Texture) * MAX_TEXTURE_COUNT);
    texture_system->texture_deletion_queue = ring_queue_create(sizeof(Vulkan_Texture), MAX_TEXTURE_COUNT);

    // memset(texture_system->renderer_textures, 0, sizeof(Vulkan_Texture) * VULKAN_INTERNAL_RENDERER_TEXTURES_COUNT);
    texture_system->renderer_texture_indexes = 0;
    texture_system->renderer_texture_indexes_max = VULKAN_INTERNAL_RENDERER_TEXTURES_COUNT;

    //we start every index off at 0
    memset(texture_system->texture_table, 0, MAX_TEXTURE_COUNT * sizeof(u32));

    //OPTIMIZE: definetly dont need this much memory for this
    texture_system->texture_pending_array = array_create(Vulkan_Texture_Pending_Upload, MAX_TEXTURE_COUNT,
                                                         &renderer->allocator);

    /*//TODO: we really should create a default texture here at slot 0, or just have a seperate queue appended to at startup for all resources needed
    Vulkan_Command_Buffer command_buffer;
    vulkan_command_buffer_begin_single_use(renderer, renderer->graphics_command_pool, &command_buffer);


    vulkan_command_buffer_end_single_use(renderer, renderer->graphics_command_pool, &command_buffer,
                                         renderer->graphics_queue);*/


    texture_system->texture_table_buffer = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                      BUFFER_TYPE_STORAGE_GPU,
                                                                      sizeof(u32) * MAX_TEXTURE_COUNT);
    texture_system->texture_table_max_size = MAX_TEXTURE_COUNT;


    return texture_system;
}


void vulkan_texture_update_async_resources(Renderer* renderer, Render_Packet* packet,
                                           Vulkan_Command_Buffer* command_buffer)
{
    Vulkan_Texture_System* texture_system = renderer->texture_system;

    //TODO: we want to wait the same number of frames in flight before deleting the texture, meaning we need to increment every value
    // you could also do a semaphore signal, based on which frame we are on, and use that, but that seems messy

    //free any textures not in use
    Vulkan_Texture* texture;
    while (!ring_queue_is_empty(texture_system->texture_deletion_queue))
    {
        ring_dequeue(texture_system->texture_deletion_queue, &texture);

        MASSERT(texture);

        vulkan_texture_free(renderer, texture);
    }


    //textures gpu upload we are waiting on, checked once a frame
    u64 i = 0;

    //OPTIMIZE: since textures uploaded in the same frame have the same signal semaphore,
    // it doesn't make sense to check each texture individual if its done, we can just check the batch
    while (i < texture_system->texture_pending_array->num_items)
    {
        Vulkan_Texture_Pending_Upload pending_upload =
            array_get(
                texture_system->texture_pending_array,
                i,
                Vulkan_Texture_Pending_Upload);

        if (timeline_semaphore_query_and_compare(
            renderer,
            vulkan_queue_system_get_transfer_semaphore(renderer),
            pending_upload.timeline_semaphore_value))
        {
            vulkan_command_buffer_reset(pending_upload.command_buffer);


            array_remove_swap(texture_system->texture_pending_array, i);

            texture_system->texture_table[pending_upload.bindless_index] = pending_upload.bindless_index;

            //TODO: flip off if not using imgui
            //create a descriptor for imgui if using it
            pending_upload.vulkan_texture->ready_for_read_by_imgui = true;
            pending_upload.vulkan_texture->imgui_descriptor_set = cImGui_ImplVulkan_AddTexture(
                pending_upload.vulkan_texture->texture_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            // Don't increment i.
            // The swapped-in element now occupies index i.
            continue;
        }

        i++;
    }

    //for now were just going to upload the texture table here every frame,
    //but realistically we want to batch the upload


    vulkan_command_buffer_debug_label_begin(renderer, command_buffer, "texture table");

    //texture table
    if (vulkan_buffer_frame_staging_upload(
        renderer, texture_system->texture_table_buffer,
        command_buffer, texture_system->texture_table, sizeof(u32) * MAX_TEXTURE_COUNT))
    {
        Vulkan_Buffer* transform_buffer = vulkan_buffer_get_frame(renderer, texture_system->texture_table_buffer);
        VkBufferMemoryBarrier2 texture_table_buffer_barrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,

            //barrier so that we finish our transfer before our shaders read the data
            .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

            .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,

            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

            .buffer = transform_buffer->handle,
            .offset = 0,
            .size = transform_buffer->current_offset,
        };
        vulkan_command_add_buffer_barrier(command_buffer, texture_table_buffer_barrier);
    }

    vulkan_command_flush_barriers(command_buffer);

    vulkan_command_buffer_debug_label_end(renderer, command_buffer);
}

void vulkan_texture_system_upload_new_textures(Renderer* renderer, Render_Packet* packet)
{
    Vulkan_Texture_System* texture_system = renderer->texture_system;


    if (ring_queue_is_empty(packet->texture_upload_queue))
    {
        return;
    }

    Vulkan_Command_Buffer* transfer_command_buffer = NULL;
    u64 semaphore_signal_value = 0;
    if (!vulkan_queue_system_get_transfer_command_buffer(renderer, &transfer_command_buffer, &semaphore_signal_value))
    {
        return;
    }
    Vulkan_Command_Buffer* graphics_command_buffer = NULL;
    vulkan_queue_system_get_graphics_command_buffer(renderer, &graphics_command_buffer);


    Texture_GPU_Upload texture_upload;

    //NOTE: each texture needs its own image memory barrier
    vulkan_command_buffer_debug_label_begin(renderer, transfer_command_buffer, "texture upload");
    while (!ring_queue_is_empty(packet->texture_upload_queue) && transfer_command_buffer)
    {
        ring_dequeue(packet->texture_upload_queue, &texture_upload);

        Vulkan_Texture* vulkan_texture = &renderer->texture_system->textures[texture_upload.texture_index];
        vulkan_texture->bindless_index = texture_upload.texture_index;

        /*//TODO: might not want to use this, and instead just recycle the buffers, made just for texture uploads
        Vulkan_Command_Buffer* single_use_cb = allocator_heap_alloc(renderer->heap_allocator,
                                                                    sizeof(Vulkan_Command_Buffer));*/


        //TODO: this allocates memory
        //create the texture
        vulkan_texture_create_image_new(renderer, &texture_upload, vulkan_texture);
        //update the heap
        update_texture_bindless_descriptor_set(renderer, renderer->descriptor_system,
                                               texture_upload.texture_index);

        initial_image_layout_transition(transfer_command_buffer, vulkan_texture->texture_image);

        //from here we need to get the texture into gpu memory using a staging buffer,
        //and if we have a dedicated transfer queue, the second image layout will need to create two memory barriers

        //create a staging buffer
        //TODO: Clean this up and dont allocate on the spot, use the upload staging buffer
        Vulkan_Buffer temp_staging = {0};
        _vulkan_buffer_create_internal_new(renderer, BUFFER_TYPE_STAGING, vulkan_texture->image_size, &temp_staging);
        memcpy(
            temp_staging.mapped_data,
            texture_upload.pixel_data,
            vulkan_texture->image_size);
        buffer_to_image_copy_new(transfer_command_buffer, &temp_staging, vulkan_texture->texture_image,
                                 texture_upload.madness_texture->width, texture_upload.madness_texture->height);

        second_image_layout_transition(renderer,
                                       transfer_command_buffer,
                                       graphics_command_buffer,
                                       vulkan_texture->texture_image,
                                       VULKAN_QUEUE_TYPE_TRANSFER, VULKAN_QUEUE_TYPE_GRAPHICS);


        //unload texture data - safe to do so here, but we cant use it yet until the upload is complete
        allocator_heap_free(texture_upload.texture_memory_allocator, texture_upload.pixel_data);

        //set the texture to point to the default location until the actual texture is loaded
        texture_system->texture_table[texture_upload.texture_index] = 0;

        Vulkan_Texture_Pending_Upload new_pending_upload = {
            .madness_texture = texture_upload.madness_texture,
            .vulkan_texture = vulkan_texture,
            .command_buffer = transfer_command_buffer,
            .timeline_semaphore_value = semaphore_signal_value,
            .bindless_index = texture_upload.texture_index,
        };
        array_push(texture_system->texture_pending_array, &new_pending_upload);
    }
    vulkan_command_buffer_debug_label_end(renderer, transfer_command_buffer);


    //NOTE: for queue family transfers unless VK_KHR_maintence8 is on,
    // all command bits needs to be used for both queues
    // also we do it at queue frame end

    /*//submit info
    VkSemaphoreSubmitInfo signal_semaphore_info = {0};
    signal_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signal_semaphore_info.pNext = 0;
    // pSignalSemaphoreInfos.deviceIndex;
    signal_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT | VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    //value assumed to be accurate for the current texture upload
    signal_semaphore_info.value = semaphore_signal_value;
    signal_semaphore_info.semaphore = vulkan_queue_system_get_transfer_semaphore(renderer);

    //only need this is we do a queue ownership transfer
    VkSemaphoreSubmitInfo wait_semaphore_info = {0};
    wait_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_semaphore_info.pNext = 0;
    wait_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    wait_semaphore_info.value = semaphore_signal_value;
    wait_semaphore_info.semaphore = vulkan_queue_system_get_transfer_semaphore(renderer);

    //TODO: handle the case if transfer and graphics are not the same queue
    vulkan_queue_add_signal_semaphore(renderer, VULKAN_QUEUE_TYPE_TRANSFER, signal_semaphore_info);
    vulkan_queue_add_wait_semaphore(renderer, VULKAN_QUEUE_TYPE_GRAPHICS, wait_semaphore_info);*/
}


Vulkan_Texture* vulkan_texture_system_get_vulkan_texture(Vulkan_Texture_System* system, u32 bindless_index)
{
    return &system->textures[bindless_index];
}


bool vulkan_texture_system_free(Renderer* renderer, Texture_Handle handle)
{
    Vulkan_Texture* vulkan_texture = &renderer->texture_system->textures[handle.handle];

    //queue the texture to be unloaded
    ring_enqueue(renderer->texture_system->texture_deletion_queue, vulkan_texture);


    return true;
}

void vulkan_texture_system_internal(Renderer* renderer, u32 width, u32 height, VkFormat format,
                                    Vulkan_Texture* out_texture)
{
    Vulkan_Texture_System* texture_system = renderer->texture_system;

    if (texture_system->renderer_texture_indexes <= VULKAN_INTERNAL_RENDERER_TEXTURES_COUNT)
    {
        //if this ever happens bump up the texture count
        MASSERT_FALSE()
    }

    out_texture->bindless_index = texture_system->renderer_texture_indexes++;

    out_texture->width = width;
    out_texture->height = width;

    // We will sample directly from the depth attachment for the shadow mapping
    VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    // VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT // might need
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    /*
            VK_IMAGE_TILING_LINEAR: Texels are laid out in row-major order like our pixels array
            VK_IMAGE_TILING_OPTIMAL: Texels are laid out in an implementation defined order for optimal access
        Unlike the layout of an image, the tiling mode cannot be changed at a later time.
        If you want to be able to directly access texels in the memory of the image,
        then you must use VK_IMAGE_TILING_LINEAR. We will be using a staging buffer instead of a staging image,
        so this won't be necessary. We will be using VK_IMAGE_TILING_OPTIMAL for efficient access from the shader.
     */

    //shadows use a depth bit
    VkImageAspectFlags view_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
    /*
    *  VK_IMAGE_ASPECT_COLOR_BIT = 0x00000001,
    VK_IMAGE_ASPECT_DEPTH_BIT = 0x00000002,
    VK_IMAGE_ASPECT_STENCIL_BIT = 0x00000004,
     */

    // Creation info.
    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D; // might need to be an image in the future
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.extent.depth = 1; // TODO: Support configurable depth.
    image_create_info.mipLevels = 1; // TODO: Support mip mapping
    image_create_info.arrayLayers = 1; // TODO: Support number of layers in the image.
    image_create_info.format = format;
    image_create_info.tiling = tiling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = usage;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Configurable sample count.
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode.

    VkResult image_create_result = vkCreateImage(renderer->logical_device, &image_create_info,
                                                 renderer->vk_allocator_callback,
                                                 &out_texture->texture_image);
    VK_CHECK(image_create_result);


    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(renderer->logical_device, out_texture->texture_image, &memory_requirements);

    // s32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    s32 memory_type = find_memory_type(renderer, memory_requirements.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memory_type == -1)
    {
        M_ERROR("Required memory type not found. Image not valid.");
    }


    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VK_CHECK(
        vkAllocateMemory(renderer->logical_device, &memory_allocate_info, renderer->vk_allocator_callback, &out_texture
            ->
            texture_image_memory
        ));

    // Bind the memory
    VK_CHECK(
        vkBindImageMemory(renderer->logical_device, out_texture->texture_image, out_texture->texture_image_memory,
            0));
    // TODO: configurable memory offset.

    // Create view
    out_texture->texture_image_view = 0;
    vulkan_image_view_create(renderer, format, view_aspect_flags, out_texture);

    VkFilter shadowmap_filter = formatIsFilterable(renderer->physical_device, format, VK_IMAGE_TILING_OPTIMAL)
                                    ? VK_FILTER_LINEAR
                                    : VK_FILTER_NEAREST;

    VkSamplerCreateInfo sampler_info = {0};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = shadowmap_filter;
    sampler_info.minFilter = shadowmap_filter;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = renderer->properties2.properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE; // not in use
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 1.0f; // has to be set

    VK_CHECK(vkCreateSampler(renderer->logical_device, &sampler_info, 0, &out_texture->texture_sampler));


    update_texture_bindless_descriptor_set_explicit(renderer, renderer->descriptor_system,
                                                    out_texture->texture_image_view, out_texture->texture_sampler,
                                                    out_texture->bindless_index,
                                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    //
    vkDeviceWaitIdle(renderer->logical_device);
}


void vulkan_texture_create_shadowmap(Renderer* renderer, u32 width, u32 height,
                                     VkFormat format, Vulkan_Texture* vk_texture)
{
    Vulkan_Texture_System* texture_system = renderer->texture_system;

    if (texture_system->renderer_texture_indexes >= VULKAN_INTERNAL_RENDERER_TEXTURES_COUNT)
    {
        //if this ever happens bump up the texture count
        MASSERT_FALSE()
    }

    vk_texture->bindless_index = MAX_TEXTURE_COUNT + texture_system->renderer_texture_indexes++;


    vk_texture->width = width;
    vk_texture->height = height;

    // We will sample directly from the depth attachment for the shadow mapping
    VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    // VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT // might need
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    /*
            VK_IMAGE_TILING_LINEAR: Texels are laid out in row-major order like our pixels array
            VK_IMAGE_TILING_OPTIMAL: Texels are laid out in an implementation defined order for optimal access
        Unlike the layout of an image, the tiling mode cannot be changed at a later time.
        If you want to be able to directly access texels in the memory of the image,
        then you must use VK_IMAGE_TILING_LINEAR. We will be using a staging buffer instead of a staging image,
        so this won't be necessary. We will be using VK_IMAGE_TILING_OPTIMAL for efficient access from the shader.
     */

    //shadows use a depth bit
    VkImageAspectFlags view_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
    /*
    *  VK_IMAGE_ASPECT_COLOR_BIT = 0x00000001,
    VK_IMAGE_ASPECT_DEPTH_BIT = 0x00000002,
    VK_IMAGE_ASPECT_STENCIL_BIT = 0x00000004,
     */

    // Creation info.
    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D; // might need to be an image in the future
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.extent.depth = 1; // TODO: Support configurable depth.
    image_create_info.mipLevels = 1; // TODO: Support mip mapping
    image_create_info.arrayLayers = 1; // TODO: Support number of layers in the image.
    image_create_info.format = format;
    image_create_info.tiling = tiling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = usage;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Configurable sample count.
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode.

    VkResult image_create_result = vkCreateImage(renderer->logical_device, &image_create_info,
                                                 renderer->vk_allocator_callback,
                                                 &vk_texture->texture_image);
    VK_CHECK(image_create_result);


    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(renderer->logical_device, vk_texture->texture_image, &memory_requirements);

    // s32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    s32 memory_type = find_memory_type(renderer, memory_requirements.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memory_type == -1)
    {
        M_ERROR("Required memory type not found. Image not valid.");
    }


    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VK_CHECK(
        vkAllocateMemory(renderer->logical_device, &memory_allocate_info, renderer->vk_allocator_callback, &vk_texture->
            texture_image_memory
        ));

    // Bind the memory
    VK_CHECK(
        vkBindImageMemory(renderer->logical_device, vk_texture->texture_image, vk_texture->texture_image_memory,
            0));
    // TODO: configurable memory offset.

    // Create view
    vk_texture->texture_image_view = 0;
    vulkan_image_view_create(renderer, format, view_aspect_flags, vk_texture);

    VkFilter shadowmap_filter = formatIsFilterable(renderer->physical_device, format, VK_IMAGE_TILING_OPTIMAL)
                                    ? VK_FILTER_LINEAR
                                    : VK_FILTER_NEAREST;

    VkSamplerCreateInfo sampler_info = {0};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = shadowmap_filter;
    sampler_info.minFilter = shadowmap_filter;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    //NOTE: for shadows we clamp to border and not edge
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler_info.anisotropyEnable = VK_FALSE; // not needed for shadow mapping
    sampler_info.maxAnisotropy = renderer->properties2.properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; // float white
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE; // not in use
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f; // has to be set

    VK_CHECK(vkCreateSampler(renderer->logical_device, &sampler_info, 0, &vk_texture->texture_sampler));


    update_texture_bindless_descriptor_set_explicit(renderer, renderer->descriptor_system,
                                                    vk_texture->texture_image_view, vk_texture->texture_sampler,
                                                    vk_texture->bindless_index,
                                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    //
    vkDeviceWaitIdle(renderer->logical_device);

    //wont work until imgui is initialized
    vk_texture->ready_for_read_by_imgui = true;
    vk_texture->imgui_descriptor_set = cImGui_ImplVulkan_AddTexture(
        // vk_texture->texture_image_view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
        // vk_texture->texture_image_view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
        vk_texture->texture_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}


void vulkan_texture_create_shadowmap_point_light(Renderer* renderer, u32 extent,
                                                 VkFormat format, Vulkan_Texture_Cubemap* vk_texture)
{
    Vulkan_Texture_System* texture_system = renderer->texture_system;

    if (texture_system->renderer_texture_indexes >= VULKAN_INTERNAL_RENDERER_TEXTURES_COUNT)
    {
        //if this ever happens bump up the texture count
        MASSERT_FALSE()
    }

    vk_texture->cubemap_bindless_index = MAX_TEXTURE_COUNT + texture_system->renderer_texture_indexes++;


    vk_texture->width = extent;
    vk_texture->height = extent;

    // We will sample directly from the depth attachment for the shadow mapping
    VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    // VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT // might need
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    /*
            VK_IMAGE_TILING_LINEAR: Texels are laid out in row-major order like our pixels array
            VK_IMAGE_TILING_OPTIMAL: Texels are laid out in an implementation defined order for optimal access
        Unlike the layout of an image, the tiling mode cannot be changed at a later time.
        If you want to be able to directly access texels in the memory of the image,
        then you must use VK_IMAGE_TILING_LINEAR. We will be using a staging buffer instead of a staging image,
        so this won't be necessary. We will be using VK_IMAGE_TILING_OPTIMAL for efficient access from the shader.
     */

    //shadows use a depth bit
    VkImageAspectFlags view_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
    /*
    *  VK_IMAGE_ASPECT_COLOR_BIT = 0x00000001,
    VK_IMAGE_ASPECT_DEPTH_BIT = 0x00000002,
    VK_IMAGE_ASPECT_STENCIL_BIT = 0x00000004,
     */

    // Creation info.
    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D; // stays 2D
    image_create_info.extent.width = extent;
    image_create_info.extent.height = extent;
    image_create_info.extent.depth = 1; // TODO: Support configurable depth.
    image_create_info.mipLevels = 1; // TODO: Support mip mapping
    image_create_info.arrayLayers = 6; // NOTE: since its a cube map, needs 6 layers.
    image_create_info.format = format;
    image_create_info.tiling = tiling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = usage;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Configurable sample count.
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode.
    image_create_info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT; //NOTE: needed for cubemaps

    VkResult image_create_result = vkCreateImage(renderer->logical_device, &image_create_info,
                                                 renderer->vk_allocator_callback,
                                                 &vk_texture->texture_image);
    VK_CHECK(image_create_result);


    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(renderer->logical_device, vk_texture->texture_image, &memory_requirements);

    // s32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    s32 memory_type = find_memory_type(renderer, memory_requirements.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memory_type == -1)
    {
        M_ERROR("Required memory type not found. Image not valid.");
    }


    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VK_CHECK(
        vkAllocateMemory(renderer->logical_device, &memory_allocate_info, renderer->vk_allocator_callback, &vk_texture->
            texture_image_memory
        ));

    // Bind the memory
    VK_CHECK(
        vkBindImageMemory(renderer->logical_device, vk_texture->texture_image, vk_texture->texture_image_memory,
            0));
    // TODO: configurable memory offset.

    // Create view
    vulkan_image_view_create_cube(renderer, format, view_aspect_flags, vk_texture, &vk_texture->cubemap_image_view);

    for (u32 face_index = 0; face_index < 6; face_index++)
    {
        vulkan_image_view_create_cube_map_face(renderer, face_index, format, view_aspect_flags, &vk_texture->texture_image,
                                     &vk_texture->texture_face_image_view[face_index]);
    }


    VkFilter shadowmap_filter = formatIsFilterable(renderer->physical_device, format, VK_IMAGE_TILING_OPTIMAL)
                                    ? VK_FILTER_LINEAR
                                    : VK_FILTER_NEAREST;

    VkSamplerCreateInfo sampler_info = {0};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = shadowmap_filter;
    sampler_info.minFilter = shadowmap_filter;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    //NOTE: for shadows we clamp to border and not edge
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    sampler_info.anisotropyEnable = VK_FALSE; // not needed for shadow mapping
    sampler_info.maxAnisotropy = renderer->properties2.properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE; // float white
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE; // not in use
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f; // has to be set

    VK_CHECK(vkCreateSampler(renderer->logical_device, &sampler_info, 0, &vk_texture->texture_sampler));


    update_texture_bindless_descriptor_set_explicit(renderer, renderer->descriptor_system,
                                                    vk_texture->cubemap_image_view, vk_texture->texture_sampler,
                                                    vk_texture->cubemap_bindless_index,
                                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    //
    vkDeviceWaitIdle(renderer->logical_device);

    //wont work until imgui is initialized
    vk_texture->ready_for_read_by_imgui = true;
    for (u32 face_index = 0; face_index < 6; face_index++)
    {
        vk_texture->imgui_descriptor_set[face_index] = cImGui_ImplVulkan_AddTexture(
            // vk_texture->texture_image_view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
            // vk_texture->texture_image_view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
            vk_texture->texture_face_image_view[face_index], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}
