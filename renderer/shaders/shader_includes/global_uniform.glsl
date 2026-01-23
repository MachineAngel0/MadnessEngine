#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require


struct directional_light{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct point_lights{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout (buffer_reference, scalar) readonly buffer Directional_Light_Buffer {
    directional_light position[];
};
layout (buffer_reference, scalar) readonly buffer Point_Light_Buffer {
    point_lights position[];
};

//Stores all our global data
layout(std140, set = 0, binding = 0) uniform UniformBufferObject{
    mat4 view;
    mat4 proj;
    //mat4 viewProj;
    vec4 cameraPos;


    Directional_Light_Buffer directional_lights;
    //uint directional_light_index, extra param, for when we want to render with a specific directional light
    Point_Light_Buffer point_lights;

} ubo[];
