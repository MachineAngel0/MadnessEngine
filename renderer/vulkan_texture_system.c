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
    texture_system->texture_file_to_handle = HASH_TABLE_CREATE(Texture_Handle, AVAILABLE_TEXTURES * 2);
    memset(texture_system->textures, 0, sizeof(Vulkan_Texture) * MAX_TEXTURE_COUNT);
    texture_system->texture_deletion_queue = ring_queue_create(sizeof(Vulkan_Texture), MAX_TEXTURE_COUNT);


    timeline_semaphore_create(renderer, &texture_system->timline_texture_upload_semaphore);
    //OPTIMIZE: definetly dont need this much memory for this
    texture_system->texture_pending_array = array_create(Vulkan_Texture_Pending_Upload, MAX_TEXTURE_COUNT,
                                                         &renderer->allocator);

    //TODO: we really should create a default texture here at slot 0


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

        vulkan_texture_free(&renderer->context, texture);
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
            &texture_system->timline_texture_upload_semaphore,
            pending_upload.timeline_semaphore_value))
        {
            pending_upload.madness_texture->bindless_slot_query =
                pending_upload.madness_texture->bindless_slot;

            vulkan_command_buffer_reset(pending_upload.command_buffer);

            array_remove_swap(
                texture_system->texture_pending_array,
                i);

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
    vulkan_command_buffer_system_get_and_begin_cb(renderer->command_buffer_system,
                                                  // VULKAN_COMMAND_BUFFER_QUEUE_TYPE_TRANSFER, &transfer_command_buffer);
                                                  VULKAN_QUEUE_TYPE_GRAPHICS, &transfer_command_buffer);

    u64 semaphore_signal_value = ++texture_system->timeline_semaphore_texture_value;
    //were not handling this correctly, since the rest of this needs to know if the command buffer is valid

    Texture_GPU_Upload texture_upload;

    while (!ring_queue_is_empty(packet->texture_upload_queue) && transfer_command_buffer)
    {
        ring_dequeue(packet->texture_upload_queue, &texture_upload);

        Vulkan_Texture* vulkan_texture = &renderer->texture_system->textures[texture_upload.madness_texture->
            bindless_slot];

        /*//TODO: might not want to use this, and instead just recycle the buffers, made just for texture uploads
        Vulkan_Command_Buffer* single_use_cb = allocator_heap_alloc(renderer->heap_allocator,
                                                                    sizeof(Vulkan_Command_Buffer));*/


        //create the texture
        vulkan_texture_create_image_new(renderer, &renderer->context, &texture_upload, vulkan_texture,
                                        transfer_command_buffer);
        update_texture_bindless_descriptor_set(renderer, renderer->descriptor_system,
                                               texture_upload.madness_texture->bindless_slot);




        //unload texture data - safe to do so here, but we cant use it yet until the upload is complete
        allocator_heap_free(texture_upload.texture_memory_allocator, texture_upload.pixel_data);

        Vulkan_Texture_Pending_Upload new_pending_upload = {
            .madness_texture = texture_upload.madness_texture,
            .texture = vulkan_texture,
            .command_buffer = transfer_command_buffer,
            .timeline_semaphore_value = semaphore_signal_value,
        };
        array_push(texture_system->texture_pending_array, &new_pending_upload);
    }

    vulkan_command_buffer_end(transfer_command_buffer);
    //submit info
    VkCommandBufferSubmitInfo cb_submit_info = vulkan_command_buffer_get_submit_info(transfer_command_buffer);

    VkSemaphoreSubmitInfo signal_semaphore_infos = {0};
    signal_semaphore_infos.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signal_semaphore_infos.pNext = 0;
    // pSignalSemaphoreInfos.deviceIndex;
    signal_semaphore_infos.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    //value assumed to be accurate for the current texture upload
    signal_semaphore_infos.value = semaphore_signal_value;
    signal_semaphore_infos.semaphore = texture_system->timline_texture_upload_semaphore;

    VkSubmitInfo2 submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.pNext = NULL;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &cb_submit_info;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos = &signal_semaphore_infos;
    // vkQueueSubmit2(renderer->context.device.transfer_queue, 1, &submit_info, NULL);
    vkQueueSubmit2(renderer->context.graphics_queue, 1, &submit_info, NULL);
}

Vulkan_Texture* vulkan_texture_system_get_vulkan_texture(Vulkan_Texture_System* system, u32 bindless_index)
{
    return &system->textures[bindless_index];
}


Texture_Handle vulkan_texture_system_add_texture_file(Renderer* renderer, Vulkan_Texture_System* system,
                                                      char const* filepath)
{
    if (hash_table_contains(system->texture_file_to_handle, filepath))
    {
        Texture_Handle* handle;
        hash_table_get(system->texture_file_to_handle, filepath, &handle);
        return *handle;
    }


    //get an available index
    Texture_Handle out_texture_handle;
    out_texture_handle.handle = system->available_texture_indexes;
    //add to hash table
    hash_table_insert(system->texture_file_to_handle, filepath, &out_texture_handle);

    //create the texture
    Vulkan_Texture* out_texture = &system->textures[out_texture_handle.handle];
    create_texture_image(&renderer->context, renderer->context.graphics_command_buffer, filepath, out_texture);

    //increment index for next usage
    system->available_texture_indexes++;

    //TODO: batch this upload once a frame
    update_texture_bindless_descriptor_set(renderer, renderer->descriptor_system, out_texture_handle.handle);

    return out_texture_handle;
}

bool vulkan_texture_system_free(Renderer* renderer, Texture_Handle handle)
{
    Vulkan_Texture* vulkan_texture = &renderer->texture_system->textures[handle.handle];

    //queue the texture to be unloaded
    ring_enqueue(renderer->texture_system->texture_deletion_queue, vulkan_texture);


    return true;
}
