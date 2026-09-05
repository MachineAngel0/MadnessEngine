#ifndef SHADER_SYSTEM_H
#define SHADER_SYSTEM_H


//TODO: increase later when it becomes relevant
#define SHADER_SYSTEM_CAPACITY 100lu
#include "vulkan_struct_types.h"


Shader_System* shader_system_init(Renderer* renderer);


void shader_system_shutdown(Shader_System* system);


void shader_system_update(Renderer* renderer, Shader_System* shader_system, Render_Packet* render_packet);


//Shader Batch system

void shader_system_shader_batch_create(Renderer* renderer, Shader_System* shader_system,
    Material_Batch* material_batch, Material_Asset* material_asset, Material_Definition* material_definition);

void shader_system_shader_batch_free(Renderer* renderer, Shader_System* shader_system, const char* shader_name);


#endif //SHADER_SYSTEM_H
