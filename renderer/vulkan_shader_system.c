#include "vulkan_shader_system.h"

#include "logger.h"
#include "vk_command_buffer.h"


Vulkan_Shader_System* vulkan_shader_system_init(Renderer* renderer)
{
    Vulkan_Shader_System* shader_system = allocator_alloc(&renderer->allocator, sizeof(Vulkan_Shader_System));
    renderer->shader_system = shader_system;

    // create_texture_image(&renderer->context, renderer->context.graphics_command_buffer,
    //                      "../renderer/texture/error_texture.png",
    //                      &(*out_shader_system)->error_texture);

    shader_system->pipeline_indexes = 0;


    //we store the pointer, we dont want a copy
    shader_system->shader_batch_hash_set = hash_set_init(sizeof(Shader_Sort_Key), 100);


    INFO("SHADER SYSTEM CREATED");


    return shader_system;
}


void vulkan_shader_system_shutdown(Vulkan_Shader_System* system)
{
    //TODO: create and hookup to the renderer shutdown
}


void vulkan_shader_system_update(Renderer* renderer, Vulkan_Shader_System* shader_system, Render_Packet* render_packet)
{
    //TODO: ideally load all our pipelines (at least for the level) at the start but keep the material data empty
    //TODO: we should call this at start up once
    for (int i = 0; i < render_packet->shaders_packet.shader_asset_count; ++i)
    {
        if (hash_set_contains(shader_system->shader_batch_hash_set,
                              &render_packet->shaders_packet.shader_assets[i].shader_pipeline_hash))
        {
            continue;
        }
        vulkan_shader_system_shader_create(renderer, shader_system,
                                                 &render_packet->shaders_packet.shader_assets[i]);
    }



}


void vulkan_shader_system_shader_create(Renderer* renderer, Vulkan_Shader_System* shader_system,
                                              Shader_Asset* shader_asset)
{
    Vulkan_Shader_Batch* shader_batch = &shader_system->shader_batch[shader_system->shader_batch_count++];


    shader_batch->pipeline_hash = shader_asset->shader_pipeline_hash;
    shader_batch->shader_name = string_to_c_string_alloc_heap(shader_asset->shader_pipeline_state.shader_name,
                                                              renderer->heap_allocator);

    shader_batch->shader_pso.blend_mode = shader_asset->shader_pipeline_state.blend_mode;


    //load pipeline from our configs
    //TODO: add more configs when neccessary
    vulkan_pipeline_graphics_create(renderer, shader_batch->shader_name, shader_batch->shader_pso.blend_mode, false,
                                    &shader_batch->pipeline,
                                    &shader_batch->wireframe_pipeline);



    hash_set_insert(shader_system->shader_batch_hash_set, &shader_batch->pipeline_hash);
}

void vulkan_shader_system_shader_free(Renderer* renderer, Vulkan_Shader_System* shader_system,
                                            const char* shader_name)
{
    //TODO:
}



Vulkan_Shader_Batch* vulkan_shader_system_get_by_pipeline_hash(Renderer* renderer, Shader_Pipeline_Hash pipeline_hash)
{
    //TODO: use the hash map
    Vulkan_Shader_System* shader_system = renderer->shader_system;

    for (u32 i = 0; i < shader_system->shader_batch_count; i++)
    {
        if (shader_system->shader_batch[i].pipeline_hash == pipeline_hash)
        {
            return &shader_system->shader_batch[i];
        }
    }

    return false;

}