#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include "darray.h"
#include "vk_buffer.h"
#include "maths/math_types.h"

//TODO: maybe move out of the renderer and just send this as part of a render packet

typedef struct Debug_Line
{
    vec3s start;
    vec3s end;
    vec4s color;
} Debug_Line;

typedef struct PC_Debug_Line
{
    VkDeviceAddress debug_line_ssbo;
    u32 padding0;
    u32 padding1;
} PC_Debug_Line;

#define DEFAULT_DEBUG_DRAW_COUNT 1000

typedef struct Debug_Draw_System
{
    DYNAMIC_ARRAY_TYPE(debug_line)* debug_lines;
    Vulkan_Shader_Pipeline debug_line_pipeline;
    Buffer_Frame_Handle debug_ssbo; // buffer of debug_line
} Debug_Draw_System;

static Debug_Draw_System debug_draw_system;

//NOTE: dynamic state we can use to set line widths
// vkCmdSetLineWidth(command_buffer, 3.0f);
//NOTE: these are good canidates for instancing, since all that changes is the material and transforms

//this is very much an immediate mode style api
//TODO: add persistance, to see whats happening over multiple frames
// (and use a pool, replacing the oldest, just go around like a ring queue, but with an array)
// also remove it in release builds

void debug_draw_system_init(Renderer* renderer, Memory_System* memory_system);

void debug_draw_system_deinit();


vec3s get_perpendicular(const vec3s v);


void debug_draw_line(vec3s start, vec3s end, vec4s color);

void debug_draw_circle(const vec3s point, float radius,
                       const vec3s x_axis, const vec3s y_axis,
                       const vec4s color, int segments);

void debug_draw_hemisphere(const vec3s center, float radius, const vec3s axis,
                           const vec3s perp, const vec4s color, int segments);


void debug_draw_sphere(vec3s point, float radius, vec4s color);

void debug_draw_bounds(vec3s center, const vec3s half_extents,
                       const vec4s color);
void debug_draw_box(vec3s center, const float size,
                    const vec4s color);

void debug_draw_capsule(const vec3s base, const vec3s tip,
                        float radius, const vec4s color);



void debug_system_upload_frame_data(Renderer* renderer);
void debug_system_draw(Renderer* renderer);


/* TODO: support these at another time, the ones above are generally more essential
void debug_draw_axis(){}

void debug_draw_text2d(){}
void debug_draw_text3d(){}
*/


#endif //DEBUG_DRAW_H
