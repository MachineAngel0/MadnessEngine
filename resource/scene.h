#ifndef SCENE_H
#define SCENE_H

#include "resource_types.h"

typedef struct Mesh_Prefab_Asset
{
    MADNESS_UUID* mesh_uuid;
    Mesh_Handle* mesh_handle;
} Mesh_Prefab_Asset;

typedef struct Scene_Mesh
{
    MADNESS_UUID* mesh_uuid;
    //overrides
    Transform* transform;
    Material_Asset* material; // should the prefab override the material? or should the mesh instance do that?
} Scene_Mesh;


struct Scene_File
{
    //basically gather everything that is active and dump its meta data into the scene file and then load it back up
    //how we structure our scene
    // lights
    // meshes (dynamic and static)
    // particles
    u32 directional_light_count;
    u32 spot_light_count;
    u32 point_light_count;

    Directional_Light* directional_light;
    Spot_Light* spot_light;
    Point_Light* point_light;

    MADNESS_UUID* meshes;
    MADNESS_UUID* skinned_meshes;


    bool unimplemented;
};


Scene* scene_init(Memory_System* memory_system);

bool scene_shutdown(Scene* scene, Memory_System* memory_system);


bool scene_get_new_transform(Scene* scene, Transform_Handle* transform_handle, MADNESS_UUID asset_uuid);
Transform* scene_get_transform(Scene* scene, Transform_Handle handle);

void scene_update(Scene* scene, Asset_System* resource_system);

void scene_save(Scene* scene);
void scene_load(Scene* scene, String* scene_to_open);


void scene_load_mesh(Asset_System* asset_system, const char* engine_path);


#endif //SCENE_H
