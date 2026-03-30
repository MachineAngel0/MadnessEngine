#ifndef MADNESSENGINE_DEBUG_DRAW_H
#define MADNESSENGINE_DEBUG_DRAW_H

//TODO:
#include "maths/math_types.h"

struct debug_draw_info
{
    float width;
    float size; // like sphere size/ radius, bounding box size
    vec3 color;
};


void debug_draw_line();
void debug_draw_sphere();
void debug_draw_point();
void debug_draw_box();
void debug_draw_axis();

void debug_draw_text2d();
void debug_draw_text3d();



#endif //MADNESSENGINE_DEBUG_DRAW_H