#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require



//NOTE: ambient can also refer to color
struct directional_light_data{
    vec3 direction;
    vec3 color;

    float diffuse;
    float specular;

};

struct point_light_data{
    vec4 position;
    vec4 color;

    float constant;
    float linear;
    float quadratic;

    float diffuse;
    float specular;
};

struct spot_light_data{

    vec3 position;
    vec3 direction;
    float cut_off;
    float outer_cut_off;

    float constant;
    float linear;
    float quadratic;

    vec3 color;
    vec3 diffuse;
    vec3 specular;
};

layout(buffer_reference, scalar) readonly buffer DIRECTIONAL_LIGHT_BUFFER{
    directional_light_data directional_light[];
};

layout(buffer_reference, scalar) readonly buffer POINT_LIGHT_BUFFER{
    point_light_data point_light[];
};

layout(buffer_reference, scalar) readonly buffer SPOT_LIGHT_BUFFER{
    spot_light_data spot_light[];
};

layout(scalar, set = 0, binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;


    DIRECTIONAL_LIGHT_BUFFER directional_lights;
//    uint directional_light_index;


    POINT_LIGHT_BUFFER point_lights;
    uint point_lights_count;


//    SPOT_LIGHT_BUFFER spot_lights;
//    uint spot_lights_count;

    vec4 camera_view_pos;

    uint render_mode;

    uint _padding0;
    uint _padding1;
    uint _padding2;
    uint _padding3;
    uint _padding4;
    uint _padding5;

} ubo[];


vec3 calculate_directional_light(directional_light_data light, vec3 normal, vec3 view_direction)
{
    float specular_shininess = 32;

    vec3 light_direction = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), specular_shininess);
    // combine results
    vec3 ambient  = light.color.xyz;
    vec3 diffuse  = light.diffuse  * diff * light.color.xyz;
    vec3 specular = light.specular * spec * light.color.xyz;

    //for textures usage
    //    vec3 ambient  = light.ambient  * vec3(texture(light.diffuse, TexCoords));
    //    vec3 diffuse  = light.diffuse  * diff * vec3(texture(light.diffuse, TexCoords));
    //    vec3 specular = light.specular * spec * vec3(texture(light.specular, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 calculate_point_light(point_light_data light, vec3 normal, vec3 frag_position, vec3 view_direction)
{

    float specular_shininess = 32;


    vec3 lightDir = normalize(light.position.xyz - frag_position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflect_direction = reflect(-lightDir, normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), specular_shininess);
    // attenuation
    float distance    = length(light.position.xyz - frag_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // combine results


    vec3 ambient  = light.color.xyz;
    vec3 diffuse  = light.diffuse * light.color.xyz;
    vec3 specular = light.specular * spec * light.color.xyz;
    //for textures usage
    //    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    //    vec3 diffuse  = light.diffuse  * diff * vec3(texture(light.diffuse, TexCoords));
    //    vec3 specular = light.specular * spec * vec3(texture(light.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(spot_light_data light, vec3 normal, vec3 frag_position, vec3 view_direction)
{

    float specular_shininess = 32;

    vec3 lightDir = normalize(light.position - frag_position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(view_direction, reflectDir), 0.0), specular_shininess);
    // attenuation
    float distance = length(light.position - frag_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.color.xyz;
    vec3 diffuse = light.diffuse * light.color.xyz;
    vec3 specular = light.specular * spec * light.color.xyz;
//    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
//    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
//    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}