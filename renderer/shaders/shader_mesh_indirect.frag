#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "shader_includes/test_uniform.glsl"

layout (set = 1, binding = 0) uniform sampler2D texture_samples[];



layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec4 in_tangent;
layout(location = 2) in vec2 in_tex;
layout(location = 3) in flat uint in_color_idx;
layout(location = 4) in vec3 in_frag_pos;


//look into subpasses/renderpasses for more/different out values
layout(location = 0) out vec4 outColor;

void main() {




    /*
    // properties
    vec3 norm = normalize(in_normal);
    vec3 view_direction = normalize(ubo[nonuniformEXT(0)].camera_view_pos.xyz - in_frag_pos);

    // Directional lighting
    vec3 result = calculate_directional_light(ubo[nonuniformEXT(0)].directional_lights.directional_light[0], norm, view_direction);
    // Point lights
    for(int i = 0; i < ubo[nonuniformEXT(0)].point_lights_count; i++){
        result += calculate_point_light(ubo[nonuniformEXT(0)].point_lights.point_light[i], norm, in_frag_pos, view_direction);
    }
    // phase 3: Spot light
    //result += CalcSpotLight(spotLight, norm, in_frag_pos, view_direction);

    //final color
    vec4 texture_result = texture(texture_samples[(nonuniformEXT(in_color_idx))], in_tex);
    vec4 final_result = vec4(result,1.0) * texture_result;

    //LIGHTING INFO
    if (ubo[nonuniformEXT(0)].render_mode == 2){
        final_result = vec4(result, 1.0) * (in_normal,1.0);
    }
    outColor = final_result;


    //NORMALS INFO
    if (ubo[nonuniformEXT(0)].render_mode == 1){
        outColor = vec4(abs(in_normal), 1.0f);
    }
    */


    outColor = vec4(1.0f, 0.5f, 0.5f,1.0f); // for testing


}