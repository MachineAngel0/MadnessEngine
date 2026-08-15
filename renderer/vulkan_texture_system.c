#include "vulkan_texture_system.h"

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


    //textures gpu upload we are waiting on, checked once a frame
    u32 iteration_count = texture_system->texture_pending_array->num_items;
    for (u32 i = 0; i < iteration_count; i++)
    {
        Vulkan_Texture_Pending_Upload pending_upload = array_get(texture_system->texture_pending_array,
                                                                   Vulkan_Texture_Pending_Upload, i);

        if (timeline_semaphore_query_and_compare(renderer, &texture_system->timline_texture_upload_semaphore,
                                                 pending_upload.timeline_semaphore_value))
        {
            pending_upload.madness_texture->bindless_slot_query = pending_upload.madness_texture->bindless_slot;
        }
    }

    //upload textures into the gpu
    const u8 frame_upload_budget = 16; // textures we upload per frame
    u8 current_upload_count = 0;
    Texture_GPU_Upload texture_upload;

    while (!ring_queue_is_empty(packet->texture_upload_queue) && current_upload_count < frame_upload_budget)
    {
        ring_dequeue(packet->texture_upload_queue, &texture_upload);

        Vulkan_Texture* vulkan_texture = &renderer->texture_system->textures[texture_upload.madness_texture->
            bindless_slot];

        //create the texture
        vulkan_texture_create_image_with_semaphore(renderer, &renderer->context, renderer->context.graphics_command_buffer, &texture_upload,
                                         vulkan_texture, &texture_system->timline_texture_upload_semaphore);
        update_texture_bindless_descriptor_set(renderer, renderer->descriptor_system,
                                               texture_upload.madness_texture->bindless_slot);

        //update the bindless index
        texture_upload.madness_texture->bindless_slot_query = texture_upload.madness_texture->bindless_slot;


        //unload texture data - safe to do so here, but we cant use it yet until the upload is complete
        allocator_heap_free(texture_upload.texture_memory_allocator, texture_upload.pixel_data);

        u64 semaphore_singal_value = ++texture_system->timeline_semaphore_texture_value;
        Vulkan_Texture_Pending_Upload new_pending_upload = {
            .madness_texture = texture_upload.madness_texture, .texture = vulkan_texture,
            .timeline_semaphore_value = semaphore_singal_value
        };
        array_push(texture_system->texture_pending_array, &new_pending_upload);

        current_upload_count++;
    }

    //free textures
    Vulkan_Texture* texture;
    while (!ring_queue_is_empty(texture_system->texture_deletion_queue))
    {
        ring_dequeue(texture_system->texture_deletion_queue, &texture);

        MASSERT(texture);

        vulkan_texture_free(&renderer->context, texture);
    }
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
