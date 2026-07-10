#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"



layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in flat uint in_texture_idx;
layout(location = 3) in flat uint in_material_idx;
layout(location = 4) in vec2 in_local_pos;


layout(location = 0) out vec4 outColor;


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
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

void main() {


    UI_Data inst_data = PC_2D.material_buffer.ui_data[nonuniformEXT(in_material_idx)];

    vec2 screen_dimensions = ubo.screen_dimensions;
    float aspect_ratio = screen_dimensions.x / screen_dimensions.y;

    vec2 p = (in_local_pos - vec2(0.5)) * 2.0;


    vec3 msd = texture(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv) * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

    vec4 background_color = vec4(inst_data.background_color, 0);
    vec4 text_color = vec4(in_color,1);

    outColor = mix(background_color, text_color, opacity);

}