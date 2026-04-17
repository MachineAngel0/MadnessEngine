#include "mesh_render.h"

Mesh_Renderer* mesh_renderer_init(Renderer* renderer, Resource_System* resource_system)
{
    Mesh_Renderer* out_mesh_renderer = arena_alloc(&renderer->arena, sizeof(Mesh_Renderer));

    u64 mesh_buffer_data_size = MB(16);


    out_mesh_renderer->mesh_shader_permutations = arena_alloc(&renderer->arena, sizeof(Mesh_Pipeline_Permutations));
    out_mesh_renderer->mesh_shader_permutations->permutation_keys = darray_create_reserve(u32, 100);

    out_mesh_renderer->vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_VERTEX,
                                                                   mesh_buffer_data_size);
    out_mesh_renderer->index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                                  mesh_buffer_data_size);
    out_mesh_renderer->indirect_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_INDIRECT, mesh_buffer_data_size);
    out_mesh_renderer->normal_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE, mesh_buffer_data_size);
    out_mesh_renderer->tangent_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                    BUFFER_TYPE_CPU_STORAGE, mesh_buffer_data_size);
    out_mesh_renderer->uv_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                               BUFFER_TYPE_CPU_STORAGE,
                                                               mesh_buffer_data_size);



    out_mesh_renderer->vertex_staging_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING,
        mesh_buffer_data_size);
    out_mesh_renderer->index_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                          BUFFER_TYPE_STAGING,
                                                                          mesh_buffer_data_size);
    out_mesh_renderer->indirect_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                             BUFFER_TYPE_STAGING,
                                                                             mesh_buffer_data_size);
    out_mesh_renderer->normal_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_STAGING, mesh_buffer_data_size);
    out_mesh_renderer->tangent_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                            BUFFER_TYPE_STAGING, mesh_buffer_data_size);
    out_mesh_renderer->uv_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_STAGING,
                                                                       mesh_buffer_data_size);

    out_mesh_renderer->uv_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_STAGING,
                                                                       mesh_buffer_data_size);

    //DRAW DATA
    out_mesh_renderer->draw_data_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE,
                                                                   mesh_buffer_data_size);
    out_mesh_renderer->draw_data_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                               BUFFER_TYPE_STAGING,
                                                               mesh_buffer_data_size);




    //from other systems
    out_mesh_renderer->pc_mesh.ubo_buffer_idx = renderer->buffer_system->global_ubo_handle.handle;



    return out_mesh_renderer;
}


void mesh_renderer_upload_draw_data_new(Renderer* renderer, Mesh_Renderer* mesh_renderer, Render_Packet* render_packet,
                                        vulkan_command_buffer* command_buffer)
{
    //update the transforms every frame


    ring_queue* mesh_render_queue = render_packet->mesh_queue;

    //return if its empty
    if (ring_queue_is_empty(mesh_render_queue)) return;

    Mesh_Upload_Data* submesh_upload_data = arena_alloc(&renderer->frame_arena, sizeof(Mesh_Upload_Data));

    //NOTE: rn it copies from an offset, which is fine for now,
    // but when the system needs to be more dynamic, its going to need a rewrite, especially the buffer system funciton calls
    while (!ring_queue_is_empty(mesh_render_queue))
    {
        ring_dequeue(mesh_render_queue, submesh_upload_data);

        //this could be optimized later, by using flat arrays for all the submeshes and just doing a memcpy
        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->vertex_staging_buffer_handle,
                                            submesh_upload_data->pos,
                                            submesh_upload_data->vertex_bytes);
        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->index_staging_buffer_handle,
                                            submesh_upload_data->indices,
                                            submesh_upload_data->indices_bytes);

        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->normal_staging_buffer_handle,
                                            submesh_upload_data->normal,
                                            submesh_upload_data->normal_bytes);

        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->uv_staging_buffer_handle,
                                            submesh_upload_data->uv,
                                            submesh_upload_data->uv_bytes);


    }


    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->vertex_buffer_handle,
                                    mesh_renderer->vertex_staging_buffer_handle, command_buffer);
    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->index_buffer_handle,
                                    mesh_renderer->index_staging_buffer_handle, command_buffer);
    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->normal_buffer_handle,
                                    mesh_renderer->normal_staging_buffer_handle, command_buffer);
    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->uv_buffer_handle, mesh_renderer->uv_staging_buffer_handle,
                                    command_buffer);
    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->tangent_buffer_handle,
                                    mesh_renderer->tangent_staging_buffer_handle, command_buffer);

}


void mesh_renderer_construct_indirect_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer,
                                           Render_Packet* render_packet, vulkan_command_buffer* command_buffer)
{
    Render_Packet_Mesh* mesh_render_packet = &render_packet->mesh_data_packet;

    vulkan_buffer_reset_offset(renderer, mesh_renderer->indirect_staging_buffer_handle);

    mesh_renderer->indirect_draw_count = 0;
    // were assuming that the data is already in the buffers,
    // we are just generating the draw calls

    Mesh_Indirect_Draw_Data* draw_data = mesh_render_packet->draw_data;
    u32 draw_data_size = mesh_render_packet->draw_data_size;

    VkDrawIndexedIndirectCommand indirect_draw = {0};
    for (u32 mesh_idx = 0; mesh_idx < draw_data_size; mesh_idx++)
    {
        Mesh_Indirect_Draw_Data* current_submesh = &draw_data[mesh_idx];

        indirect_draw.firstIndex = current_submesh->index_offset;
        indirect_draw.indexCount = current_submesh->index_count;
        indirect_draw.vertexOffset = current_submesh->vertex_offset;
        //instance data
        indirect_draw.instanceCount = 1;
        indirect_draw.firstInstance = 0;

        mesh_renderer->indirect_draw_count++;

        //add the draw data
        //push the indirect draw into the draw data
        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->indirect_staging_buffer_handle,
                                            &indirect_draw,
                                            sizeof(VkDrawIndexedIndirectCommand));
    }


    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->indirect_buffer_handle,
                                    mesh_renderer->indirect_staging_buffer_handle, command_buffer);
}

void mesh_renderer_construct_indirect_draw_new(Renderer* renderer, Mesh_Renderer* mesh_renderer,
    Render_Packet* render_packet, vulkan_command_buffer* command_buffer)
{

    Render_Packet_Mesh* mesh_render_packet = &render_packet->mesh_data_packet;

    vulkan_buffer_reset_offset(renderer, mesh_renderer->indirect_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->draw_data_staging_buffer_handle);

    mesh_renderer->indirect_draw_count = 0;
    // were assuming that the data is already in the buffers,
    // we are just generating the draw calls

    Mesh_Indirect_Draw_Data* draw_data = mesh_render_packet->draw_data;
    u32 draw_data_size = mesh_render_packet->draw_data_size;




    VkDrawIndexedIndirectCommand indirect_draw = {0};
    Mesh_Draw_Data mesh_draw_data = {0};
    for (u32 mesh_idx = 0; mesh_idx < draw_data_size; mesh_idx++)
    {
        Mesh_Indirect_Draw_Data* current_submesh = &draw_data[mesh_idx];

        indirect_draw.firstIndex = current_submesh->index_offset;
        indirect_draw.indexCount = current_submesh->index_count;
        indirect_draw.vertexOffset = current_submesh->vertex_offset;
        //instance data
        indirect_draw.instanceCount = 1;
        indirect_draw.firstInstance = 0;

        mesh_renderer->indirect_draw_count++;


        //push the indirect draw into the draw data
        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->indirect_staging_buffer_handle,
                                            &indirect_draw,
                                            sizeof(VkDrawIndexedIndirectCommand));

        //per object draw data
        mesh_draw_data.transform_idx = current_submesh->transform_handle.handle;
        mesh_draw_data.material_instance_handle = current_submesh->material_handle.handle;

        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->draw_data_staging_buffer_handle,
                                           &mesh_draw_data,
                                           sizeof(Mesh_Draw_Data));


    }


    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->indirect_buffer_handle,
                                    mesh_renderer->indirect_staging_buffer_handle, command_buffer);

    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->draw_data_buffer_handle,
                                mesh_renderer->draw_data_staging_buffer_handle, command_buffer);




}


void mesh_renderer_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer, vulkan_command_buffer* command_buffer,
                        vulkan_shader_pipeline* pipeline)
{
    INFO("MESH SYSTEM DRAW CALLS")

    //only bind the vertex and index, the storage buffers are in bindless
    Vulkan_Buffer* indirect_buffer = vulkan_buffer_get(renderer, mesh_renderer->indirect_buffer_handle);
    Vulkan_Buffer* vertex_buffer = vulkan_buffer_get(renderer, mesh_renderer->vertex_buffer_handle);
    Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);


    VkDeviceSize pOffsets[] = {0};


    //UBER SHADER MESH INDIRECT DRAW
    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->indirect_mesh_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->indirect_mesh_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //texturess
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->indirect_mesh_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->indirect_mesh_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
                           pOffsets);

    vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                         VK_INDEX_TYPE_UINT16);


    VkPushConstantsInfo push_constant_info = {0};
    push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info.layout = pipeline->pipeline_layout;
    push_constant_info.offset = 0;
    push_constant_info.size = sizeof(PC_Mesh);
    // push_constant_info.size = sizeof(mesh_system->pc_mesh); // make sure its not a pointer if i use this
    push_constant_info.pValues = &mesh_renderer->pc_mesh;
    push_constant_info.pNext = NULL;
    vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


    //no need to sort by permutation rn, it gets handled in the shader, we can always sort out this later
    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 indirect_buffer->handle, 0,
                                 mesh_renderer->indirect_draw_count,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < mesh_renderer->indirect_draw_count; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }
}
