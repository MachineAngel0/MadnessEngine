#include "scene.h"

Scene* scene_init(Memory_System* memory_system)
{
    Scene* scene = memory_system_alloc(memory_system, sizeof(Scene), MEMORY_SUBSYSTEM_RESOURCE);

    scene->transforms = memory_system_alloc(memory_system, sizeof(Transform) * MAX_TRANSFORM_COUNT,
                                            MEMORY_SUBSYSTEM_RESOURCE);
    scene->world_transforms = memory_system_alloc(memory_system, sizeof(mat4s) * MAX_TRANSFORM_COUNT,
                                                  MEMORY_SUBSYSTEM_RESOURCE);

    scene->asset_uuid = memory_system_alloc(memory_system, sizeof(Transform) * MAX_TRANSFORM_COUNT,
                                        MEMORY_SUBSYSTEM_RESOURCE);

    scene->transform_count = 0;
    for (int i = 0; i < MAX_TRANSFORM_COUNT; ++i)
    {
        transform_set_default(&scene->transforms[i]);
    }


    MASSERT(scene);

    return scene;
}

bool scene_shutdown(Scene* scene, Memory_System* memory_system)
{
    MASSERT(scene);
    memory_system_memory_free(memory_system, scene->transforms, MEMORY_SUBSYSTEM_RESOURCE);
    memory_system_memory_free(memory_system, scene, MEMORY_SUBSYSTEM_RESOURCE);

    scene = NULL;
    return true;
}

bool scene_get_new_transform(Scene* scene, Transform_Handle* transform_handle, MADNESS_UUID asset_uuid)
{
    MASSERT(scene);
    *transform_handle = (Transform_Handle){
        .handle = scene->transform_count,
        .gen = 0,
    };

    scene->asset_uuid[scene->uuid_counts] = asset_uuid;

    scene->transforms[scene->transform_count];
    scene->transform_count++;
    scene->uuid_counts++;

    return true;
}

void scene_update(Scene* scene, Asset_System* resource_system)
{
    //generate the world space matrix, and that also takes care of all the local transforms
    //NOTE: might not be the most efficient updating transforms this way,
    // I should profile sorting by parents and then by children, and see if that is more efficient
    // also I technically should know the max children any given object should have (thats not a skeletal mesh and its bones)
    for (int i = 0; i < scene->transform_count; ++i)
    {
        scene->world_transforms[i] = transform_get_world(&scene->transforms[i]);
    }
}

void scene_save(Scene* scene)
{
    FILE* fptr = fopen(string_to_c_string(scene->scene_name), "wb");

    fwrite(&scene->uuid_counts, sizeof(scene->uuid_counts), 1, fptr);
    for (u32 i = 0; i < scene->uuid_counts; ++i)
    {
        madness_uuid_serialize(scene->asset_uuid[i], fptr);
    }
}

void scene_load(Scene* scene, String* scene_to_open)
{
    FILE* fptr = fopen(string_to_c_string(scene_to_open), "rb");

    if (!fptr)
    {
        WARN("SCENE LOAD: INVALID SCENE NAME");
        return;
    }


    fread(&scene->uuid_counts, sizeof(scene->uuid_counts), 1, fptr);
    scene->asset_uuid = malloc(sizeof(MADNESS_UUID) * scene->uuid_counts);
    for (u32 i = 0; i < scene->uuid_counts; ++i)
    {
        madness_uuid_deserialize(&scene->asset_uuid[i], fptr);
    }

    scene->scene_name = string_duplicate(scene_to_open);

}



