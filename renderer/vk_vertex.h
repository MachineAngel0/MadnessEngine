#ifndef VK_VERTEX_H
#define VK_VERTEX_H

#include <stdalign.h>



//NOTE: its not that each member needs to be aligned,
// its that the whole struct needs to be aligned to a power of 16 (on my system at least)
typedef struct vertex_3d
{
     vec3 position;
     vec3 color;
     vec2 padding;
} vertex_3d;

/* same thing, as above
typedef struct vertex_3d
{
    alignas(16) vec3 position;
    alignas(16) vec3 color;
} vertex; */

typedef struct vertex_tex
{
    // alignas(16) vec3 position;
    // alignas(16) vec3 color;
    // alignas(16) vec2 texture;
    vec3 position;
    vec3 color;
    vec2 texture;
} vertex_tex;


// const vertex_3d test_vertices[] = {
//     {{-0.5f, -0.5f, 0.0f}},
//     {{0.5f, -0.5f, 0.0f}},
//     {{0.5f, 0.5f, 0.0f}},
//     {{-0.5f, 0.5f, 0.0f}},
//
// };
const vertex_3d test_vertices[] = {
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}
};

const vertex_tex texture_vertices[] = {
    {{1.0f + 2.0f, 1.0f + 2.0f, 0.0f + 2.0f}, {1.0f, 0.0f, 0.0f},},
    {{-1.0f + 2.0f, 1.0f + 2.0f, 0.0f + 2.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.0f + 2.0f, -1.0f + 2.0f, 0.0f + 2.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}}
};


//for a square
// const uint16_t test_indices[] = {
// 0, 1, 2, 2, 3, 0,
// };
const uint16_t test_indices[] = {
    0, 1, 2
};

typedef struct uniform_buffer_object
{
    alignas(16) mat4 model;
    alignas(16) mat4 view;
    alignas(16) mat4 proj;
} uniform_buffer_object;

//TODO: global uniform buffer
typedef struct uniform_buffer_object_new
{
    mat4 view;
    mat4 proj;

    VkDeviceAddress view_buffer;
    VkDeviceAddress projection_buffer;
    VkDeviceAddress directional_light_buffer;
    VkDeviceAddress spot_light_buffer;

    //camera camera
     // vec4 cameraPos;

    //Shadow cameras

    //TODO: should probably be an enum
    // regular, normal, lighting, shadows, lights, etc...
    u32 debug_mode;


    //Shadow cameras

    //TODO: should probably be an enum
    // regular, normal, lighting, shadows, lights, etc...
    u32 debug_mode;


} uniform_buffer_object_new;


//TODO: PUSH CONSTANTS
typedef struct pc_globals
{
    u32 projection_view_index;
    u32 camera_something;
    u32 vertex_offset;
    u32 vertex_size;
}pc_globals;

typedef struct pc_vertex
{
    mat4 model;
    u32 vertex_offset;
    u32 vertex_size;
}pc_vertex;


typedef struct pc_material
{
    vec3 color;
    float emissive;
    float roughness;
    float metallic;

}pc_material;

#endif //VK_VERTEX_H
