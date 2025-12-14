#version 450

//layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec4 in_tangent;
layout(location = 2) in vec2 in_tex;


//look into subpasses/renderpasses for more/different out values
layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(inColor, 1.0) * texture(texSampler, in_tex); // if we want colors overlayed
    //outColor = texture(texSampler, in_tex);
    outColor = vec4(1.0f, 0.5f, 0.5f,1.0f);

}