#ifndef SCENE_H
#define SCENE_H

#include "resource_types.h"


#define MAX_TRANSFORM_COUNT 1000

Scene* scene_init(Memory_System* memory_system);

bool scene_shutdown(Scene* scene, Memory_System* memory_system);



Transform_Handle scene_get_new_mesh_transform(Scene* scene);

void scene_update(Scene* scene, Resource_System* resource_system);




#endif //SCENE_H
