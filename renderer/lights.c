#include "lights.h"


Light_System* light_system_init(renderer* renderer)
{
    Light_System* out_light_system = arena_alloc(&renderer->arena, sizeof(Light_System));

    //TODO: TEMP FOR NOW
    out_light_system->directional_light_count = 1;
    out_light_system->point_light_count = 1;

    out_light_system->point_lights =
        arena_alloc(&renderer->arena, sizeof(Point_Light) * out_light_system->point_light_count);

    out_light_system->directional_lights =
        arena_alloc(&renderer->arena, sizeof(Directional_Light) * out_light_system->directional_light_count);


    for (int point_light_idx = 0; point_light_idx < out_light_system->point_light_count; point_light_idx++)
    {
        point_light_init(&out_light_system->point_lights[point_light_idx]);
    }
    for (int directional_light_idx = 0; directional_light_idx < out_light_system->directional_light_count;
         directional_light_idx++)
    {
        directional_light_init(&out_light_system->directional_lights[directional_light_idx]);
    }

    //TODO: remove later just some test lights for now
    out_light_system->directional_lights[0].color = (vec3){1.0f, 0.0f, 0.5f};
    out_light_system->point_lights[0].color = (vec4){1.0f, 1.0f, 0.0f, 0.0f};


    //TODO: allocate larger sizes for the buffers
    out_light_system->directional_light_storage_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_CPU_STORAGE,
        sizeof(Directional_Light) * out_light_system->directional_light_count);

    out_light_system->point_light_storage_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_CPU_STORAGE,
        sizeof(Point_Light) * out_light_system->point_light_count);


    out_light_system->directional_light_staging_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING,
        sizeof(Directional_Light) * out_light_system->directional_light_count);

    out_light_system->point_light_staging_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING,
        sizeof(Point_Light) * out_light_system->point_light_count);


    vulkan_buffer_data_copy_and_upload(renderer,
                                       out_light_system->directional_light_storage_buffer_handle,
                                       out_light_system->directional_light_staging_buffer_handle,
                                       out_light_system->directional_lights,
                                       sizeof(Directional_Light) * out_light_system->directional_light_count);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       out_light_system->point_light_storage_buffer_handle,
                                       out_light_system->point_light_staging_buffer_handle,
                                       out_light_system->point_lights,
                                       sizeof(Point_Light) * out_light_system->point_light_count);

    INFO("LIGHT SYSTEM CREATED")

    return out_light_system;
}

void light_system_update(renderer* renderer, Light_System* light_system)
{
    //TODO: data upload of any lights and replace the function calls in vulkan buffer data copy

    vulkan_buffer_reset_offset(renderer, light_system->point_light_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, light_system->point_light_staging_buffer_handle);


    vulkan_buffer_data_copy_and_upload(renderer,
                                       light_system->directional_light_storage_buffer_handle,
                                       light_system->point_light_staging_buffer_handle,
                                       light_system->directional_lights,
                                       sizeof(Directional_Light) * light_system->directional_light_count);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       light_system->point_light_storage_buffer_handle,
                                       light_system->point_light_staging_buffer_handle,
                                       light_system->point_lights,
                                       sizeof(Point_Light) * light_system->point_light_count);

}


void directional_light_init(Directional_Light* light)
{
    light->direction = vec3_zero();
    light->color = vec3_zero();

    light->diffuse = 0.5f;
    light->specular = 0.5f;
}

void point_light_init(Point_Light* light)
{
    light->position = vec4_zero();
    light->color = vec4_one();

    light->diffuse = 0.5f;
    light->specular = 0.5f;

    // light->intensity = 1.0f;
    // light->radius = 1.0f;


    light->constant = 1.0f;
    light->linear = 0.09f;
    light->quadratic = 0.032f;
}

void spot_light_init(Spot_Light* light)
{
    light->position = vec4_zero();
    light->color = vec4_one();
    light->direction = vec3_one();

    light->diffuse = 0.5f;
    light->specular = 0.5f;

    // light->intensity = 1.0f;
    // light->radius = 1.0f;


    light->constant = 1.0f;
    light->linear = 0.09f;
    light->quadratic = 0.032f;
}
