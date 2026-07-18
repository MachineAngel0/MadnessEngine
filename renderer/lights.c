#include "lights.h"

#include "vk_buffer.h"




Light_System* light_system_init(Renderer* renderer)
{
    Light_System* out_light_system = allocator_alloc(&renderer->allocator, sizeof(Light_System));

    //TODO: TEMP FOR NOW
    out_light_system->directional_light_count = 1;
    out_light_system->point_light_count = 1;
    out_light_system->spot_light_count = 1;

    out_light_system->point_lights =
        allocator_alloc(&renderer->allocator, sizeof(Point_Light) * out_light_system->point_light_count);

    out_light_system->directional_lights =
        allocator_alloc(&renderer->allocator, sizeof(Directional_Light) * out_light_system->directional_light_count);

    out_light_system->spot_lights =
        allocator_alloc(&renderer->allocator, sizeof(Spot_Light) * out_light_system->directional_light_count);


    for (u32 point_light_idx = 0; point_light_idx < out_light_system->point_light_count; point_light_idx++)
    {
        point_light_init(&out_light_system->point_lights[point_light_idx]);
    }
    for (u32 directional_light_idx = 0; directional_light_idx < out_light_system->directional_light_count;
         directional_light_idx++)
    {
        directional_light_init(&out_light_system->directional_lights[directional_light_idx]);
    }

    for (u32 spot_light_idx = 0; spot_light_idx < out_light_system->directional_light_count;
     spot_light_idx++)
    {
        spot_light_init(&out_light_system->spot_lights[spot_light_idx]);
    }


    //TODO: remove later just some test lights for now
    out_light_system->directional_lights[0].color = (vec3s){1.0f, 0.0f, 0.5f};
    out_light_system->point_lights[0].color = (vec4s){1.0f, 1.0f, 0.0f, 0.0f};
    out_light_system->spot_lights[0].color = (vec4s){0.0f, 0.0f, 1.0f, 0.0f};


    //TODO: allocate larger sizes for the buffers
    out_light_system->directional_light_storage_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_CPU_STORAGE,
        sizeof(Directional_Light) * out_light_system->directional_light_count);

    out_light_system->point_light_storage_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_CPU_STORAGE,
        sizeof(Point_Light) * out_light_system->point_light_count);

    out_light_system->spot_light_storage_buffer_handle = vulkan_buffer_create(
    renderer, renderer->buffer_system, BUFFER_TYPE_CPU_STORAGE,
    sizeof(Spot_Light) * out_light_system->point_light_count);


    out_light_system->directional_light_staging_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING,
        sizeof(Directional_Light) * out_light_system->directional_light_count);

    out_light_system->point_light_staging_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING,
        sizeof(Point_Light) * out_light_system->point_light_count);

    out_light_system->spot_light_staging_buffer_handle = vulkan_buffer_create(
    renderer, renderer->buffer_system, BUFFER_TYPE_STAGING,
    sizeof(Spot_Light) * out_light_system->point_light_count);



    // vulkan_buffer_data_copy_and_upload(renderer,
    //                                    out_light_system->directional_light_storage_buffer_handle,
    //                                    out_light_system->directional_light_staging_buffer_handle,
    //                                    out_light_system->directional_lights,
    //                                    sizeof(Directional_Light) * out_light_system->directional_light_count);
    //
    // vulkan_buffer_data_copy_and_upload(renderer,
    //                                    out_light_system->point_light_storage_buffer_handle,
    //                                    out_light_system->point_light_staging_buffer_handle,
    //                                    out_light_system->point_lights,
    //                                    sizeof(Point_Light) * out_light_system->point_light_count);

    INFO("LIGHT SYSTEM CREATED")

    return out_light_system;
}

void light_system_update(Renderer* renderer, Light_System* light_system, vulkan_command_buffer* command_buffer)
{
    //TODO: probably shouldn't be reuploading every frame

    vulkan_buffer_reset_offset(renderer, light_system->point_light_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, light_system->point_light_staging_buffer_handle);

    vulkan_buffer_reset_offset(renderer, light_system->directional_light_storage_buffer_handle);
    vulkan_buffer_reset_offset(renderer, light_system->directional_light_staging_buffer_handle);


    vulkan_buffer_reset_offset(renderer, light_system->spot_light_storage_buffer_handle);
    vulkan_buffer_reset_offset(renderer, light_system->spot_light_staging_buffer_handle);


    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer,
                                                   light_system->directional_light_storage_buffer_handle,
                                                   light_system->directional_light_staging_buffer_handle,
                                                   command_buffer,
                                                   light_system->directional_lights,
                                                   sizeof(Directional_Light) * light_system->directional_light_count);

    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer,
                                                   light_system->point_light_storage_buffer_handle,
                                                   light_system->point_light_staging_buffer_handle, command_buffer,
                                                   light_system->point_lights,
                                                   sizeof(Point_Light) * light_system->point_light_count);

    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer,
                                               light_system->spot_light_storage_buffer_handle,
                                               light_system->spot_light_staging_buffer_handle, command_buffer,
                                               light_system->spot_lights,
                                               sizeof(Spot_Light) * light_system->spot_light_count);


    float near_plane = 1.0f, far_plane = 7.5f;
    for (u32 i = 0; i < light_system->spot_light_count; i++)
    {
        light_system->spot_lights[i];
        mat4s light_projection = glms_ortho(-10, 10, -10, 10, near_plane, far_plane);
        mat4s light_view = glms_lookat((vec3s){-2.0f, 4.0f, -1.0f},
                               (vec3s){0.0f, 0.0f, 0.0f},
                               (vec3s){0.0f, 1.0f, 0.0f});
        mat4s light_space_matrix = glms_mat4_mul(light_projection, light_view);

    }




    /*
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging(renderer,
                                               light_system->spot_light_storage_buffer_handle,
                                               command_buffer,
                                               light_system->spot_lights,
                                               sizeof(Spot_Light) * light_system->spot_light_count);
    */





}


void directional_light_init(Directional_Light* light)
{
    light->direction = glms_vec3_zero();
    light->color = glms_vec3_zero();

    light->diffuse = 0.5f;
    light->specular = 0.5f;
}

void point_light_init(Point_Light* light)
{
    light->position = glms_vec4_zero();
    light->color = glms_vec4_one();

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
    light->position = glms_vec4_zero();
    light->color = glms_vec4_one();
    light->direction = glms_vec3_one();

    light->diffuse = 0.5f;
    light->specular = 0.5f;

    // light->intensity = 1.0f;
    // light->radius = 1.0f;


    light->constant = 1.0f;
    light->linear = 0.09f;
    light->quadratic = 0.032f;
}
