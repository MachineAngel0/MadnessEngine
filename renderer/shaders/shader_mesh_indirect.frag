#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require


layout (set = 1, binding = 0) uniform sampler2D texture_samples[];



layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec4 in_tangent;
layout(location = 2) in vec2 in_tex;
layout(location = 3) in flat uint in_color_idx;


//look into subpasses/renderpasses for more/different out values
layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(inColor, 1.0) * texture(texSampler, in_tex); // if we want colors overlayed
    //outColor = texture(texSampler, in_tex);
    //    outColor = vec4(1.0f, 0.5f, 0.5f,1.0f); // old
    //    outColor = vec4(1.0f, 0.5f, 0.5f, 1.0f) * texture(texture_samples[(nonuniformEXT(pc.albedo_idx))], in_tex);

    vec3 norm = normalize(in_normal);
//    vec3 light_direction = normalize(lightPos - FragPos);



    outColor = texture(texture_samples[(nonuniformEXT(in_color_idx))], in_tex);
//    outColor = texture(texture_samples[(nonuniformEXT(0))], in_tex);
//        outColor = vec4(1.0f, 0.5f, 0.5f,1.0f); // old

}