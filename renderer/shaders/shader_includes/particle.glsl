#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "macros.glsl"


layout(buffer_reference, std430) readonly buffer Particle_PosX_Buffer{
    float pos_x[];
};
layout(buffer_reference, std430) readonly buffer Particle_PosY_Buffer{
    float pos_y[];
};
layout(buffer_reference, std430) readonly buffer Particle_PosZ_Buffer{
    float pos_z[];
};

layout(buffer_reference, std430) readonly buffer Particle_RotX_Buffer{
    float rot_x[];
};

layout(buffer_reference, std430) readonly buffer Particle_RotY_Buffer{
    float rot_y[];
};

layout(buffer_reference, std430) readonly buffer Particle_ScaleX_Buffer{
    float scale_x[];
};
layout(buffer_reference, std430) readonly buffer Particle_ScaleY_Buffer{
    float scale_y[];
};


layout(buffer_reference, std430) readonly buffer Particle_Color_Buffer{
    vec4 data[];
};


struct PC_Particle{
    //particle buffers
    Particle_PosX_Buffer particle_posx_buffer;
    Particle_PosY_Buffer particle_posy_buffer;
    Particle_PosZ_Buffer particle_posz_buffer;
    Particle_RotX_Buffer particle_rotx_buffer;
    Particle_RotY_Buffer particle_roty_buffer;
    Particle_ScaleX_Buffer particle_sizex_buffer;
    Particle_ScaleY_Buffer particle_sizey_buffer;
    Particle_Color_Buffer particle_color_buffer; // 9 * 8 = 72 (we chillin)
    //material buffers
    Spherical_Billboard_Buffer material_buffer;
}pc;