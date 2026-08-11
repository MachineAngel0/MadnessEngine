#ifndef RESOURCE_TYPES2_H
#define RESOURCE_TYPES2_H

//here cause stupid header referencing

#include "resource_types.h"
#include "renderer.h"

typedef struct Asset_System
{
    //the asset system is just a container for all the system,
    //gather the cpu-gpu resources and send them to renderer

    //Systems
    // TODO: might change this into a pool allocator, or even segregated list allocator
    Heap_Allocator* heap_allocator;
    Frame_Allocator* frame_allocator;

    Renderer* renderer;

    // Shader_System* shader_system;
    // Material_System* shader_system; //probably want a material system, but not a shader system here, but in the renderer
    Sprite_System* sprite_system;
    Mesh_System* mesh_system;
    Texture_System* texture_system;
    Material_System* material_system;
    Scene* scene;
    Animation_System* animation_system;

    Particle_System* particle_system;

    //Render Packet
    Render_Packet* render_packet;

    Asset_Registry* asset_registry;
} Asset_System;


#endif //RESOURCE_TYPES2_H
