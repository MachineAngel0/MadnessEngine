#include "mesh_system.h"

#include "animation_system.h"
#include "cgltf.h"
#include "resource_types.h"
#include "ufbx.h"


Mesh_System* mesh_system_init(Asset_System* resource_system, Memory_System* memory_system)
{
    Mesh_System* out_mesh_system = memory_system_alloc(memory_system, sizeof(Mesh_System), MEMORY_SUBSYSTEM_MESH);
    memset(out_mesh_system, 0, sizeof(Mesh_System));


    out_mesh_system->mesh_ring_queue = ring_queue_create(sizeof(Mesh_GPU_Upload), MAX_MESH_COUNT);
    out_mesh_system->skinned_mesh_ring_queue = ring_queue_create(sizeof(Skinned_Mesh_GPU_Upload), MAX_SKINNED_MESH_COUNT);


    out_mesh_system->madness_mesh_count = 0;
    out_mesh_system->mesh_instance_count = 0;

    out_mesh_system->madness_sk_mesh_count = 0;
    out_mesh_system->skinned_mesh_instance_count = 0;

    INFO("MESH SYSTEM CREATED");

    return out_mesh_system;
}

bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system)
{
    MASSERT(mesh_system);
    memory_system_memory_free(memory_system, mesh_system, MEMORY_SUBSYSTEM_MESH);

    mesh_system = NULL;

    return true;
}


