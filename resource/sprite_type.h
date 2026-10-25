#ifndef VULKAN_TYPES_VERTEX_H
#define VULKAN_TYPES_VERTEX_H

#include "../core/dsa/array.h"
#include "maths/math_types.h"

typedef struct Sprite
{
    vec2s pos;
    vec2s tex;
} Sprite;

typedef enum Sprite_Flags
{
    SPRITE_FLAG_NONE = BITFLAG(0),
    SPRITE_FLAG_TEXTURE = BITFLAG(1),
    SPRITE_FLAG_COLOR = BITFLAG(2),
} Sprite_Flags;

typedef struct Sprite_Data
{
    //will be in a per instance storage buffer
    Sprite_Flags flags;

    //will be used for text and for anything else 2d that needs a texture
    // Transform transform; //TODO: when i make this more robust
    vec4s pos;
    vec2s size;

    float rotation; // a float since we only rotate on one dimension, we can always change it later




    // offset into a texture atlas if using one, otherwise {0, 0}
    vec2s uv_offset;
    // start from offset and this will give us our bottom right uv, which tells us all the other info we need
    vec2s uv_size;

    //material data here
    vec3s color;
    u32 texture_index;
} Sprite_Data;



Sprite default_sprite[4] = {
    {.pos = {-1.f, -1.f}, .tex = {0.0f, 0.0f}},  // top-left
    {.pos = {-1.f,  1.f}, .tex = {0.0f, 1.0f}},  // bottom-left
    {.pos = { 1.f,  1.f}, .tex = {1.0f, 1.0f}},  // bottom-right
    {.pos = { 1.f, -1.f}, .tex = {1.0f, 0.0f}},  // top-right
};

u16 default_sprite_indices[6] = {
    0, 1, 2, 2, 3, 0
};


#endif //VULKAN_TYPES_VERTEX_H
