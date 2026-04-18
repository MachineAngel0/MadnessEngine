#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/2d_structs.glsl"
#include "shader_includes/test_uniform.glsl"


layout (set = 1, binding = 0) uniform sampler2D texture_samples[];

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec2 in_tex;
layout(location = 3) in flat uint in_material_idx;
layout(location = 4) in flat uint in_flags;


layout(location = 0) out vec4 outColor;

void main() {

    outColor = vec4(in_color, 1.0f);

    if ((in_flags & SPRITE_FLAG_TEXTURE) != 0u){
        outColor *= texture(texture_samples[(nonuniformEXT(in_material_idx))], in_uv);
    }

}