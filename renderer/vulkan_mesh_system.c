#include "vulkan_mesh_system.h"

#include "scene.h"
#include "vk_buffer.h"
#include "vulkan_struct_types.h"
#include "logger.h"
#include "sort_key.h"
#include "vulkan_shader_system.h"


Vulkan_Mesh_System* vulkan_mesh_system_init(Renderer* renderer)
{
    Vulkan_Mesh_System* mesh_system = allocator_alloc(&renderer->allocator, sizeof(Vulkan_Mesh_System));

    u64 mesh_buffer_data_size = MB(64);


    mesh_system->vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                             BUFFER_TYPE_STORAGE_GPU,
                                                             mesh_buffer_data_size);
    mesh_system->index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                            mesh_buffer_data_size);


    mesh_system->normal_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                             BUFFER_TYPE_STORAGE_GPU, mesh_buffer_data_size);
    mesh_system->tangent_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                              BUFFER_TYPE_STORAGE_GPU, mesh_buffer_data_size);
    mesh_system->uv_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                         BUFFER_TYPE_STORAGE_GPU,
                                                         mesh_buffer_data_size);
    mesh_system->joint_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                            BUFFER_TYPE_STORAGE_GPU,
                                                            mesh_buffer_data_size);
    mesh_system->weight_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                             BUFFER_TYPE_STORAGE_GPU,
                                                             mesh_buffer_data_size);


    //
    mesh_system->mesh_submitted_pending_array = array_create(Mesh_Gpu_Upload_Pending, 1024, &renderer->allocator);
    mesh_system->mesh_not_yet_uploaded_array = array_create(Mesh_Unfinished_Upload, 1024, &renderer->allocator);


    mesh_system->skinned_submitted_pending_array = array_create(Mesh_Gpu_Upload_Pending, 1024, &renderer->allocator);
    mesh_system->skinned_not_yet_uploaded_array = array_create(Mesh_Unfinished_Upload, 1024, &renderer->allocator);




    return mesh_system;
}


bool mesh_system_upload_data_into_buffers(Renderer* renderer, Vulkan_Mesh_System* mesh_system,
                                          Array* submit_pending_array,
                                          Array* not_yet_submitted_array,
                                          Vulkan_Command_Buffer* transfer_command_buffer,
                                          Buffer_Handle handle,
                                          Vulkan_Mesh_Data_Type type,
                                          void* data,
                                          u64 data_byte_size,
                                          u32 mesh_id,
                                          u64 mesh_upload_semaphore_value,
                                          u8* pending_uploads_count, bool is_initial_submit)
{
    if (data_byte_size == 0)
    {
        WARN("mesh_system_upload_data: passed in 0 data size, for type %d", type);
        return true;
    }

    if (is_initial_submit)
    {
        *pending_uploads_count += 1;
    }

    if (vulkan_buffer_transfer_upload(renderer,
                                      transfer_command_buffer,
                                      handle,
                                      data,
                                      data_byte_size, mesh_upload_semaphore_value))
    {
        //successful upload
        Mesh_Gpu_Upload_Pending mesh_pending =
        {
            .submesh_id = mesh_id,
            .type = type,
            .timeline_semaphore_value = mesh_upload_semaphore_value,
        };
        array_push(submit_pending_array, &mesh_pending);
        return true;
    }
    else
    {
        //we dont want to add this again, if its already an unfinished upload
        if (is_initial_submit)
        {
            //not enough memory, will try again next frame
            Mesh_Unfinished_Upload unfinished_upload = {
                .submesh_id = mesh_id,
                .type = type,
                .buffer_handle = handle,
                .bytes = data_byte_size,
                .data = data,
            };

            array_push(not_yet_submitted_array, &unfinished_upload);
        }
        return false;
    }
}


void vulkan_mesh_system_update_async_resources(Renderer* renderer, Vulkan_Mesh_System* mesh_system)
{
    //mesh gpu uploads we are waiting on, checked once a frame
    u64 mesh_pending_index = 0;

    while (mesh_pending_index < mesh_system->mesh_submitted_pending_array->num_items)
    {
        Mesh_Gpu_Upload_Pending pending_upload =
            array_get(
                mesh_system->mesh_submitted_pending_array,
                mesh_pending_index,
                Mesh_Gpu_Upload_Pending);

        if (timeline_semaphore_query_and_compare(
            renderer,
            vulkan_queue_system_get_transfer_semaphore(renderer),
            pending_upload.timeline_semaphore_value))
        {
            Mesh_Render_Record* mesh_record = &mesh_system->mesh_render_record[pending_upload.submesh_id];
            mesh_record->pending_uploads--;
            if (mesh_record->pending_uploads == 0)
            {
                mesh_record->is_uploaded = true;
            }

            array_remove_swap(
                mesh_system->mesh_submitted_pending_array,
                mesh_pending_index);

            // Don't increment i.
            // The swapped-in element now occupies index i.
            continue;
        }

        mesh_pending_index++;
    }


    u64 skinned_pending_index = 0;
    while (skinned_pending_index < mesh_system->skinned_submitted_pending_array->num_items)
    {
        Mesh_Gpu_Upload_Pending pending_upload =
            array_get(
                mesh_system->skinned_submitted_pending_array,
                skinned_pending_index,
                Mesh_Gpu_Upload_Pending);

        if (timeline_semaphore_query_and_compare(
            renderer,
            vulkan_queue_system_get_transfer_semaphore(renderer),
            pending_upload.timeline_semaphore_value))
        {
            Skinned_Mesh_Render_Record* mesh_record = &mesh_system->skinned_render_record[pending_upload.submesh_id];
            mesh_record->pending_uploads--;
            if (mesh_record->pending_uploads == 0)
            {
                mesh_record->is_uploaded = true;
            }

            array_remove_swap(
                mesh_system->skinned_submitted_pending_array,
                skinned_pending_index);

            // Don't increment i.
            // The swapped-in element now occupies index i.
            continue;
        }

        skinned_pending_index++;
    }
}


void vulkan_mesh_system_upload_new_resources(Renderer* renderer, Vulkan_Mesh_System* mesh_system,
                                             Render_Packet* render_packet)
{
    PROFILE_ZONE(vulkan_mesh_system_upload_new_resources)

    if (ring_queue_is_empty(render_packet->mesh_queue) && ring_queue_is_empty(render_packet->skinned_mesh_queue))
    {
        return;
    }


    //TODO: we still want to flush our queues
    Vulkan_Command_Buffer* transfer_command_buffer = NULL;
    u64 mesh_upload_semaphore_value = 0;
    if (!vulkan_queue_system_get_transfer_command_buffer(renderer, &transfer_command_buffer, &mesh_upload_semaphore_value))
    {
        PROFILE_ZONE_END(vulkan_mesh_system_upload_new_resources)
        return;
    }



    // mesh data we werent able to upload yet
     u64 unfinished_idx = 0;
     while (unfinished_idx < mesh_system->mesh_not_yet_uploaded_array->num_items)
     {
         Mesh_Unfinished_Upload unfinished_upload =
             array_get(
                 mesh_system->mesh_not_yet_uploaded_array,
                 unfinished_idx,
                 Mesh_Unfinished_Upload);


         Mesh_Render_Record* record = &mesh_system->mesh_render_record[unfinished_upload.submesh_id];

         //if upload successful remove it from the array
         if (mesh_system_upload_data_into_buffers(renderer,
                                                  mesh_system,
                                                  mesh_system->mesh_submitted_pending_array,
                                                  mesh_system->mesh_not_yet_uploaded_array,
                                                  transfer_command_buffer,
                                                  unfinished_upload.buffer_handle,
                                                  unfinished_upload.type,
                                                  unfinished_upload.data,
                                                  unfinished_upload.bytes,
                                                  unfinished_upload.submesh_id,
                                                  mesh_upload_semaphore_value,
                                                  &record->pending_uploads, false))
         {
             // Don't increment i.
             // The swapped-in element now occupies index i.
             array_remove_swap(
                 mesh_system->mesh_not_yet_uploaded_array,
                 unfinished_idx);
             continue;
         }
         //upload could not be done
         unfinished_idx++;
     }


    u64 skinned_unfinished_idx = 0;
    while (skinned_unfinished_idx < mesh_system->skinned_not_yet_uploaded_array->num_items)
    {
        Mesh_Unfinished_Upload unfinished_upload =
            array_get(
                mesh_system->skinned_not_yet_uploaded_array,
                skinned_unfinished_idx,
                Mesh_Unfinished_Upload);


        Skinned_Mesh_Render_Record* record = &mesh_system->skinned_render_record[unfinished_upload.submesh_id];

        //if upload successful remove it from the array
        if (mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->skinned_submitted_pending_array,
                                                 mesh_system->skinned_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 unfinished_upload.buffer_handle,
                                                 unfinished_upload.type,
                                                 unfinished_upload.data,
                                                 unfinished_upload.bytes,
                                                 unfinished_upload.submesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, false))
        {
            // Don't increment i.
            // The swapped-in element now occupies index i.
            array_remove_swap(
                mesh_system->skinned_not_yet_uploaded_array,
                skinned_unfinished_idx);
            continue;
        }
        //upload could not be done
        skinned_unfinished_idx++;
    }


    //TODO: get skinned data out of the upload data as well
    ring_queue* mesh_render_queue = render_packet->mesh_queue;
    ring_queue* skinned_mesh_render_queue = render_packet->skinned_mesh_queue;

    //NOTE: rn it copies from an offset, which is fine for now,
    // but when the system needs to be more dynamic, its going to need a rewrite, especially the buffer system function calls

    //TODO: add memory barriers
    Mesh_GPU_Upload submesh_upload_data = {0};


    while (!ring_queue_is_empty(mesh_render_queue))
    {
        ring_dequeue(mesh_render_queue, &submesh_upload_data);

        Mesh_Render_Record* record = &mesh_system->mesh_render_record[submesh_upload_data.mesh_id];
        record->submesh_count = submesh_upload_data.submesh_count;
        record->mesh_id = submesh_upload_data.mesh_id;
        record->is_uploaded = false; // TODO: change this for now we are just testing, should be false
        record->submesh_data = submesh_upload_data.submesh;


        //allocate space for the record
        record->tangent_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                            sizeof(u64) * record->submesh_count);
        record->vertex_color_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                                 sizeof(u64) * record->submesh_count);
        record->vertex_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                           sizeof(u64) * record->submesh_count);
        record->normal_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                           sizeof(u64) * record->submesh_count);
        record->uv_bytes_offset = allocator_heap_alloc(renderer->heap_allocator, sizeof(u64) * record->submesh_count);

        //makes indirect draw easier to use
        record->index_count = allocator_heap_alloc(renderer->heap_allocator, sizeof(u32) * record->submesh_count);
        record->vertex_count_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                           sizeof(u32) * record->submesh_count);
        record->index_offset_count =
            allocator_heap_alloc(renderer->heap_allocator, sizeof(u32) * record->submesh_count);

        record->index_type = allocator_heap_alloc(renderer->heap_allocator, sizeof(Index_Type) * record->submesh_count);


        for (u32 i = 0; i < submesh_upload_data.submesh_count; i++)
        {
            // TODO: change this for now we are just testing, should be false
            //indirect draw data
            record->index_count[i] = submesh_upload_data.submesh[i].index_count;
            record->index_type[i] = submesh_upload_data.submesh[i].index_type;

            //TODO: TEMP CODE, need a free list for this
            record->vertex_count_offset[i] = mesh_system->vertex_offset_count;
            record->index_offset_count[i] = mesh_system->index_offset_count;

            mesh_system->vertex_offset_count += submesh_upload_data.submesh[i].vertex_count;
            mesh_system->index_offset_count += submesh_upload_data.submesh[i].index_count;

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->mesh_submitted_pending_array,
                                                 mesh_system->mesh_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->vertex_buffer_handle,
                                                 VERTEX,
                                                 submesh_upload_data.gpu_data[i].vertex,
                                                 submesh_upload_data.submesh[i].vertex_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->mesh_submitted_pending_array,
                                                 mesh_system->mesh_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->index_buffer_handle,
                                                 INDEX,
                                                 submesh_upload_data.gpu_data[i].indices,
                                                 submesh_upload_data.submesh[i].indices_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->mesh_submitted_pending_array,
                                                 mesh_system->mesh_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->normal_buffer_handle,
                                                 NORMAL,
                                                 submesh_upload_data.gpu_data[i].normal,
                                                 submesh_upload_data.submesh[i].normal_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->mesh_submitted_pending_array,
                                                 mesh_system->mesh_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->uv_buffer_handle,
                                                 UV,
                                                 submesh_upload_data.gpu_data[i].uv,
                                                 submesh_upload_data.submesh[i].uv_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);
            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->mesh_submitted_pending_array,
                                                 mesh_system->mesh_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->tangent_buffer_handle,
                                                 TANGENT,
                                                 submesh_upload_data.gpu_data[i].tangent,
                                                 submesh_upload_data.submesh[i].tangent_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);
        }
    }


    //skinned data
    Skinned_Mesh_GPU_Upload skinned_mesh_upload_data = {0};
    while (!ring_queue_is_empty(skinned_mesh_render_queue))
    {
        ring_dequeue(skinned_mesh_render_queue, &skinned_mesh_upload_data);

        Skinned_Mesh_Render_Record* record = &mesh_system->skinned_render_record[skinned_mesh_upload_data.mesh_id];
        record->submesh_count = skinned_mesh_upload_data.submesh_count;
        record->mesh_id = skinned_mesh_upload_data.mesh_id;
        record->is_uploaded = false; // TODO: change this for now we are just testing, should be false
        record->submesh_data = skinned_mesh_upload_data.submesh;


        //allocate space for the record
        record->tangent_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                            sizeof(u64) * record->submesh_count);
        record->vertex_color_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                                 sizeof(u64) * record->submesh_count);
        record->vertex_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                           sizeof(u64) * record->submesh_count);
        record->normal_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                           sizeof(u64) * record->submesh_count);
        record->uv_bytes_offset = allocator_heap_alloc(renderer->heap_allocator, sizeof(u64) * record->submesh_count);

        //makes indirect draw easier to use
        record->index_count = allocator_heap_alloc(renderer->heap_allocator, sizeof(u32) * record->submesh_count);
        record->vertex_count_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                           sizeof(u32) * record->submesh_count);
        record->index_offset_count =
            allocator_heap_alloc(renderer->heap_allocator, sizeof(u32) * record->submesh_count);

        record->index_type = allocator_heap_alloc(renderer->heap_allocator, sizeof(Index_Type) * record->submesh_count);

        record->joints_byte_offset =
            allocator_heap_alloc(renderer->heap_allocator, sizeof(u64) * record->submesh_count);
        record->weight_byte_offset =
            allocator_heap_alloc(renderer->heap_allocator, sizeof(u64) * record->submesh_count);

        record->joint_idx = allocator_heap_alloc(renderer->heap_allocator, sizeof(u64) * record->submesh_count);
        record->weight_idx = allocator_heap_alloc(renderer->heap_allocator, sizeof(u64) * record->submesh_count);

        for (u32 i = 0; i < skinned_mesh_upload_data.submesh_count; i++)
        {
            // TODO: change this for now we are just testing, should be false
            //indirect draw data
            record->index_count[i] = skinned_mesh_upload_data.submesh[i].index_count;
            record->index_type[i] = skinned_mesh_upload_data.submesh[i].index_type;

            //TODO: TEMP CODE, need a free list for this
            record->vertex_count_offset[i] = mesh_system->vertex_offset_count;
            record->index_offset_count[i] = mesh_system->index_offset_count;

            mesh_system->vertex_offset_count += skinned_mesh_upload_data.submesh[i].vertex_count;
            mesh_system->index_offset_count += skinned_mesh_upload_data.submesh[i].index_count;

            record->joints_byte_offset[i] = mesh_system->joint_offset;
            record->weight_byte_offset[i] = mesh_system->weight_offset;

            record->joint_idx[i] = record->joints_byte_offset[i] / sizeof(vec4s);
            record->weight_idx[i] = record->weight_byte_offset[i] / sizeof(vec4s);

            mesh_system->joint_offset += skinned_mesh_upload_data.skinned_submesh_data[i].joint_bytes;
            mesh_system->weight_offset += skinned_mesh_upload_data.skinned_submesh_data[i].weight_bytes;


            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->skinned_submitted_pending_array,
                                                 mesh_system->skinned_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->vertex_buffer_handle,
                                                 VERTEX,
                                                 skinned_mesh_upload_data.gpu_data[i].vertex,
                                                 skinned_mesh_upload_data.submesh[i].vertex_bytes,
                                                 skinned_mesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->skinned_submitted_pending_array,
                                                 mesh_system->skinned_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->index_buffer_handle,
                                                 INDEX,
                                                 skinned_mesh_upload_data.gpu_data[i].indices,
                                                 skinned_mesh_upload_data.submesh[i].indices_bytes,
                                                 skinned_mesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->skinned_submitted_pending_array,
                                                 mesh_system->skinned_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->normal_buffer_handle,
                                                 NORMAL,
                                                 skinned_mesh_upload_data.gpu_data[i].normal,
                                                 skinned_mesh_upload_data.submesh[i].normal_bytes,
                                                 skinned_mesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->skinned_submitted_pending_array,
                                                 mesh_system->skinned_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->uv_buffer_handle,
                                                 UV,
                                                 skinned_mesh_upload_data.gpu_data[i].uv,
                                                 skinned_mesh_upload_data.submesh[i].uv_bytes,
                                                 skinned_mesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);
            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->skinned_submitted_pending_array,
                                                 mesh_system->skinned_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->tangent_buffer_handle,
                                                 TANGENT,
                                                 skinned_mesh_upload_data.gpu_data[i].tangent,
                                                 skinned_mesh_upload_data.submesh[i].tangent_bytes,
                                                 skinned_mesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->skinned_submitted_pending_array,
                                                 mesh_system->skinned_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->joint_buffer_handle,
                                                 JOINT,
                                                 skinned_mesh_upload_data.skinned_gpu_data[i].joints,
                                                 skinned_mesh_upload_data.skinned_submesh_data[i].joint_bytes,
                                                 skinned_mesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);
            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 mesh_system->skinned_submitted_pending_array,
                                                 mesh_system->skinned_not_yet_uploaded_array,
                                                 transfer_command_buffer,
                                                 mesh_system->weight_buffer_handle,
                                                 WEIGHT,
                                                 skinned_mesh_upload_data.skinned_gpu_data[i].weights,
                                                 skinned_mesh_upload_data.skinned_submesh_data[i].weight_bytes,
                                                 skinned_mesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 &record->pending_uploads, true);
        }
    }


    PROFILE_ZONE_END(vulkan_mesh_system_upload_new_resources)
}


void vulkan_mesh_system_construct_draw_lists(Renderer* renderer,
                                             Render_Packet* render_packet,
                                             Vulkan_Command_Buffer* graphics_command_buffer,
                                             Vulkan_Mesh_Render_List* render_list)
{
    PROFILE_ZONE(mesh_renderer_construct_draw_lists)
    //TODO: were going to assume we have already done culling,
    // but we would need to pass in another structure for actually culled data
    // or even cull and create the render format at the same time, instead of two seperate arrays

    vulkan_command_buffer_debug_label_begin_color(renderer, graphics_command_buffer, "MESH DRAW GENERATION",
                                              (float[4]){1.0, 1.0, 0.0, 1.0});
    //steps:
    // 1. cull // TODO:
    // 2. add items into each appropriate renderpass list
    // 3. sort each renderpass lists and their items
    // 4. batch the instances
    // 5. add the draws into the buffer


    // loop through our meshes and create render items for each

    //TODO: TEMP
    render_list->mesh_renderable = allocator_alloc(&renderer->frame_allocator,
                                                   1000 * sizeof(Vulkan_Mesh_Renderable));
    Vulkan_Mesh_Renderable* render_items = render_list->mesh_renderable;
    u32 render_item_count = 0;
    render_list->draw_count = 0; // TODO: move out to a frame start

    // Vulkan_Mesh_Draw draw_data = {
    // .transform_idx = sub_mesh_instance->parent_transform_handle.handle,
    // .material_instance_handle = sub_mesh_instance->material_handle.buffer_handle,
    // };
    render_list->mesh_renderable_count = render_packet->draw_3d_data_packet.mesh_instances_count;

    for (u32 i = 0; i < render_packet->draw_3d_data_packet.mesh_instances_count; i++)
    {
        Madness_Mesh_Instance* mesh_instance = &render_packet->draw_3d_data_packet.mesh_instances[i];
        Mesh_Render_Record* render_record = &renderer->mesh_system->mesh_render_record[mesh_instance->
            madness_mesh_index];

        //check if its valid for uploading
        if (!render_record->is_uploaded)
        {
            continue;
        }

        //TODO: check if material upload is valid yet, when we get around to that,
        //NOTE: YOU CAN JUST ASSUME MATERIALS and TEXTURES ARE VALID


        for (u32 submesh_idx = 0; submesh_idx < mesh_instance->submesh_count; submesh_idx++)
        {
            Vulkan_Mesh_Renderable* render_inst = &render_items[render_item_count++];

            *render_inst = (Vulkan_Mesh_Renderable){
                //TODO: each sort key should vary based on pass, as they are being added into the list
                // .shader_sort_key = sort_key_create_oqaque(mesh_instance->pipeline_hash[i], mesh_instance->permutation[i]),
                .pipeline_hash = mesh_instance->pipeline_hash[submesh_idx],
                .render_pass_masks = 0,
                .mesh_id = mesh_instance->madness_mesh_index,
                .visible = true,
                .index_count = render_record->index_count[submesh_idx],
                .index_offset = render_record->index_offset_count[submesh_idx],
                .vertex_offset = render_record->vertex_count_offset[submesh_idx],
                .mesh_render_item = (Mesh_GPU_Renderable){
                    .material_idx = mesh_instance->material_handles[submesh_idx].handle,
                    .transform_idx = mesh_instance->transform_handle.handle,
                    .normal_offset = render_record->normal_bytes_offset[submesh_idx],
                    .uv_offset = render_record->uv_bytes_offset[submesh_idx],
                    .tangent_offset = render_record->tangent_bytes_offset[submesh_idx],
                }
            };
        }
    }

    //TODO: append to each render pass type


    //TODO: sort the meshes by sort key
    qsort(render_items, render_item_count, sizeof(Vulkan_Mesh_Renderable), vulkan_mesh_sort_key);


    //TODO/ OPTIMIZE : when a render item doesn't match, that means we move onto the next batch
    // we can also upload based on that range instead of individual uploads

    VkDrawIndexedIndirectCommand indirect_draw = {0};

    for (u32 i = 0; i < render_item_count; i++)
    {
        //TODO: check if material upload is valid yet, when we get around to that
        Vulkan_Mesh_Renderable* mesh_item = &render_items[i];

        // Vulkan_Shader* shader = vulkan_shader_system_get_by_pipeline_hash(renderer, mesh_item->pipeline_hash);

        //TODO: batch instances, basically figure it out
        indirect_draw = (VkDrawIndexedIndirectCommand){
            .indexCount = mesh_item->index_count,
            .firstIndex = mesh_item->index_offset,
            .vertexOffset = mesh_item->vertex_offset,
            .instanceCount = 1,
            .firstInstance = 0
        };

        vulkan_buffer_frame_staging_upload(
            renderer, render_list->mesh_draw_data,
            graphics_command_buffer,
            &mesh_item->mesh_render_item,
            sizeof(Mesh_GPU_Renderable));


        vulkan_buffer_frame_staging_upload(
            renderer, render_list->indirect_buffer,
            graphics_command_buffer,
            &indirect_draw,
            sizeof(VkDrawIndexedIndirectCommand));

        render_list->draw_count++; // TODO: modify it later to count each instance draw
    }


    VkBufferMemoryBarrier2 mesh_draw_data_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, render_list->mesh_draw_data)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };


    VkBufferMemoryBarrier2 indirect_buffer_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, render_list->indirect_buffer)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };


    vulkan_command_add_buffer_barrier(graphics_command_buffer, mesh_draw_data_barrier);
    vulkan_command_add_buffer_barrier(graphics_command_buffer, indirect_buffer_barrier);

    vulkan_command_buffer_debug_label_end(renderer, graphics_command_buffer);

    PROFILE_ZONE_END(mesh_renderer_construct_draw_lists)
}

void vulkan_mesh_system_construct_skinned_draw_lists(Renderer* renderer, Render_Packet* render_packet,
                                                     Vulkan_Command_Buffer* graphics_command_buffer,
                                                     Vulkan_Skinned_Mesh_Render_List* render_list)
{
    PROFILE_ZONE(vulkan_mesh_system_construct_skinned_draw_lists)
    //TODO: were going to assume we have already done culling,
    // but we would need to pass in another structure for actually culled data
    // or even cull and create the render format at the same time, instead of two seperate arrays

    // loop through our meshes and create render items for each

    vulkan_command_buffer_debug_label_begin_color(renderer, graphics_command_buffer, "SKINNED DRAW GENERATION",
                                              (float[4]){1.0, 1.0, 0.0, 1.0});

    //TODO: TEMP
    render_list->skinned_mesh_renderable = allocator_alloc(&renderer->frame_allocator,
                                                           1000 * sizeof(Vulkan_Skinned_Renderable));
    Vulkan_Skinned_Renderable* render_items = render_list->skinned_mesh_renderable;
    u32 render_item_count = 0;
    render_list->draw_count = 0; // TODO: move out to a frame start


    render_list->skinned_mesh_renderable_count = render_packet->draw_3d_data_packet.skinned_instances_count;

    for (u32 i = 0; i < render_packet->draw_3d_data_packet.skinned_instances_count; i++)
    {
        Skinned_Mesh_Instance* mesh_instance = &render_packet->draw_3d_data_packet.skinned_instances[i];
        Skinned_Mesh_Render_Record* render_record = &renderer->mesh_system->skinned_render_record[mesh_instance->
            madness_mesh_index];

        //check if its valid for uploading
        if (!render_record->is_uploaded)
        {
            continue;
        }

        //TODO: check if material upload is valid yet, when we get around to that,
        //NOTE: YOU CAN JUST ASSUME MATERIALS and TEXTURES ARE VALID


        for (u32 submesh_idx = 0; submesh_idx < mesh_instance->submesh_count; submesh_idx++)
        {
            Vulkan_Skinned_Renderable* render_inst = &render_items[render_item_count++];

            *render_inst = (Vulkan_Skinned_Renderable){
                //TODO: each sort key should vary based on pass, as they are being added into the list
                // .shader_sort_key = sort_key_create_oqaque(mesh_instance->pipeline_hash[i], mesh_instance->permutation[i]),
                .pipeline_hash = mesh_instance->pipeline_hash[submesh_idx],
                .permutation = mesh_instance->permutation[submesh_idx],
                .render_pass_masks = 0,
                .mesh_id = mesh_instance->madness_mesh_index,
                .visible = true,
                .index_count = render_record->index_count[submesh_idx],
                .index_offset = render_record->index_offset_count[submesh_idx],
                .vertex_offset = render_record->vertex_count_offset[submesh_idx],
                .skinned_render_item = (Skinned_GPU_Renderable){
                    .material_idx = mesh_instance->material_handles[submesh_idx].handle,
                    .transform_idx = mesh_instance->transform_handle.handle,
                    .normal_offset = render_record->normal_bytes_offset[submesh_idx],
                    .uv_offset = render_record->uv_bytes_offset[submesh_idx],
                    .tangent_offset = render_record->tangent_bytes_offset[submesh_idx],
                    .joint_idx = render_record->joint_idx[submesh_idx],
                    .weight_idx = render_record->weight_idx[submesh_idx],
                    .vertex_offset = render_record->vertex_count_offset[submesh_idx],
                    .skinned_matrix_idx = mesh_instance->skinned_matrix_count_offset,
                }
            };
        }
    }

    //TODO: append to each render pass type


    //TODO: sort the meshes by sort key
    qsort(render_items, render_item_count, sizeof(Vulkan_Skinned_Renderable), vulkan_mesh_sort_key);


    //TODO/ OPTIMIZE : when a render item doesn't match, that means we move onto the next batch
    // we can also upload based on that range instead of individual uploads

    VkDrawIndexedIndirectCommand indirect_draw = {0};

    for (u32 i = 0; i < render_item_count; i++)
    {
        //TODO: check if material upload is valid yet, when we get around to that
        Vulkan_Skinned_Renderable* mesh_item = &render_items[i];

        // Vulkan_Shader* shader = vulkan_shader_system_get_by_pipeline_hash(renderer, mesh_item->pipeline_hash);

        //TODO: batch instances, basically figure it out
        indirect_draw = (VkDrawIndexedIndirectCommand){
            .indexCount = mesh_item->index_count,
            .firstIndex = mesh_item->index_offset,
            .vertexOffset = mesh_item->vertex_offset,
            .instanceCount = 1,
            .firstInstance = 0
        };

        vulkan_buffer_frame_staging_upload(
            renderer, render_list->skinned_draw_data,
            graphics_command_buffer,
            &mesh_item->skinned_render_item,
            sizeof(Skinned_GPU_Renderable));


        vulkan_buffer_frame_staging_upload(
            renderer, render_list->skinned_indirect_buffer,
            graphics_command_buffer,
            &indirect_draw,
            sizeof(VkDrawIndexedIndirectCommand));

        render_list->draw_count++; // TODO: modify it later to count each instance draw
    }


    VkBufferMemoryBarrier2 skinned_draw_data_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, render_list->skinned_draw_data)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };


    VkBufferMemoryBarrier2 indirect_buffer_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, render_list->skinned_indirect_buffer)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };


    vulkan_command_add_buffer_barrier(graphics_command_buffer, skinned_draw_data_barrier);
    vulkan_command_add_buffer_barrier(graphics_command_buffer, indirect_buffer_barrier);

    vulkan_command_buffer_debug_label_end(renderer, graphics_command_buffer);

    PROFILE_ZONE_END(vulkan_mesh_system_construct_skinned_draw_lists);
}


void mesh_renderer_batch_draw(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                              Vulkan_Command_Buffer* command_buffer, Vulkan_Mesh_Render_List* render_list)
{
    PROFILE_ZONE(mesh_renderer_batch_draw)
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant

    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, "MESH",
                                                  (float[4]){1.0, 1.0, 0.0, 1.0});

    Vulkan_Buffer* indirect_buffer = vulkan_buffer_get_frame(renderer, render_list->indirect_buffer);

    for (u32 draw_idx = 0; draw_idx < render_list->draw_count; ++draw_idx)
    {
        Vulkan_Shader_Batch* vulkan_shader = vulkan_shader_system_get_by_pipeline_hash(
            renderer, render_list->mesh_renderable[draw_idx].pipeline_hash);

        if (!vulkan_shader)
        {
            FATAL("mesh_renderer_batch_draw: could not find vulkan shader")
            continue;
        }

        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //get pipeline
        VkPipeline pipeline = vulkan_shader->pipeline.handle;
        VkPipelineLayout layout = vulkan_shader->pipeline.pipeline_layout;

        //check if we are using wireframe_mode
        if (renderer->wireframe_mode)
        {
            pipeline = vulkan_shader->wireframe_pipeline.handle;
            layout = vulkan_shader->wireframe_pipeline.pipeline_layout;;
        }


        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);


        PC_General pc_data = {
            .draw_data_buffer = vulkan_buffer_get_frame_device_address(renderer, render_list->mesh_draw_data),
            .extra_buffer = 0,
        };


        VkPushConstantsInfo push_constant_info = {0};
        push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
        push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_info.layout = layout;
        push_constant_info.offset = 0;
        push_constant_info.size = sizeof(PC_General);
        // push_constant_info.size = sizeof(draw_data->general); // make sure its not a pointer if i use this
        push_constant_info.pValues = &pc_data;
        push_constant_info.pNext = NULL;
        vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


        if (renderer->features2.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     render_list->draw_count,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (u64 j = 0; j < render_list->draw_count; j++)
            {
                vkCmdDrawIndexedIndirect(command_buffer->handle,
                                         indirect_buffer->handle,
                                         j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                         sizeof(VkDrawIndexedIndirectCommand));
            }
        }
    }


    vulkan_command_buffer_debug_label_end(renderer, command_buffer);

    PROFILE_ZONE_END(mesh_renderer_batch_draw)
}


void mesh_renderer_batch_draw_custom_pipeline(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                              Vulkan_Command_Buffer* command_buffer,
                                              Vulkan_Shader_Pipeline* vulkan_pipeline,
                                              Vulkan_Mesh_Render_List* render_list, const char* name)
{
        PROFILE_ZONE(mesh_renderer_batch_draw)
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant

    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, name,
                                                  (float[4]){1.0, 1.0, 0.0, 1.0});

    Vulkan_Buffer* indirect_buffer = vulkan_buffer_get_frame(renderer, render_list->indirect_buffer);

    for (u32 draw_idx = 0; draw_idx < render_list->draw_count; ++draw_idx)
    {
        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //get pipeline
        VkPipeline pipeline = vulkan_pipeline->handle;
        VkPipelineLayout layout = vulkan_pipeline->pipeline_layout;


        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);


        PC_General pc_data = {
            .draw_data_buffer = vulkan_buffer_get_frame_device_address(renderer, render_list->mesh_draw_data),
            .extra_buffer = 0,
        };


        VkPushConstantsInfo push_constant_info = {0};
        push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
        push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_info.layout = layout;
        push_constant_info.offset = 0;
        push_constant_info.size = sizeof(PC_General);
        // push_constant_info.size = sizeof(draw_data->general); // make sure its not a pointer if i use this
        push_constant_info.pValues = &pc_data;
        push_constant_info.pNext = NULL;
        vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


        if (renderer->features2.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     render_list->draw_count,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (u64 j = 0; j < render_list->draw_count; j++)
            {
                vkCmdDrawIndexedIndirect(command_buffer->handle,
                                         indirect_buffer->handle,
                                         j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                         sizeof(VkDrawIndexedIndirectCommand));
            }
        }
    }


    vulkan_command_buffer_debug_label_end(renderer, command_buffer);

    PROFILE_ZONE_END(mesh_renderer_batch_draw)
}


void mesh_renderer_batch_draw_custom_pipeline_and_push_constant(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                              Vulkan_Command_Buffer* command_buffer,
                                              Vulkan_Shader_Pipeline* vulkan_pipeline,
                                              Vulkan_Mesh_Render_List* render_list, void* push_constant, u32 push_constant_size, const char* name)
{
        PROFILE_ZONE(mesh_renderer_batch_draw)
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant

    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, name,
                                                  (float[4]){1.0, 1.0, 0.0, 1.0});

    Vulkan_Buffer* indirect_buffer = vulkan_buffer_get_frame(renderer, render_list->indirect_buffer);

    for (u32 draw_idx = 0; draw_idx < render_list->draw_count; ++draw_idx)
    {
        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //get pipeline
        VkPipeline pipeline = vulkan_pipeline->handle;
        VkPipelineLayout layout = vulkan_pipeline->pipeline_layout;


        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);




        VkPushConstantsInfo push_constant_info = {0};
        push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
        push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_info.layout = layout;
        push_constant_info.offset = 0;
        push_constant_info.size = push_constant_size;
        // push_constant_info.size = sizeof(draw_data->general); // make sure its not a pointer if i use this
        push_constant_info.pValues = push_constant;
        push_constant_info.pNext = NULL;
        vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


        if (renderer->features2.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     render_list->draw_count,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (u64 j = 0; j < render_list->draw_count; j++)
            {
                vkCmdDrawIndexedIndirect(command_buffer->handle,
                                         indirect_buffer->handle,
                                         j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                         sizeof(VkDrawIndexedIndirectCommand));
            }
        }
    }


    vulkan_command_buffer_debug_label_end(renderer, command_buffer);

    PROFILE_ZONE_END(mesh_renderer_batch_draw)
}



void mesh_renderer_skinned_batch_draw(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                      Vulkan_Command_Buffer* command_buffer, Vulkan_Skinned_Mesh_Render_List* render_list)
{
    PROFILE_ZONE(mesh_renderer_batch_draw)
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant

    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, "SKINNED MESH",
                                                  (float[4]){1.0, 1.0, 0.0, 1.0});

    Vulkan_Buffer* indirect_buffer = vulkan_buffer_get_frame(renderer, render_list->skinned_indirect_buffer);

    //TODO: draw count is temporary until we do instancing properly
    for (u32 draw_idx = 0; draw_idx < render_list->skinned_mesh_renderable_count; ++draw_idx)
    {
        Vulkan_Shader_Batch* vulkan_shader = vulkan_shader_system_get_by_pipeline_hash(
            renderer, render_list->skinned_mesh_renderable[draw_idx].pipeline_hash);

        if (!vulkan_shader)
        {
            FATAL("mesh_renderer_batch_draw: could not find vulkan shader")
            continue;
        }

        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //get pipeline
        VkPipeline pipeline = vulkan_shader->pipeline.handle;
        VkPipelineLayout layout = vulkan_shader->pipeline.pipeline_layout;

        //check if we are using wireframe_mode
        if (renderer->wireframe_mode)
        {
            pipeline = vulkan_shader->wireframe_pipeline.handle;
            layout = vulkan_shader->wireframe_pipeline.pipeline_layout;;
        }


        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);


        PC_General pc_data = {
            .draw_data_buffer = vulkan_buffer_get_frame_device_address(renderer, render_list->skinned_draw_data),
            .extra_buffer = 0,
        };


        VkPushConstantsInfo push_constant_info = {0};
        push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
        push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_info.layout = layout;
        push_constant_info.offset = 0;
        push_constant_info.size = sizeof(PC_General);
        // push_constant_info.size = sizeof(draw_data->general); // make sure its not a pointer if i use this
        push_constant_info.pValues = &pc_data;
        push_constant_info.pNext = NULL;
        vkCmdPushConstants2(command_buffer->handle, &push_constant_info);

        if (renderer->features2.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     render_list->draw_count,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (u64 j = 0; j < render_list->draw_count; j++)
            {
                vkCmdDrawIndexedIndirect(command_buffer->handle,
                                         indirect_buffer->handle,
                                         j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                         sizeof(VkDrawIndexedIndirectCommand));
            }
        }
    }


    vulkan_command_buffer_debug_label_end(renderer, command_buffer);

    PROFILE_ZONE_END(mesh_renderer_batch_draw)
}



void mesh_renderer_skinned_batch_draw_custom_pipeline(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                      Vulkan_Command_Buffer* command_buffer,
                                      Vulkan_Shader_Pipeline* vulkan_pipeline,
                                      Vulkan_Skinned_Mesh_Render_List* render_list)
{
    PROFILE_ZONE(mesh_renderer_batch_draw)
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant

    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, "SKINNED MESH",
                                                  (float[4]){1.0, 1.0, 0.0, 1.0});

    Vulkan_Buffer* indirect_buffer = vulkan_buffer_get_frame(renderer, render_list->skinned_indirect_buffer);

    //TODO: draw count is temporary until we do instancing properly
    for (u32 draw_idx = 0; draw_idx < render_list->skinned_mesh_renderable_count; ++draw_idx)
    {

        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //get pipeline
        VkPipeline pipeline = vulkan_pipeline->handle;
        VkPipelineLayout layout = vulkan_pipeline->pipeline_layout;

        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);


        PC_General pc_data = {
            .draw_data_buffer = vulkan_buffer_get_frame_device_address(renderer, render_list->skinned_draw_data),
            .extra_buffer = 0,
        };


        VkPushConstantsInfo push_constant_info = {0};
        push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
        push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_info.layout = layout;
        push_constant_info.offset = 0;
        push_constant_info.size = sizeof(PC_General);
        // push_constant_info.size = sizeof(draw_data->general); // make sure its not a pointer if i use this
        push_constant_info.pValues = &pc_data;
        push_constant_info.pNext = NULL;
        vkCmdPushConstants2(command_buffer->handle, &push_constant_info);

        if (renderer->features2.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     render_list->draw_count,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (u64 j = 0; j < render_list->draw_count; j++)
            {
                vkCmdDrawIndexedIndirect(command_buffer->handle,
                                         indirect_buffer->handle,
                                         j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                         sizeof(VkDrawIndexedIndirectCommand));
            }
        }
    }


    vulkan_command_buffer_debug_label_end(renderer, command_buffer);

    PROFILE_ZONE_END(mesh_renderer_batch_draw)
}