#ifndef SPRITE_H
#define SPRITE_H


typedef enum Sprite_Pipeline_Flags
{
    SPRITE_PIPELINE_NONE = BITFLAG(1),
    SPRITE_PIPELINE_TEXTURE = BITFLAG(2),
    SPRITE_PIPELINE_COLOR = BITFLAG(3),
} Sprite_Pipeline_Flags;


//TODO: change into vec3's
typedef struct Quad_Vertex
{
    vec2 pos;
    vec3 color;
} Quad_Vertex;

typedef struct Quad_Texture
{
    //will be used for text and for anything else 2d that needs a texture
    vec2 pos;
    vec3 color;
    vec2 tex;
} Quad_Texture;


//auto has the default uv's
Quad_Texture default_quad_texture_vertex[4] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f}},
};


u16 default_quad_indices[6] = {
    0, 1, 2, 2, 3, 0
};

typedef struct Sprite_Handle
{
    u32 handle;
} Sprite_Handle;

typedef struct Sprite
{
    vec2 pos;
    vec2 tex;
} Sprite;

Sprite default_sprite[4] = {
    {.pos = {-0.5f, -0.5f}, .tex = {0, 1}},
    {.pos = {0.5f, -0.5f}, .tex = {1, 1}},
    {.pos = {0.5f, 0.5f}, .tex = {1, 0}},
    {.pos = {-0.5f, 0.5f}, .tex = {0, 0}},
};

u16 default_sprite_indices[6] = {
    0, 1, 2, 2, 3, 0
};


typedef struct Sprite_Instance_Data
{
    //will be in a per instance storage buffer

    u32 flags;

    //will be used for text and for anything else 2d that needs a texture
    // Transform transform; //TODO: when i make this more robust
    vec2 pos;
    float rotation; // a float since we only rotate on one dimension, we can always change it later
    vec2 scale;

    // offset into a texture atlas if using one, otherwise {0, 0}
    vec2 uv_offset;
    // start from offset and this will give us our bottom right uv, which tells us all the other info we need
    vec2 uv_size;

    //material data here
    vec3 color;
    u32 texture_index;
} Sprite_Instance_Data;


//for drawing

Quad_Vertex* quad_create(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color);
Quad_Vertex* quad_create_screen_size(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, vec2 screen_size);

Quad_Vertex* quad_create_screen_percentage(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color);

Quad_Texture* quad_create_textured(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, vec2 uv0, vec2 uv1);


ARRAY_GENERATE_TYPE(Quad_Vertex)
ARRAY_GENERATE_TYPE(Quad_Texture)

ARRAY_GENERATE_TYPE(Sprite_Instance_Data)

#define MAX_SPRITE_COUNT 100

typedef struct Sprite_System
{
    Frame_Arena* frame_arena;
    vec2 screen_size; // grab every frame on start


    Sprite sprites[4]; // literally just need one quad for a vertex buffer
    Sprite_Instance_Data_array* sprites_instance_data;
    u16 sprite_indices[6];

    VkIndexType index_type;


    Buffer_Handle sprite_vertex_buffer;
    Buffer_Handle sprite_index_buffer;
    Buffer_Handle sprite_indirect_buffer;
    Buffer_Handle sprite_instance_buffer;

    Buffer_Handle sprite_vertex_staging_buffer;
    Buffer_Handle sprite_index_staging_buffer;
    Buffer_Handle sprite_instance_staging_buffer;
    Buffer_Handle sprite_indirect_staging_buffer;
} Sprite_System;

void sprite_system_init(Sprite_System* sprite_system, renderer* renderer);

void sprite_begin(Sprite_System* sprite_system,i32 screen_size_x, i32 screen_size_y);

void sprite_upload_draw_data(Sprite_System* sprite_system,renderer* renderer);

void sprite_draw(Sprite_System* sprite_system,renderer* renderer, vulkan_command_buffer* command_buffer);


//sprites will only last for the frame, modify this is for another time
void sprite_create(Sprite_System* sprite_system, vec2 pos, vec2 size, vec3 color, Texture_Handle texture,
                         Sprite_Pipeline_Flags material_flags);

#endif //SPRITE_H
