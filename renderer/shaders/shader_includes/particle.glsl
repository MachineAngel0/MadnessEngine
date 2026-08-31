#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "macros.glsl"


layout(buffer_reference, std430) readonly buffer Particle_PosX_Buffer{
    float data[];
};
layout(buffer_reference, std430) readonly buffer Particle_PosY_Buffer{
    float data[];
};
layout(buffer_reference, std430) readonly buffer Particle_PosZ_Buffer{
    float data[];
};

layout(buffer_reference, std430) readonly buffer Particle_RotX_Buffer{
    float data[];
};

layout(buffer_reference, std430) readonly buffer Particle_RotY_Buffer{
    float data[];
};

layout(buffer_reference, std430) readonly buffer Particle_SizeX_Buffer{
    float data[];
};
layout(buffer_reference, std430) readonly buffer Particle_SizeY_Buffer{
    float data[];
};


layout(buffer_reference, std430) readonly buffer Particle_Color_Buffer{
    vec4 data[];
};


struct PC_Particle{
    //particle buffers
    Particle_PosX_Buffer particle_posx_buffer;
    Particle_PosY_Buffer particle_posy_buffer;
    Particle_PosZ_Buffer particle_posz_buffer;
    Particle_Rot_Buffer particle_rotx_buffer;
    Particle_Rot_Buffer particle_roty_buffer;
    Particle_Size_Buffer particle_sizex_buffer;
    Particle_Size_Buffer particle_sizey_buffer;
    Particle_Color_Buffer particle_color_buffer; // 9 * 8 = 72 (we chillin)
    //material buffers
    Spherical_Billboard_Buffer material_buffer;
}pc;