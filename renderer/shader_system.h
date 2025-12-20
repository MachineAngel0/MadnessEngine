#ifndef SHADER_SYSTEM_H
#define SHADER_SYSTEM_H

#include "vulkan_types.h"

//TODO: increase later when it becomes relevant
#define shader_system_texture_amount 100;


/*typedef struct shader_system
{
    Texture error_texture;
    Texture* textures;
    Material* material_references;
    vulkan_shader_pipeline* pipeline_referenes;
}shader_system;*/


void shader_system_init(vulkan_context* context, shader_system* out_system)
{
    create_texture_image(context, context->graphics_command_buffer, "../renderer/texture/error_texture.png",
                         &out_system->error_texture);

    out_system->available_texture_indexes = 0;
    out_system->material_indexes = 0;
}

//pass out the texture index
u32 shader_system_add_texture(vulkan_context* context, shader_system* system, char const* filepath)
{
    system->available_texture_indexes++;
    u32 out_texture_index = system->available_texture_indexes;

    Texture* out_texture = system->textures[out_texture_index];
    create_texture_image(context, context->graphics_command_buffer, filepath, out_texture);

    return out_texture_index;
}

void shader_system_remove_texture(shader_system* system)
{
}

void shader_system_add_material(shader_system* system)
{
}

void shader_system_remove_material(shader_system* system)
{
}


#endif //SHADER_SYSTEM_H
