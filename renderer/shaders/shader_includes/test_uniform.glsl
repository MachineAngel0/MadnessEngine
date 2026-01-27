#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require


struct directional_light_data{
    vec3 direction;
    vec3 color;
    float diffuse;
    float specular;
};

struct point_light_data{
    vec4 position;
    vec4 color;
};


layout(buffer_reference, scalar) readonly buffer POINT_LIGHT_BUFFER{
    point_light_data point_light[];
};

layout(std430, set = 0, binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;


    POINT_LIGHT_BUFFER point_lights;
    uint point_lights_count;

    vec4 camera_view_pos;

    uint render_mode;

} ubo[];


