#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_ARB_shader_draw_parameters : enable


#include "shader_includes/sprite_structs.glsl"
#include "shader_includes/test_uniform.glsl"




layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec2 in_tex;
layout(location = 3) in flat uint in_flags;
layout(location = 4) in flat uint instance_index;


layout(location = 0) out vec4 outColor;

void main() {

    outColor = vec4(in_color, 1.0f);

    Sprite sprite_data = push_sprite.sprite_ssbo.data[instance_index];

    if ((in_flags & SPRITE_FLAG_TEXTURE) != 0u){
        //get texture from texture table
        uint texture_idx = ubo.texture_table_buffer.texture_table[sprite_data.texture_index];
        outColor *= texture(texture_samples[(nonuniformEXT(texture_idx))], in_uv);
    }

}