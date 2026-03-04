#ifndef SPRITE_H
#define SPRITE_H
#include "vulkan_types.h"


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


Sprite default_sprite[4] = {
    {.pos = {-0.5f, -0.5f}, .tex = {0, 1}},
    {.pos = {0.5f, -0.5f}, .tex = {1, 1}},
    {.pos = {0.5f, 0.5f}, .tex = {1, 0}},
    {.pos = {-0.5f, 0.5f}, .tex = {0, 0}},
};

u16 default_sprite_indices[6] = {
    0, 1, 2, 2, 3, 0
};


//for drawing

Quad_Vertex* quad_create(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color);
Quad_Vertex* quad_create_screen_size(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, vec2 screen_size);

Quad_Vertex* quad_create_screen_percentage(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color);

Quad_Texture* quad_create_textured(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, vec2 uv0, vec2 uv1);


ARRAY_GENERATE_TYPE(Quad_Vertex)
ARRAY_GENERATE_TYPE(Quad_Texture)


#define MAX_SPRITE_COUNT 100


Sprite_System* sprite_system_init(Renderer* renderer);

void sprite_begin(Sprite_System* sprite_system, i32 screen_size_x, i32 screen_size_y);

void sprite_upload_draw_data(Renderer* renderer, Sprite_System* sprite_system);
//TODO:
// void sprite_upload_draw_data_packet(Renderer* renderer, Sprite_System* sprite_system, Renderer_Packet_List* render_packet);

void sprite_draw(Sprite_System* sprite_system, Renderer* renderer, vulkan_command_buffer* command_buffer);


//sprites will only last for the frame, modify this is for another time
void sprite_create(Sprite_System* sprite_system, vec2 pos, vec2 size, vec3 color, Texture_Handle texture,
                   Sprite_Pipeline_Flags material_flags);

Sprite_Data* sprite_create_new(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, Texture_Handle texture,
                               Sprite_Pipeline_Flags material_flags);

#endif //SPRITE_H
