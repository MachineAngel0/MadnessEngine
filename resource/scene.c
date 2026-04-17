#include "scene.h"

Scene* scene_init(Memory_System* memory_system)
{
    Scene* scene = memory_system_alloc(memory_system, sizeof(Scene), MEMORY_SUBSYSTEM_RESOURCE);

    scene->transforms = memory_system_alloc(memory_system, sizeof(Transform) * MAX_TRANSFORM_COUNT,
                                            MEMORY_SUBSYSTEM_RESOURCE);
    scene->world_transforms = memory_system_alloc(memory_system, sizeof(mat4) * MAX_TRANSFORM_COUNT,
                                            MEMORY_SUBSYSTEM_RESOURCE);

    scene->transform_count = 0;

    MASSERT(scene);

    return scene;
}

bool scene_shutdown(Scene* scene, Memory_System* memory_system)
{
    MASSERT(scene);
    memory_system_memory_free(memory_system, scene->transforms);
    memory_system_memory_free(memory_system, scene);

    scene = NULL;
    return true;
}

Transform_Handle scene_get_new_mesh_transform(Scene* scene)
{
    MASSERT(scene);
    Transform_Handle out_handle = {
        .handle = scene->transform_count,
        .gen = 0,
    };
    scene->transforms[scene->transform_count];
    scene->transform_count++;

    return out_handle;
}

void scene_update(Scene* scene, Resource_System* resource_system)
{
    //generate the world space matrix, and that also takes care of all the local transforms
    //NOTE: might not be the most efficient updating transforms this way,
    // I should profile sorting by parents and then by children, and see if that is more efficient
    // also I technically should know the max children any given object should have (thats not a skeletal mesh and its bones)
    for (int i = 0; i < scene->transform_count; ++i)
    {
        scene->world_transforms[i] = transform_get_world(scene->transforms);
    }

    resource_system->render_packet->transform_data_packet.world_space_matrix_array = scene->world_transforms;
    resource_system->render_packet->transform_data_packet.world_space_matrix_count = scene->transform_count;

}

