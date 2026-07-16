#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : enable



#include "shader_includes/test_uniform.glsl"

//https://www.flipcode.com/archives/Billboarding-Excerpt_From_iReal-Time_Renderingi_2E.shtml



struct Spherical_Billboard{
    vec3 position;
    uint texture_idx;
    vec2 size;//also a radius
    vec2 rotation;// should be one rotation in radians
//    vec4 color; //tint basically
};

layout(buffer_reference, scalar) readonly buffer Spherical_Billboard_Buffer{
    Spherical_Billboard data[];
};

struct PC_Particle{
    Spherical_Billboard_Buffer material_buffer;
    Spherical_Billboard_Buffer unused;
};

layout(push_constant, scalar) uniform PC_PARTICLE{
    PC_Particle pc;
};

const vec2 corners[4] = vec2[](
vec2(-1.0, -1.0), // top left
vec2(1.0, -1.0), // top right
vec2(1.0, 1.0), // bottom right
vec2(-1.0, 1.0)// bottom left
);

const vec2 uvs[4] = vec2[](
vec2(0.0, 1.0), // top-left
vec2(1.0, 1.0), // top-right
vec2(1.0, 0.0), // bottom-right
vec2(0.0, 0.0)// bottom-left
);

//we are manually generating the index and vertex data, since its always the same just with offsets and positions
int indices[6] = int[6](0, 1, 2, 2, 3, 0);


layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out flat uint out_texture_idx;




void main() {

    uint instance_idx = gl_InstanceIndex;
    Spherical_Billboard billboard_data = pc.material_buffer.data[instance_idx];
    out_texture_idx = billboard_data.texture_idx;

    out_color = vec3(1.0, 0, 0);

    float half_size_x = billboard_data.size.x * 0.5;
    float half_size_y = billboard_data.size.y * 0.5;


    int idx = indices[gl_VertexIndex];

    vec2 corner = corners[idx];
    out_uv = uvs[idx];



    //    vec3 cameraRight = vec3(ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]);
    vec3 world_up    = vec3(ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]);

    vec3 to_camera = normalize(ubo.camera_position.xyz - billboard_data.position.xyz);

    vec3 vector_right = normalize(cross(world_up, to_camera));// right
    vec3 vector_up = normalize(cross(to_camera, vector_right));// up

    float c = cos(billboard_data.rotation.x);
    float s = sin(billboard_data.rotation.x);

    vec3 rotated_right = vector_right * c + vector_up * s;
    vec3 rotated_up = -vector_right * s + vector_up * c;

    vec3 world_pos =
    billboard_data.position +
    rotated_right * corner.x * half_size_x +
    rotated_up * corner.y * half_size_y;

    /*    vec3 world_pos =
        billboard.point
        + cameraRight * corner.x * billboard.size
        + cameraUp    * corner.y * billboard.size;*/

    gl_Position = ubo.proj * ubo.view * vec4(world_pos, 1.0);


    //spherical billboards are meant to always be looking at the camera, not matter the angle or axis

    //for generating the vertex's
    // p = point
    // r = radius (or size)
    // rx = half width, ry = half height
    // x = M-object(inverse), M-Camera[0]
    // y = M-object(inverse), M-Camera[1]

    // v0 = p - rxX + ryY
    // v1 = p + rxX + ryY
    // v2 = p + rxX - ryY
    // v3 = p - rxX - ryY


    //properly orienting the camera in z space
    // cam_position = Mobject(inv)*Mcamera[3]

    //normal
    // n = nrm(c-p)
    //tangent
    // a = nrm(n*y)

    // v0 = p - rxa - ryb
    // v1 = p + rxa - ryb
    // v2 = p + rxa + ryb
    // v3 = p - rxa + ryb

    //including rotations

    // v0 = p - (rx cos() - ry sin() )a - (ry cos() + rx sin() ) b
    // v1 = p + (rx cos() + ry sin() )a - (ry cos() - rx sin() ) b
    // v2 = p + (rx cos() - ry sin() )a + (ry cos() + rx sin() ) b
    // v3 = p - (rx cos() + ry sin() )a + (ry cos() - rx sin() ) b


}