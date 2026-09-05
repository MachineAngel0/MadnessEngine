#ifndef ASSET_SERIALIZATION_H
#define ASSET_SERIALIZATION_H

#include "resource_types.h"



//
MAPI bool asset_texture_serialize(Madness_Texture_Runtime* runtime, FILE* fptr);
MAPI bool asset_texture_deserialize(Madness_Texture_Runtime* runtime, FILE* fptr, Heap_Allocator* texture_memory_allocator);


MAPI bool asset_font_serialize(Madness_Font_Runtime* runtime, FILE* fptr);
MAPI bool asset_font_deserialize(Madness_Font_Runtime* runtime, FILE* fptr, Heap_Allocator* texture_memory_allocator);


//
//material defines the layout and shader definition of the object, instance is the specific data/params of that material
MAPI bool asset_material_asset_serialize(Material_Asset_Runtime* runtime, FILE* fptr);
MAPI bool asset_material_asset_deserialize(Material_Asset_Runtime* runtime, FILE* fptr,
                                          Heap_Allocator* allocator);

MAPI bool asset_material_instance_serialize(Material_Instance* instance, FILE* fptr);
MAPI bool asset_material_instance_deserialize(Material_Instance* instance, FILE* fptr, Heap_Allocator* allocator);


//
MAPI bool asset_mesh_serialize(Madness_Mesh_Runtime* runtime, FILE* fptr);
MAPI bool asset_mesh_deserialize(Madness_Mesh_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator);

//
MAPI bool asset_skmesh_serialize(Madness_SkMesh_Runtime* runtime, FILE* fptr);
MAPI bool asset_skmesh_deserialize(Madness_SkMesh_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator);



bool particle_emitter_serialize(Particle_Emitter* particle_emitter, FILE* fptr)
{
    fwrite(&particle_emitter->data, sizeof(Particle_Emitter_Data), 1, fptr);
    string_serialize(particle_emitter->name, fptr);
    asset_material_instance_serialize(&particle_emitter->material_instance, fptr);
    return true;
}

bool particle_emitter_deserialize(Asset_System* asset_system, Particle_Emitter* particle_emitter, FILE* fptr)
{
    fread(&particle_emitter->data, sizeof(Particle_Emitter_Data), 1, fptr);
    string_deserialize_heap(particle_emitter->name, fptr, asset_system->heap_allocator);
    asset_material_instance_deserialize(&particle_emitter->material_instance, fptr, asset_system->heap_allocator);
    return true;
}

bool particle_effect_serialize(Particle_Effect* particle_effect, FILE* fptr)
{
    fwrite(&particle_effect->effect_current_time, sizeof(u32), 1, fptr);
    fwrite(&particle_effect->effect_length, sizeof(u32),1,fptr);

    transform_serialize(&particle_effect->transform, fptr);

    // emitter location offset in relation to the particle effects base tramsform

    fwrite(particle_effect->emitter_position, sizeof(u32) * 4, 1, fptr);
    fwrite(particle_effect->emitters_start, sizeof(u32) * 4, 1, fptr);
    fwrite(particle_effect->emitters_end, sizeof(u32) * 4, 1, fptr);
    fwrite(&particle_effect->emitter_count, sizeof(u32), 1, fptr);

    for (u32 i = 0; i < particle_effect->emitter_count; i++)
    {
        madness_uuid_serialize(particle_effect->emmiter_uuid[i], fptr);
    }
    string_serialize(particle_effect->name, fptr);
    return true;
}

bool particle_effect_deserialize(Particle_Effect* particle_effect, FILE* fptr, Heap_Allocator* allocator)
{
    fread(&particle_effect->effect_current_time, sizeof(u32), 1, fptr);
    fread(&particle_effect->effect_length, sizeof(u32),1,fptr);

    transform_deserialize(&particle_effect->transform, fptr);

    // emitter location offset in relation to the particle effects base tramsform

    fread(particle_effect->emitter_position, sizeof(u32) * 4, 1, fptr);
    fread(particle_effect->emitters_start, sizeof(u32) * 4, 1, fptr);
    fread(particle_effect->emitters_end, sizeof(u32) * 4, 1, fptr);
    fread(&particle_effect->emitter_count, sizeof(u32), 1, fptr);

    for (u32 i = 0; i < particle_effect->emitter_count; i++)
    {
        madness_uuid_deserialize(&particle_effect->emmiter_uuid[i], fptr);
    }
    string_deserialize_heap(particle_effect->name, fptr, allocator);
    return true;
}



#endif
