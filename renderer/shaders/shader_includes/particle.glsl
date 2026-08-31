#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "macros.glsl"


struct Particle_2D{
    float pos_x;
    float pos_y;
    float pos_z;

    uint padding;

    vec2 size;//also a radius
    vec2 rotation;// should be one rotation in radians
    vec4 color; //tint basically
};


layout(buffer_reference, std430) readonly buffer Particle_2D_Buffer{
    Particle_2D data[];
};

struct PC_Particle{
    Spherical_Billboard_Buffer material_buffer;
    Particle_2D_Buffer particle_buffer;
    Spherical_Billboard_Buffer unused;
};