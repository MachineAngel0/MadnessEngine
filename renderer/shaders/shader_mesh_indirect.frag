#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/materials.glsl"

layout (set = 1, binding = 0) uniform sampler2D texture_samples[];



layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec4 in_tangent;
layout(location = 2) in vec2 in_tex;
layout(location = 3) in flat uint in_color_idx;
layout(location = 4) in vec3 in_world_position;


//look into subpasses/renderpasses for more/different out values
layout(location = 0) out vec4 outColor;

void main() {




    // properties
    vec3 norm = normalize(in_normal);
    // Normal = mat3(transpose(inverse(model))) * aNormal; // for generating proper normals with non-uniform scales

    vec3 view_direction = normalize(ubo[nonuniformEXT(0)].camera_view_pos.xyz - in_world_position);

    // Directional lighting
    vec3 result = calculate_directional_light(ubo[nonuniformEXT(0)].directional_lights.directional_light[0], norm, view_direction);
    // Point lights
    for(int i = 0; i < ubo[nonuniformEXT(0)].point_lights_count; i++){
        result += calculate_point_light(ubo[nonuniformEXT(0)].point_lights.point_light[i], norm, in_world_position, view_direction);
    }
    // phase 3: Spot light
    //result += CalcSpotLight(spotLight, norm, in_frag_pos, view_direction);

    //final color
    vec4 texture_result = texture(texture_samples[(nonuniformEXT(in_color_idx))], in_tex);
    vec4 final_result = vec4(result,1.0) * texture_result;
    outColor = final_result;

    //LIGHTING INFO
    if (ubo[nonuniformEXT(0)].render_mode == 2){
        final_result = vec4(result, 1.0) * (in_normal,1.0);
        outColor = final_result;
    }

    //NORMALS INFO
    if (ubo[nonuniformEXT(0)].render_mode == 1){
        outColor = vec4(abs(in_normal), 1.0f);
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