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
