#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"


layout (set = 1, binding = 0) uniform sampler2D texture_samples[];


layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;


layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(fragColor, 1.0);
    //outColor = vec4(fragTexCoord, 0.0, 1.0); // useful for debugging
    //outColor = texture(texSampler, fragTexCoord);
    outColor = vec4(fragColor, 1.0) * texture(texture_samples[(nonuniformEXT(0))], fragTexCoord); // if we want colors overlayed

    //creates greyish outline, but looks bad
    //vec4 texel = texture(texSampler, fragTexCoord);      // sample atlas RGBA
    //float alpha = texel.a;                               // use alpha channel
    //vec3 rgb   = fragColor * alpha;                      // premultiply tint
    //outColor   = vec4(rgb, alpha);

}