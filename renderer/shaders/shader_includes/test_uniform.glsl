#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int8  : require
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference2 : require

//NOTE: ambient can also refer to color
struct directional_light_data{
    vec3 position;
    vec3 direction;
    vec3 color;

    float diffuse;
    float specular;
    float orthobox_extent;
};

struct directional_light_shadow_data{
    uint light_idx;
    uint shadow_texture_idx;
    vec2 shadow_texture_size;
    mat4 light_world_view_matrix;
};

struct spot_light_shadow_data{
    uint light_idx;
    uint shadow_texture_idx;
    vec2 shadow_texture_size;
    mat4 light_world_view_matrix;
};


struct point_light_data{
    vec4 position;
    vec4 color;

    float constant;
    float linear;
    float quadratic;

    float diffuse;
    float specular;

    float range;

    float intensity;
    float padding1;
};

struct point_light_shadow_data{
    uint light_idx;
    uint shadow_texture_idx;
    vec2 shadow_texture_size;
    mat4 light_world_view_matrix[6];
};

struct spot_light_data{

    vec4 position;
    vec4 color;
    vec3 direction;

    float range;
    float cut_off;
    float outer_cut_off;

    float constant;
    float linear;
    float quadratic;

    float diffuse;
    float specular;
};


//BDA pointers

layout(buffer_reference, scalar) readonly buffer DIRECTIONAL_LIGHT_BUFFER{
    directional_light_data directional_light[];
};

layout(buffer_reference, scalar) readonly buffer DIRECTIONAL_LIGHT_SHADOW_BUFFER{
    directional_light_shadow_data directional_light_shadow_data[];
};


layout(buffer_reference, scalar) readonly buffer POINT_LIGHT_BUFFER{
    point_light_data point_light[];
};

layout(buffer_reference, scalar) readonly buffer POINT_LIGHT_SHADOW_BUFFER{
    point_light_shadow_data point_light_shadow_data[];
};


layout(buffer_reference, scalar) readonly buffer SPOT_LIGHT_BUFFER{
    spot_light_data spot_light[];
};
layout(buffer_reference, scalar) readonly buffer SPOT_LIGHT_SHADOW_BUFFER{
    spot_light_shadow_data spot_light_shadow_data[];
};


layout(buffer_reference, scalar) readonly buffer Vertex_Buffer{
    vec3 vertex_data[];
};
layout(buffer_reference, scalar) readonly buffer UV_Buffer{
    vec2 uv_data[];
};
layout(buffer_reference, scalar) readonly buffer Normal_Buffer{
    vec3 normal_data[];
};
layout(buffer_reference, scalar) readonly buffer Tangent_Buffer{
    vec4 tangent_data[];
};

layout(buffer_reference, scalar) readonly buffer Joint_Buffer{
    vec4 joint_data[];
};
layout(buffer_reference, scalar) readonly buffer Weight_Buffer{
    vec4 weights_data[];
};
layout(buffer_reference, scalar) readonly buffer Skinned_Matrix_Buffer{
    mat4 skinned_matrix_data[];
};

layout(buffer_reference, scalar) readonly buffer Transform_Buffer{
    mat4 transform_data[];
//model transform to be specific
};


//material
struct Material_Record{
    uint64_t offset;
    uint64_t size;
};

layout(buffer_reference, scalar) readonly buffer Material_Record_Buffer{
    Material_Record data[];
};


layout(buffer_reference, scalar) readonly buffer Material_Buffer{
    uint8_t material_data[];
};


layout(buffer_reference, scalar) readonly buffer Texture_Table_Buffer{
    uint texture_table[];
};






//Set 0 = global data
//Set 1 = textures
//Set 2 = buffers (not in use)
//Set 3 = (object material data) // (not in use)

layout(scalar, set = 0, binding = 0) uniform Global_UBO{
    mat4 view;
    mat4 proj;


    DIRECTIONAL_LIGHT_BUFFER directional_lights;
    DIRECTIONAL_LIGHT_SHADOW_BUFFER directional_lights_shadow;
    uint directional_shadow_map_index;
    uint directional_lights_count;


    SPOT_LIGHT_BUFFER spot_lights;
    SPOT_LIGHT_SHADOW_BUFFER spot_light_shadow_data;
    uint spot_light_shadow_map_index;
    uint spot_lights_count;


    POINT_LIGHT_BUFFER point_lights;
    POINT_LIGHT_SHADOW_BUFFER point_light_shadow_data;
    uint point_light_shadow_map_index;
    uint point_lights_count;


    vec4 camera_position;
    vec2 screen_dimensions;


    float time;
    uint render_mode;

//global list of buffer resources

//meshes
    Vertex_Buffer vertex_buffer;
    Normal_Buffer normal_buffer;
    UV_Buffer uv_buffer;
    Tangent_Buffer tangent_buffer;

//sk meshes
    Joint_Buffer joint_buffer;
    Weight_Buffer weight_buffer;
    Skinned_Matrix_Buffer skinned_matrix_buffer;

    Transform_Buffer transform_buffer;


    Material_Buffer material_buffer;
    Material_Record_Buffer material_record_buffer;
    Texture_Table_Buffer texture_table_buffer;

}ubo;


layout (set = 1, binding = 0) uniform sampler2D texture_samples[];
layout (set = 1, binding = 0) uniform texture2D textures[];
layout (set = 1, binding = 0) uniform texture2DMS texturesMS[];
layout (set = 1, binding = 0) uniform textureCube texture_cubes[];
layout(set = 1, binding = 0) uniform samplerCube texture_cubes_sampler[];
layout (set = 1, binding = 0) uniform texture2DArray textureArrays[];
//layout (set = 1, binding = 1) uniform sampler samplers[];




float directional_shadow_calculation(directional_light_data light, vec4 fragment_light_space_matrix, uint shadow_map_index, vec3 normalized_norm, vec4 model_position)
{

    //NOTE: doing this using the vulkan pipeline object
    //    const float min_shadow_bias = 0.00002f;
    //    const float max_shadow_bias = 0.0001f;

    //    vec3 light_direction = normalize(light.direction);

    //    const float shadow_bias = max(max_shadow_bias * (1.0 - dot(normalized_norm, light_direction)), min_shadow_bias);
    //    const float shadow_bias = 0;


    // perform perspective divide
    vec3 projection_coordinates = fragment_light_space_matrix.xyz / fragment_light_space_matrix.w;
    // transform into texture space from normalized device coordinates [-1,1] -> [0,1]
    projection_coordinates.xy = projection_coordinates.xy * 0.5 + 0.5;


    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    //    if (projection_coordinates.z > 1.0)
    //    {
    //        shadow = 0.0;
    //    }

    //early out for things not in shadow
    if (projection_coordinates.z < 0.0 ||
        projection_coordinates.z > 1.0 ||
        any(lessThan(projection_coordinates.xy, vec2(0.0))) ||
        any(greaterThan(projection_coordinates.xy, vec2(1.0))))
    {
        return 0.0;
    }


    // grab the shadow map
    // get closest depth value from light's perspective (using [-1,1] range fragPosLight as coords)
    float closest_depth = texture(texture_samples[(nonuniformEXT(shadow_map_index))], projection_coordinates.xy).x;
    // get depth of current fragment from light's perspective
    float current_depth = projection_coordinates.z;
    // check whether current frag pos is in shadow
    float shadow = 0;
    //     shadow = current_depth /*- shadow_bias*/ > closest_depth  ? 1.0 : 0.0;

    // percentage-closer filtering
    vec2 texel_size = 1.0 / textureSize(texture_samples[(nonuniformEXT(shadow_map_index))], 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(texture_samples[(nonuniformEXT(shadow_map_index))], projection_coordinates.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;








    return shadow;

}

float spot_light_shadow_calculation(spot_light_data spot_light_data, vec4 fragment_light_space_matrix, uint shadow_map_index, vec3 normalized_norm, vec4 model_position)
{
    //    const float min_shadow_bias = 0.00002f;
    //    const float max_shadow_bias = 0.0001f;

    //    vec3 light_direction = normalize(light.direction);

    //    const float shadow_bias = max(max_shadow_bias * (1.0 - dot(normalized_norm, light_direction)), min_shadow_bias);
    //    const float shadow_bias = 0;


    // perform perspective divide
    vec3 projection_coordinates = fragment_light_space_matrix.xyz / fragment_light_space_matrix.w;
    // transform into texture space from normalized device coordinates [-1,1] -> [0,1]
    projection_coordinates.xy = projection_coordinates.xy * 0.5 + 0.5;


    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    //    if (projection_coordinates.z > 1.0)
    //    {
    //        shadow = 0.0;
    //    }

    //early out for things not in shadow
    if (projection_coordinates.z < 0.0 ||
        projection_coordinates.z > 1.0 ||
        any(lessThan(projection_coordinates.xy, vec2(0.0))) ||
        any(greaterThan(projection_coordinates.xy, vec2(1.0))))
    {
        return 0.0;
    }


    // grab the shadow map
    // get closest depth value from light's perspective (using [-1,1] range fragPosLight as coords)
    float closest_depth = texture(texture_samples[(nonuniformEXT(shadow_map_index))], projection_coordinates.xy).x;
    // get depth of current fragment from light's perspective
    float current_depth = projection_coordinates.z;
    // check whether current frag pos is in shadow
    float shadow = 0;
    //     shadow = current_depth /*- shadow_bias*/ > closest_depth  ? 1.0 : 0.0;

    // percentage-closer filtering
    vec2 texel_size = 1.0 / textureSize(texture_samples[(nonuniformEXT(shadow_map_index))], 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(texture_samples[(nonuniformEXT(shadow_map_index))], projection_coordinates.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;

}

//TODO:
float point_light_shadow_calculation(point_light_data point_light_data, uint shadow_map_index, vec3 normalized_norm, vec3 world_position)
{
    /*   vec3 world_to_light_space =
       world_position.xyz - point_light_data.position.xyz;

       float current_distance = length(world_to_light_space);

       // Outside the point light's shadow range.
       if (current_distance >= point_light_data.range)
       return 0.0;

       // Sample the cubemap from the light toward the fragment.
       float closest_depth = texture(texture_cubes_sampler[nonuniformEXT(shadow_map_index)], world_to_light_space).r;

       // Assumes the shadow pass stores radial depth.
       float current_depth = current_distance / point_light_data.range;

       vec3 light_direction = normalize(-world_to_light_space);

       float NdotL = max(
               dot(normalize(normalized_norm), light_direction),
               0.0
       );

       float bias = max(0.005 * (1.0 - NdotL), 0.0005);

       return current_depth - bias > closest_depth ? 1.0 : 0.0;*/

    /* // must equal the near and far used by the six face projections in the shadow pass
     float POINT_SHADOW_NEAR = 1.0f;
     float far_plane = point_light_data.range;

     vec3  to_fragment = world_position.xyz - point_light_data.position.xyz;     // light -> fragment
     float dist        = length(to_fragment);

     // beyond the light's reach: nothing to shadow
     if (dist >= far_plane)
     { return 0.0; }

     // normal-offset bias: move the lookup point along the surface normal,
     // more at grazing angles to the light
     float n_dot_l = max(dot(normalized_norm, -to_fragment / dist), 0.0);
     vec3  lookup  = to_fragment + normalized_norm * (0.02 * (1.0 - n_dot_l) * dist);

     // projected depth that the shadow pass stored in this direction.
     // m comes from the SAME vector as the cube lookup, because the hardware picks the face from it
     float m = max(max(abs(lookup.x), abs(lookup.y)), abs(lookup.z));
     m = max(m, POINT_SHADOW_NEAR);
     float current = far_plane * (m - POINT_SHADOW_NEAR) / (m * (far_plane - POINT_SHADOW_NEAR));

     float closest = textureLod(texture_cubes_sampler[nonuniformEXT(shadow_map_index)], lookup, 0.0).r;

     const float bias = 0.0005;           // small constant on top of the normal offset
     return (current - bias > closest) ? 1.0 : 0.0;*/


    vec3 to_fragment =
    world_position - point_light_data.position.xyz;

    float major_axis = max(
            max(abs(to_fragment.x), abs(to_fragment.y)),
            abs(to_fragment.z)
    );

    float near_plane = 0.1;
    float far_plane = point_light_data.range;

    if (major_axis <= near_plane || major_axis >= far_plane)
    return 0.0;

    // Depth produced by the perspective projection in the shadow pass.
    float current_depth =
    far_plane / (far_plane - near_plane)
    - (far_plane * near_plane) /
    ((far_plane - near_plane) * major_axis);

    // Sample the closest depth stored in the cubemap.
    float closest_depth = textureLod(
            texture_cubes_sampler[nonuniformEXT(shadow_map_index)],
            to_fragment, 0.0 ).r;

    // Bias to reduce shadow acne.
    vec3 light_direction = normalize(-to_fragment);

    float NdotL = max(
            dot(normalize(normalized_norm), light_direction),
            0.0
    );


    // 1 = shadow, 0 = lit.
    return (current_depth > closest_depth) ? 1.0 : 0.0;
}



vec3 calculate_directional_light(directional_light_data light, vec3 normalized_normal, vec3 view_direction)
{
    float specular_shininess = 32;

    vec3 light_direction = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normalized_normal, light_direction), 0.0);
    // specular shading
    vec3 reflect_direction = reflect(-light_direction, normalized_normal);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), specular_shininess);
    // combine results
    vec3 ambient = light.color.xyz;
    vec3 diffuse = light.diffuse * diff * light.color.xyz;
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
    float distance = length(light.position.xyz - frag_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // combine results


    vec3 ambient = light.color.xyz;
    vec3 diffuse = light.diffuse * light.color.xyz;
    vec3 specular = light.specular * spec * light.color.xyz;
    //for textures usage
    //    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    //    vec3 diffuse  = light.diffuse  * diff * vec3(texture(light.diffuse, TexCoords));
    //    vec3 specular = light.specular * spec * vec3(texture(light.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 calculate_spot_light(spot_light_data light, vec3 normal, vec3 frag_position, vec3 view_direction)
{

    float specular_shininess = 32;

    vec3 lightDir = normalize(light.position.xyz - frag_position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(view_direction, reflectDir), 0.0), specular_shininess);
    // attenuation
    float distance = length(light.position.xyz - frag_position);
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