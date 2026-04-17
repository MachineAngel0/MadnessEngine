#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable


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

    uint vertex_idx;
    uint index_idx;
    uint normal_idx;
    uint tangent_idx;
    uint uv_idx;
    uint transform_idx;

    uint draw_data_idx;
    uint material_instance_idx;


    uint material_pbr_idx;
    uint material_wave_idx;
    uint material_black_hole_idx;
    uint material_uv_animation_idx;
    uint material_blend1_idx;
    uint material_blend2_idx;
} ubo[];



// binding 0 stores our textures
// binding 1 stores our params and indexes into the descriptor
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];
layout (set = 1, binding = 0) uniform texture2D textures[];
layout (set = 1, binding = 0) uniform texture2DMS texturesMS[];
layout (set = 1, binding = 0) uniform textureCube textureCubes[];
layout (set = 1, binding = 0) uniform texture2DArray textureArrays[];
//layout (set = 1, binding = 1) uniform sampler samplers[];

layout(set = 2, binding = 0, scalar) readonly buffer UV_BUFFER{
    vec2 uv[];
}UV[];

layout(set = 2, binding = 0, scalar) readonly buffer NORMAL_BUFFER{
    vec3 normal[];
}NORMAL[];

layout(set = 2, binding = 0, scalar) readonly buffer TRANSFORM_BUFFER{
    mat4 model_transforms[];
}TRANSFORM[];

layout(set = 2, binding = 0, scalar) readonly buffer TANGENT_BUFFER{
    vec4 tangent[];
}TANGENT[];










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


    vec3 light_direction = normalize(light.position.xyz - frag_position);


    // diffuse shading
    float diff = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflect_direction = reflect(-light_direction, normal);

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


// PBR

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anyways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.


const float PI = 3.14159265359;
/*
vec3 getNormalFromMap(uint normal_map_index)
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
float DistributionGGX(vec3 normal, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(normal, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
float GeometrySchlickGGX(float normal_dot_vector, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = normal_dot_vector;
    float denom = normal_dot_vector * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 normal, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(normal, V), 0.0);
    float NdotL = max(dot(normal, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

*/