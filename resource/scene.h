#ifndef SCENE_H
#define SCENE_H

#include "resource_types.h"


#define MAX_TRANSFORM_COUNT 1000

Scene* scene_init(Memory_System* memory_system)
{
    Scene* scene = memory_system_alloc(memory_system, sizeof(Scene), MEMORY_SUBSYSTEM_RESOURCE);

    scene->transforms = memory_system_alloc(memory_system, sizeof(Transform) * MAX_TRANSFORM_COUNT,
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
}


Transform_Handle scene_get_new_transform(Scene* scene)
{
    MASSERT(scene);
    Transform_Handle out_handle = {
        .handle = scene->transform_count
        .gen = 0,
    };
    scene->transforms[scene->transform_count];
    scene->transform_count++;

    return out_handle;
}


#endif //SCENE_H
