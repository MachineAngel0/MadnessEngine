#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

//TODO:
#include "maths/math_types.h"


typedef struct debug_line_material
{
    vec3 start;
    vec3 end;
    vec4 color;
    u32 line_width;
} debug_line_material;

typedef struct debug_sphere_material
{
    vec3 point;
    float radius;
    vec4 color;
    u32 line_width;
} debug_sphere_material;

typedef struct debug_box_material
{
    vec3 point;
    float size;
    vec4 color;
    u32 line_width;
} debug_box_material;


typedef struct debug_box_bound_material
{
    vec3 point;
    float width;
    float height;
    float length;
    vec4 color;
    u32 line_width;
} debug_box_bound_material;


//NOTE: dynamic state we can use to set line widths
// vkCmdSetLineWidth(command_buffer, 3.0f);
//NOTE: these are good canidates for instancing, since all that changes is the material and transforms

//this is very much an immediate more style api
//TODO: add persistance, to see whats happening over multiple frames
// (and use a pool, replacing the oldest, just go around like a ring queue, but with an array)

void debug_draw_line(vec3 start, vec3 end, vec4 color, u32 line_width)
{
}

void debug_draw_sphere(vec3 point, float radius, vec4 color, u32 line_width)
{
}

void debug_draw_box(vec3 point, float size, vec4 color, u32 line_width)
{
}

void debug_draw_box_bound(vec3 point, float width, float height, float length, vec4 color, u32 line_width)
{
}


/* TODO: support these at another time, the ones above are generally more essential
void debug_draw_axis(){}

void debug_draw_text2d(){}
void debug_draw_text3d(){}
*/


#endif //DEBUG_DRAW_H
