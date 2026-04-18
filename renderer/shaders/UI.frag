#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"


layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec2 in_local_uv;
layout(location = 3) in flat uint in_texture_idx;
layout(location = 4) in flat uint in_sprite_buffer_location;


layout(location = 0) out vec4 outColor;

void main() {
    uint sprite_instance_buffer_idx = PC_2D.instance_buffer_idx;

    vec2 screen_dimensions = ubo[nonuniformEXT(PC_2D.ubo_buffer_idx)].screen_dimensions;
    float aspect_ratio = screen_dimensions.x / screen_dimensions.y;

    Sprite_Data inst_data =
    Sprite_Instance_Buffer[nonuniformEXT(sprite_instance_buffer_idx)].sprite_instance_data[nonuniformEXT(in_sprite_buffer_location)];
    outColor = vec4(in_color, 1.0);


    if ((inst_data.flags & SPRITE_FLAG_CIRCLE) != 0u){

        float outline_thickness = inst_data.thickness;

        //    vec2 screen_dimensions = ubo[nonuniformEXT(ubo_index)].screen_dimension; // TODO:

        // Normalized pixel coordinates (from 0 to 1)
        vec2 local_position = in_local_uv * 2.0 - 1.0;
        local_position.x *= aspect_ratio;

        float distance = 1.0 - length(local_position);

        const float edge_fade = 0.005; // smooths the edge

        float circleAlpha = smoothstep(0, edge_fade, distance);
        circleAlpha *= smoothstep(outline_thickness + edge_fade, outline_thickness, distance);

        outColor = vec4(in_color, circleAlpha);

        //    outColor = vec4(in_color * circle, 1.0); * texture(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv); // if we want colors overlayed
    }

}