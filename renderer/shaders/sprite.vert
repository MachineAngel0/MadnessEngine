#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : enable

#include "shader_includes/sprite_structs.glsl"
#include "shader_includes/test_uniform.glsl"

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec2 out_tex;
layout(location = 3) out flat uint out_flags;
layout(location = 4) out flat uint instance_index;



const vec2 corners[4] = vec2[](
        vec2(-1.0, -1.0), // top left
        vec2(1.0, -1.0), // top right
        vec2(1.0, 1.0), // bottom right
        vec2(-1.0, 1.0)// bottom left
);

const vec2 const_uvs[4] = vec2[](
        vec2(0.0, 1.0), // top-left
        vec2(1.0, 1.0), // top-right
        vec2(1.0, 0.0), // bottom-right
        vec2(0.0, 0.0)// bottom-left
);

const int indices[6] = int[6](0, 1, 2, 2, 3, 0);


//NOTE: spherical billboard sprites
void main() {

    uint instance_idx = gl_InstanceIndex;
    instance_index = instance_idx;

    Sprite sprite_data = push_sprite.sprite_ssbo.data[instance_idx];

    //    vec2 scaled = in_pos * inst_data.scale;
    //    vec2 rotated = rotate(scaled, inst_data.rotation);
    //    vec2 world_position =  inst_data.translation + rotated;

    // gl_Position = camera.viewProj * vec4(worldPos, 0.0, 1.0);
    //    gl_Position = vec4(world_position, 0.0, 1.0);

    float half_size_x = sprite_data.size.x * 0.5;
    float half_size_y = sprite_data.size.y * 0.5;

    int idx = indices[gl_VertexIndex];

    vec2 corner = corners[idx];
    out_uv = const_uvs[idx];

    //    vec3 cameraRight = vec3(ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]);
    vec3 world_up = vec3(ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]);

    vec3 to_camera = normalize(ubo.camera_position.xyz - sprite_data.pos.xyz);

    vec3 vector_right = normalize(cross(world_up, to_camera));// right
    vec3 vector_up = normalize(cross(to_camera, vector_right));// up

    float c = cos(sprite_data.rotation.x);
    float s = sin(sprite_data.rotation.x);

    vec3 rotated_right = vector_right * c + vector_up * s;
    vec3 rotated_up = -vector_right * s + vector_up * c;

    vec3 world_pos =
    sprite_data.pos.xyz +
    rotated_right * corner.x * half_size_x +
    rotated_up * corner.y * half_size_y;


    gl_Position = ubo.proj * ubo.view * vec4(world_pos, 1.0);


    out_color = vec3(1.f, 1.f, 1.f); // default color




    out_flags = sprite_data.flags;

    if ((sprite_data.flags & SPRITE_FLAG_COLOR) != 0u){
        out_color = sprite_data.color;

    }


    if ((sprite_data.flags & SPRITE_FLAG_TEXTURE) != 0u){

        float left = sprite_data.uv_offset.x;
        float top = sprite_data.uv_offset.y;
        float right = sprite_data.uv_offset.x + sprite_data.uv_size.x;
        float bottom = sprite_data.uv_offset.y + sprite_data.uv_size.y;

        vec2 uvs[4]=
        {
        vec2(left, top),
        vec2(left, bottom),
        vec2(right, bottom),
        vec2(right, top),
        };

        out_uv = uvs[idx];
    }

}