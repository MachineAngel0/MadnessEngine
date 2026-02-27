#include "sprite.h"

#include "arena.h"
#include "maths/math_types.h"


Quad_Vertex* quad_create(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color)
{
    Quad_Vertex* out_vertex = arena_alloc(frame_arena, sizeof(Quad_Vertex) * 4);

    out_vertex[0] = (Quad_Vertex){.pos = {pos.x, pos.y}, .color = color};
    out_vertex[1] = (Quad_Vertex){.pos = {pos.x, pos.y + size.y}, .color = color};
    out_vertex[2] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y + size.y}, .color = color};
    out_vertex[3] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y}, .color = color};

    return out_vertex;
}

Quad_Vertex* quad_create_screen_size(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, vec2 screen_size)
{
    Quad_Vertex* out_vertex = arena_alloc(frame_arena, sizeof(Quad_Vertex) * 4);

    vec2 f_pos = vec2_div(pos, screen_size);
    vec2 f_size = vec2_div(size, screen_size);

    out_vertex[0] = (Quad_Vertex){.pos = {f_pos.x, f_pos.y}, .color = color};
    out_vertex[1] = (Quad_Vertex){.pos = {f_pos.x, f_pos.y + f_size.y}, .color = color};
    out_vertex[2] = (Quad_Vertex){.pos = {f_pos.x + f_size.x, f_pos.y + f_size.y}, .color = color};
    out_vertex[3] = (Quad_Vertex){.pos = {f_pos.x + f_size.x, f_pos.y}, .color = color};

    return out_vertex;
}

Quad_Vertex* quad_create_screen_percentage(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color)
{
    Quad_Vertex* out_vertex = arena_alloc(frame_arena, sizeof(Quad_Vertex) * 4);

    out_vertex[0] = (Quad_Vertex){.pos = {pos.x - size.x, pos.y - size.y}, .color = color};
    out_vertex[1] = (Quad_Vertex){.pos = {pos.x - size.x, pos.y + size.y}, .color = color};
    out_vertex[2] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y + size.y}, .color = color};
    out_vertex[3] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y - size.y}, .color = color};

    return out_vertex;
}

Quad_Texture* quad_create_textured(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, vec2 uv0, vec2 uv1)
{
    Quad_Texture* out_vertex = arena_alloc(frame_arena, sizeof(Quad_Texture) * 4);

    out_vertex[0] = (Quad_Texture){.pos = {pos.x - size.x, pos.y - size.y}, .color = color, .tex = {uv0.x, uv0.y}};
    out_vertex[1] = (Quad_Texture){.pos = {pos.x - size.x, pos.y + size.y}, .color = color, .tex = {uv0.x, uv1.y}};
    out_vertex[2] = (Quad_Texture){.pos = {pos.x + size.x, pos.y + size.y}, .color = color, .tex = {uv1.x, uv1.y}};
    out_vertex[3] = (Quad_Texture){.pos = {pos.x + size.x, pos.y - size.y}, .color = color, .tex = {uv1.x, uv0.y}};

    return out_vertex;
}



void sprite_system_init(renderer* renderer)
{
    sprite_system = arena_alloc(&renderer->arena, sizeof(Sprite_System));
    sprite_system->frame_arena = &renderer->frame_arena;
    sprite_system->index_type = VK_INDEX_TYPE_UINT16;

    //create one sprite, that will get transformed based on the instance
    //we also keep its pivot at the center
    memcpy(sprite_system->sprites, default_sprite, sizeof(default_sprite));
    memcpy(sprite_system->sprite_indices, default_sprite_indices, sizeof(default_sprite_indices));


    sprite_system->sprites_instance_data = Sprite_Instance_Data_array_create(MAX_SPRITE_COUNT);
    //TODO: replace with a fill function later
    Sprite_Instance_Data instance_data = {0};
    for (u64 i = 0; i < sprite_system->sprites_instance_data->capacity; i++)
    {
        sprite_system->sprites_instance_data->data[i] = instance_data;
    }


    //TODO: move out the memory capacity to the function params
    u64 memory_capacity = MB(1);

    sprite_system->sprite_vertex_buffer = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_VERTEX,
                                                               memory_capacity);

    sprite_system->sprite_index_buffer = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                              memory_capacity);
    sprite_system->sprite_indirect_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                 BUFFER_TYPE_INDIRECT, memory_capacity);

    sprite_system->sprite_vertex_staging_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_STAGING, memory_capacity);
    sprite_system->sprite_index_staging_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                      BUFFER_TYPE_STAGING, memory_capacity);
    sprite_system->sprite_indirect_staging_buffer = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING, memory_capacity);
}

void sprite_begin(const i32 screen_size_x, const i32 screen_size_y)
{
    sprite_system->screen_size.x = (float)screen_size_x;
    sprite_system->screen_size.y = (float)screen_size_y;

    Sprite_Instance_Data_array_clear(sprite_system->sprites_instance_data);
}


void sprite_upload_draw_data(renderer* renderer)
{
    vulkan_buffer_data_copy_and_upload(renderer, sprite_system->sprite_vertex_buffer,
                                       sprite_system->sprite_vertex_staging_buffer,
                                       &sprite_system->sprites, sizeof(Sprite) * 4);

    vulkan_buffer_data_copy_and_upload(renderer, sprite_system->sprite_index_buffer,
                                       sprite_system->sprite_index_staging_buffer,
                                       sprite_system->sprite_indices,
                                       sizeof(u16) * 6);

    vulkan_buffer_data_copy_and_upload(renderer, sprite_system->sprite_instance_buffer,
                                       sprite_system->sprite_instance_staging_buffer,
                                       sprite_system->sprites_instance_data->data,
                                       Sprite_Instance_Data_array_get_bytes_used(sprite_system->sprites_instance_data));


    //generate indirect draw data
    //basically just a bunch of instances with indexes into the instance data buffer

    //literally only need one
    VkDrawIndexedIndirectCommand sprite_indirect_draw;
    u64 sprite_count = sprite_system->sprites_instance_data->num_items;

    sprite_indirect_draw.firstIndex = 0;
    sprite_indirect_draw.firstInstance = 0;
    sprite_indirect_draw.vertexOffset = 6; // one quad is 2 triangles / 6 vertex's
    sprite_indirect_draw.indexCount = sizeof(default_quad_indices);
    sprite_indirect_draw.instanceCount = sprite_count;


    vulkan_buffer_data_copy_and_upload(renderer, sprite_system->sprite_indirect_buffer,
                                       sprite_system->sprite_indirect_staging_buffer,
                                       &sprite_indirect_draw,
                                       sizeof(VkDrawIndexedIndirectCommand));
}

void sprite_draw(renderer* renderer, vulkan_command_buffer* command_buffer)
{

    vulkan_buffer* vert_buffer = vulkan_buffer_get(renderer, sprite_system->sprite_vertex_buffer);
    vulkan_buffer* index_buffer = vulkan_buffer_get(renderer, sprite_system->sprite_index_buffer);
    vulkan_buffer* indirect_buffer = vulkan_buffer_get(renderer, sprite_system->sprite_indirect_buffer);

    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->sprite_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    VkDeviceSize offsets_bindless[1] = {0};
    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1,
                           &vert_buffer->handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer->handle,
                         index_buffer->handle, 0,
                         sprite_system->index_type
    );

    u64 sprite_count = sprite_system->sprites_instance_data->num_items;

    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 indirect_buffer->handle, 0,
                                 sprite_count,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < sprite_count; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }

}


//sprites will only last for the frame, modify this is for another time
void sprite_create(vec2 pos, vec2 size, vec3 color, Texture_Handle texture,
                         Sprite_Pipeline_Flags material_flags)
{
    Sprite_Instance_Data* data = &sprite_system->sprites_instance_data->data[sprite_system->sprites_instance_data->
        num_items];
    data->flags = material_flags;
    data->pos = pos;
    data->scale = size;
    data->color = color;
    data->texture_index = texture.handle;
}




