#ifndef SCENE_H
#define SCENE_H

#include "resource_types.h"


#define MAX_TRANSFORM_COUNT 1000

Scene* scene_init(Memory_System* memory_system);

bool scene_shutdown(Scene* scene, Memory_System* memory_system);



bool scene_get_new_transform(Scene* scene, Transform_Handle* transform_handle, MADNESS_UUID asset_uuid);

void scene_update(Scene* scene, Asset_System* resource_system);

void scene_save(Scene* scene);
void scene_load(Scene* scene, String* scene_to_open);


void scene_load_mesh(Asset_System* asset_system, const char* engine_path);


#endif //SCENE_H
