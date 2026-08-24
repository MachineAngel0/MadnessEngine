#include "vulkan_texture_system.h"

#include "vk_command_buffer.h"
#include "vk_image.h"
#include "vk_descriptors.h"


//TODO: pass in texture config amount
Vulkan_Texture_System* vulkan_texture_system_init(Renderer* renderer)
{
    Vulkan_Texture_System* texture_system = allocator_alloc(&renderer->allocator, sizeof(Vulkan_Texture_System));
    renderer->texture_system = texture_system;


    texture_system->available_texture_indexes = 0;
    // texture_system->texture_file_to_handle = HASH_TABLE_CREATE(Texture_Handle, AVAILABLE_TEXTURES * 2);
    memset(texture_system->textures, 0, sizeof(Vulkan_Texture) * MAX_TEXTURE_COUNT);
    texture_system->texture_deletion_queue = ring_queue_create(sizeof(Vulkan_Texture), MAX_TEXTURE_COUNT);


    //OPTIMIZE: definetly dont need this much memory for this
    texture_system->texture_pending_array = array_create(Vulkan_Texture_Pending_Upload, MAX_TEXTURE_COUNT,
                                                         &renderer->allocator);

    //TODO: we really should create a default texture here at slot 0
    Vulkan_Command_Buffer command_buffer;
    vulkan_command_buffer_begin_single_use(renderer, renderer->graphics_command_pool, &command_buffer);


    vulkan_command_buffer_end_single_use(renderer, renderer->graphics_command_pool, &command_buffer,
                                         renderer->graphics_queue);


    return texture_system;
}

void vulkan_texture_system_update(Renderer* renderer, Render_Packet* packet)
{
    Vulkan_Texture_System* texture_system = renderer->texture_system;


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
                Vulkan_Texture_Pending_Upload,
                i);

        if (timeline_semaphore_query_and_compare(
            renderer,
            vulkan_queue_system_get_transfer_semaphore(renderer),
            pending_upload.timeline_semaphore_value))
        {
            pending_upload.madness_texture->texture_load_state = ASSET_LOAD_STATE_LOADED;

            vulkan_command_buffer_reset(pending_upload.command_buffer);

            array_remove_swap(texture_system->texture_pending_array, i);

            // Don't increment i.
            // The swapped-in element now occupies index i.
            continue;
        }

        i++;
    }


    if (ring_queue_is_empty(packet->texture_upload_queue))
    {
        return;
    }

    Vulkan_Command_Buffer* transfer_command_buffer = NULL;
    if (!vulkan_queue_system_get_transfer_command_buffer(renderer, &transfer_command_buffer))
    {
        return;
    }
    Vulkan_Command_Buffer* graphics_command_buffer = NULL;
    vulkan_queue_system_get_graphics_command_buffer(renderer, &graphics_command_buffer);

    u64 semaphore_signal_value = 0;
    vulkan_queue_system_get_transfer_semaphore_value(renderer, &semaphore_signal_value);

    Texture_GPU_Upload texture_upload;

    //NOTE: each texture needs its own image memory barrier
    vulkan_command_buffer_debug_label_begin(renderer, transfer_command_buffer, "texture upload");
    while (!ring_queue_is_empty(packet->texture_upload_queue) && transfer_command_buffer)
    {
        ring_dequeue(packet->texture_upload_queue, &texture_upload);

        Vulkan_Texture* vulkan_texture = &renderer->texture_system->textures[texture_upload.texture_index];

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

        Vulkan_Texture_Pending_Upload new_pending_upload = {
            .madness_texture = texture_upload.madness_texture,
            .vulkan_texture = vulkan_texture,
            .command_buffer = transfer_command_buffer,
            .timeline_semaphore_value = semaphore_signal_value,
            .bindless_index = texture_upload.texture_index
        };
        array_push(texture_system->texture_pending_array, &new_pending_upload);
    }
    vulkan_command_buffer_debug_label_end(renderer, transfer_command_buffer);


    //NOTE: for queue family transfers unless VK_KHR_maintence8 is on,
    // all command bits needs to be used for both queues

    //submit info
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
    vulkan_queue_add_wait_semaphore(renderer, VULKAN_QUEUE_TYPE_GRAPHICS, wait_semaphore_info);
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
