#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/pc_indirect_mesh.glsl"


layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec4 in_tangent;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_world_position;
layout(location = 4) in flat uint64_t material_offset;
layout(location = 6) in vec4 in_frag_model;
layout(location = 7) in mat4 in_model_transform;

//look into subpasses/renderpasses for more/different out values
layout(location = 0) out vec4 outColor;


void main() {

    Pbr material = Pbr_Buffer(uint64_t(ubo.material_buffer) + material_offset).data;

    // properties
    vec3 norm = normalize(in_normal);
    // Normal = mat3(transpose(inverse(model))) * aNormal; // for generating proper normals with non-uniform scales

    vec3 view_direction = normalize(ubo.camera_position.xyz - in_world_position);

    vec3 result = vec3(0);
    // Directional lighting
    vec4 directional_light_model_matrix = ubo.directional_lights_shadow.directional_light_shadow_data[0].light_world_view_matrix * in_frag_model;

    vec3 sun = calculate_directional_light(ubo.directional_lights.directional_light[0], norm, view_direction);
    // calculate directional shadow
    float shadow = directional_shadow_calculation(ubo.directional_lights.directional_light[0], directional_light_model_matrix, ubo.directional_shadow_map_index, norm, in_frag_model);
    result = sun * (1.0 - shadow);

    // Point lights
    for (int i = 0; i < ubo.point_lights_count; i++){
        result += calculate_point_light(ubo.point_lights.point_light[i], norm, in_world_position, view_direction);

    }
    // Spot light
    for (int i = 0; i < ubo.spot_lights_count; i++){

        vec4 spot_light_model_matrix = ubo.spot_light_shadow_data.spot_light_shadow_data[i].light_world_view_matrix * in_frag_model;
        vec3 spot_light_result = calculate_spot_light(ubo.spot_lights.spot_light[i], norm, in_world_position, view_direction);
        float shadow_spot_light_result = spot_light_shadow_calculation(ubo.spot_lights.spot_light[i], spot_light_model_matrix, ubo.spot_light_shadow_map_index, norm, in_frag_model);
        result += spot_light_result * (1 - shadow_spot_light_result);
    }



    //final color
    vec4 albedo = material.color;
    if ((material.flags & MESH_PIPELINE_COLOR) != 0u){

        uint color_idx = ubo.texture_table_buffer.texture_table[material.color_index];

        vec4 texture_result = texture(texture_samples[(nonuniformEXT(color_idx))], in_uv);
        albedo *= texture_result;
    }




    outColor = vec4(result, 1.0) * albedo;


    //LIGHTING INFO
    if (ubo.render_mode == 2){
        albedo = vec4(result, 1.0);
        // albedo = material.color * vec4(result,1.0) ; // if we want color info
        outColor = albedo;
    }

    //NORMALS INFO
    if (ubo.render_mode == 1){
        outColor = vec4(abs(in_normal), 1.0f);

        /*
        // float nearDist = 0.1;
        // float farDist = 50.0;

        shows depth values in a black and white range
        float z = gl_FragCoord.z ;
        float vDepth = (2.0 * nearDist * farDist) / (farDist + nearDist - z * (farDist - nearDist));

        float t = clamp((vDepth - nearDist) / (farDist - nearDist), 0.0, 1.0);
        outColor = vec4(vec3(t), 1.0);
        */
    }


    //    outColor = vec4(1.0f, 0.5f, 0.5f,1.0f); // for testing

    /* PBR IMPLEMENTATION

    vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    vec3 albedo     = color; //not a texture, hard coded color
    float metallic  = texture(metallicMap, TexCoords).r;
    float metallic  = metallic;
    float roughness = texture(roughnessMap, TexCoords).g;
    float ao        = texture(aoMap, TexCoords).b;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);


    */


}