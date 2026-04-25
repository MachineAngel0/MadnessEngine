#include "material_system.h"

Material_System* material_system_init(Memory_System* memory_system)
{
    Material_System* material_system = memory_system_alloc(memory_system, sizeof(Material_System),
                                                           MEMORY_SUBSYSTEM_RESOURCE);


    return material_system;
}

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system)
{
    MASSERT(material_system);

    memory_system_memory_free(memory_system, material_system, MEMORY_SUBSYSTEM_RESOURCE);

    return true;
}

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_Material* render_packet_material)
{
    render_packet_material->material_instance = material_system->material_instance;
    render_packet_material->material_instance_count = MAX_MATERIAL;
    render_packet_material->material_instance_bytes = sizeof(Material_Instance) * MAX_MATERIAL;

    render_packet_material->prb = material_system->prb;
    render_packet_material->prb_count = MAX_MATERIAL;
    render_packet_material->prb_bytes = sizeof(Material_PBR) * MAX_MATERIAL;

    render_packet_material->uv_anim = material_system->uv_anim;
    render_packet_material->uv_anime_count = MAX_MATERIAL;
    render_packet_material->uv_anime_bytes = sizeof(Material_UV_Anim_Data) * MAX_MATERIAL;

    return true;
}

Material_Handle material_system_create_material(Material_System* material_system)
{
    return (Material_Handle){material_system->internal_count++};
}

void material_system_enable_flag(Material_System* material_system, Material_Handle material_handle,
                                 Material_Flag flags)
{
    Material_Instance* inst = &material_system->material_instance[material_handle.handle];
    inst->flags |= flags;
}

void material_system_disable_flag(Material_System* material_system, Material_Handle material_handle,
                                  Material_Flag flags)
{
    Material_Instance* inst = &material_system->material_instance[material_handle.handle];
    inst->flags &= flags;

}

Material_PBR* material_system_add_pbr(Material_System* material_system, Material_Handle material_handle)
{
    Material_Instance* inst = &material_system->material_instance[material_handle.handle];

    Material_PBR* pbr = &material_system->prb[material_system->pbr_count];

    material_system_pbr_init(pbr);

    inst->flags |= MATERIAL_FLAG_PBR;
    inst->pbr_idx = material_system->pbr_count;

    material_system->pbr_count++;
    return pbr;
}

Material_PBR* material_system_pbr_get(Material_System* material_system, Material_Handle material_handle)
{
    Material_Instance* inst = &material_system->material_instance[material_handle.handle];
    return &material_system->prb[inst->pbr_idx];
}

void material_system_pbr_init(Material_PBR* out_data)
{
    out_data->color = vec4_one();
    out_data->ambient_strength = 1.0f;
    out_data->roughness_strength = 1.0f;
    out_data->metallic_strength = 1.0f;
    out_data->normal_strength = 1.0f;
    out_data->ambient_occlusion_strength = 1.0f;
    out_data->emissive_strength = 1.0f;

    out_data->color_index = 0;
    out_data->normal_index = 0;
    out_data->metallic_index = 0;
    out_data->roughness_index = 0;
    out_data->ambient_occlusion_index = 0;
    out_data->emissive_index = 0;
}
