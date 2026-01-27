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

    //    outColor = vec4(1.0f, 0.5f, 0.5f,1.0f); // for testing

    // ambient
    //    float ambientStrength = 0.1;
    //    vec3 ambient = ambientStrength * lightColor;

    float ambient_strength = 0.5;
    float specular_strength = 0.5;

    vec3 light_pos = ubo[nonuniformEXT(0)].point_lights.point_light[0].position.xyz;
    vec3 light_color = ubo[nonuniformEXT(0)].point_lights.point_light[0].color.xyz;

    vec3 ambient = ambient_strength * light_color;

    //diffuse
    vec3 norm = normalize(in_normal);
    vec3 light_direction = normalize(light_pos - in_frag_pos);
    float diff = max(dot(norm, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

    //specular
    vec3 view_direction = normalize(ubo[nonuniformEXT(0)].camera_view_pos.xyz - in_frag_pos);
    vec3 reflect_direction = reflect(-light_direction, norm);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;

    //final color
    vec4 texture_result = texture(texture_samples[(nonuniformEXT(in_color_idx))], in_tex);
    vec4 final_result = vec4(ambient + diffuse + specular, 1.0) * texture_result;

    outColor = final_result;
//    outColor = vec4(norm * 0.5 + 0.5, 1.0);

}