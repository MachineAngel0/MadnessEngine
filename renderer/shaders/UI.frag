#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"


layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec2 in_local_pos;
layout(location = 3) in flat uint in_texture_idx;
layout(location = 4) in flat uint in_material_buffer_location;


layout(location = 0) out vec4 outColor;

//sdf
float sdRoundBox(in vec2 p, in vec2 b, in vec4 r)
{
    r.xy = (p.x>0.0)?r.xy : r.zw;
    r.x  = (p.y>0.0)?r.x  : r.y;
    vec2 q = abs(p)-b+r.x;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r.x;
}


//text
float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}


vec2 sqr(vec2 x) { return x*x; }// squares vector components

//TODO: this isn't correct, needs to be set cpu side, and this needs to be in linear color range, not rgb
const float pxRange = 2;// set to distance field's pixel range

float screenPxRange(sampler2D msdf, vec2 uv) {
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(msdf, 0));
    // If inversesqrt is not available, use vec2(1.0)/sqrt
    vec2 screenTexSize = inversesqrt(sqr(dFdx(uv))+sqr(dFdy(uv)));
    // Can also be approximated as screenTexSize = vec2(1.0)/fwidth(texCoord);
    return max(00.5*dot(unitRange, screenTexSize), 1.0);
}

void main() {
    UI_Data inst_data = PC_2D.material_buffer.ui_data[nonuniformEXT(in_material_buffer_location)];

    vec2 screen_dimensions = ubo.screen_dimensions;
    float aspect_ratio = screen_dimensions.x / screen_dimensions.y;

    vec3 fill_color = in_color;

    if ((inst_data.flags & UI_FLAG_IMAGE) != 0u){
        fill_color *= texture(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv).rgb;
        //fill_color = texture(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv).rgb;
    }


    {
        vec2 p = (in_local_pos - vec2(0.5)) * 2.0;

        // size is normalized, so turn back to pixel dimensions first to get the proper size
        vec2 pixel_size = inst_data.size * screen_dimensions;
        float quad_aspect = pixel_size.x / pixel_size.y;
        p.x *= quad_aspect;
        vec2 size = vec2(quad_aspect, 1.0);

        vec4 radius = vec4(inst_data.rounded_radius);

        float dist = sdRoundBox(p, size, radius);
        float px = fwidth(dist);

        float outline_thickness = inst_data.outline_thickness;
        vec3 outline_color = inst_data.outline_color;

        float fill_alpha = 1.0 - smoothstep(-px, px, dist / px);

        float edge_outer = smoothstep(-px, px, dist / px);
        float edge_inner = smoothstep(-outline_thickness - px, -outline_thickness + px, dist);

        float outline_alpha = clamp(edge_inner - edge_outer, 0.0, 1.0);

        // combine
        vec3 color = mix(fill_color, outline_color, outline_alpha);
        outColor = vec4(color, fill_alpha);
    }

    if ((inst_data.flags & UI_FLAG_TEXT) != 0u){
        outColor = vec4(in_color, 1.0f) * texture(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv);// if we want colors overlayed

        vec3 msd = texture(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv).rgb;
        float sd = median(msd.r, msd.g, msd.b);
        float screenPxDistance = screenPxRange(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv) * (sd - 0.5);
        float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

        vec4 background_color = vec4(inst_data.background_color, 0);
        vec4 text_color = vec4(in_color,1);

        outColor = mix(background_color, text_color, opacity);
    }

    if ((inst_data.flags & UI_FLAG_CIRCLE) != 0u){

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