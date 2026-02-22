#ifndef SPRITE_H
#define SPRITE_H


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
    {{-0.5f, 0.5f}, {1.0f, 1.0f}}
};


u16 default_quad_indices[6] = {
    0, 1, 2, 2, 3, 0
};

//for drawing

Quad_Vertex* quad_create(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color);
Quad_Vertex* quad_create_screen_size(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, vec2 screen_size);

Quad_Vertex* quad_create_screen_percentage(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color);

Quad_Texture* quad_create_textured(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, vec2 uv0, vec2 uv1);

#endif //SPRITE_H