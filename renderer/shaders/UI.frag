#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"


layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec2 in_local_pos;
layout(location = 3) in flat uint in_texture_idx;
layout(location = 4) in flat uint in_sprite_buffer_location;


layout(location = 0) out vec4 outColor;


float sdRoundBox(in vec2 p, in vec2 b, in vec4 r)
{
    r.xy = (p.x>0.0)?r.xy : r.zw;
    r.x  = (p.y>0.0)?r.x  : r.y;
    vec2 q = abs(p)-b+r.x;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r.x;
}

void main() {
    uint sprite_instance_buffer_idx = PC_2D.instance_buffer_idx;

    vec2 screen_dimensions = ubo[nonuniformEXT(PC_2D.ubo_buffer_idx)].screen_dimensions;
    float aspect_ratio = screen_dimensions.x / screen_dimensions.y;

    Sprite_Data inst_data =
    Sprite_Instance_Buffer[nonuniformEXT(sprite_instance_buffer_idx)].sprite_instance_data[nonuniformEXT(in_sprite_buffer_location)];
    outColor = vec4(in_color, 1.0);


    {
        //calculations for rounded rects and for any outlines
        //get screen space position and  size
        vec2 p = (in_local_pos - vec2(0.5)) * 2.0;
        vec2 size = vec2(1.0);

        //TODO: get radius from sprite data
        vec4 radius = vec4(0.2);

        float dist = sdRoundBox(p, size, radius);
        float px = fwidth(dist);

        //TODO: params for outline color and outline thickness
        float outline_thickness = 0.05;
        vec3 outline_color = vec3(0.0);
        vec3 fill_color = in_color;

        //  fill (AA edge)
        float fill_alpha = 1.0 - smoothstep(-px, px, dist/ px);

        //  smooth inner outline band
        // outer edge (near surface)
        float edge_outer = smoothstep(-px, px, dist / px);

        // inner edge
        float edge_inner = smoothstep(-outline_thickness - px, -outline_thickness + px, dist);

        // band = region between both edges
        float outline_alpha = edge_inner - edge_outer;

        // clamp just to be safe
        outline_alpha = clamp(outline_alpha, 0.0, 1.0);

        //  combine
        vec3 color = mix(fill_color, outline_color, outline_alpha);
        float alpha = fill_alpha;

        outColor = vec4(color, alpha);
    }





    if ((inst_data.flags & SPRITE_FLAG_CIRCLE) != 0u){

        float outline_thickness = inst_data.thickness;

        //    vec2 screen_dimensions = ubo[nonuniformEXT(ubo_index)].screen_dimension; // TODO:

        // Normalized pixel coordinates (from 0 to 1)
        vec2 local_position = in_local_pos * 2.0 - 1.0;
        local_position.x *= aspect_ratio;

        float distance = 1.0 - length(local_position);

        const float edge_fade = 0.005;// smooths the edge

        float circleAlpha = smoothstep(0, edge_fade, distance);
        circleAlpha *= smoothstep(outline_thickness + edge_fade, outline_thickness, distance);

        outColor = vec4(in_color, circleAlpha);

        //    outColor = vec4(in_color * circle, 1.0); * texture(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv); // if we want colors overlayed
    }

}