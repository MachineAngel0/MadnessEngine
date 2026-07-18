#include "material_system.h"

Material_System* material_system_init(Memory_System* memory_system)
{
    Material_System* material_system = memory_system_alloc(memory_system, sizeof(Material_System),
                                                           MEMORY_SUBSYSTEM_RESOURCE);

    material_system->pbr_count = 0;


    material_system->heap_allocator = memory_system_heap_allocator_create(
        memory_system, MB(1), MEMORY_SUBSYSTEM_RESOURCE);

    material_system->reflection_system = reflection_system_init(memory_system);
    reflection_system_parse(material_system->reflection_system, "../resource/material_types.h",
                            REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(material_system->reflection_system, "../resource/material_types.h", REFLECTION_PARSE_ENUM);
    reflection_system_parse(material_system->reflection_system, "../resource/material_types.h",
                            REFLECTION_PARSE_STRUCT);


    reflection_data_to_files(material_system->reflection_system, "material",
                             "../resource/generated/mat_enums.h",
                             "../resource/generated/mat_structs.h");

    material_system->reflection_registry = reflection_registry_init(memory_system);
    generate_runtime_enums_material(material_system->reflection_registry);
    generate_runtime_structs_material(material_system->reflection_registry);


    reflection_registry_debug_print_info(material_system->reflection_registry);


    material_system_instantiate_material(material_system, "mesh", "Material_Default",
                                         Shader_Mesh_Type_Mesh, Shader_Blend_Mode_Default,
                                         Shader_Pass_Type_Opaque | Shader_Pass_Type_Shadow);
    material_system_instantiate_material(material_system, "skinned_mesh", "Material_Default",
                                         Shader_Mesh_Type_Skinned, Shader_Blend_Mode_Default,
                                         Shader_Pass_Type_Opaque | Shader_Pass_Type_Shadow);



    return material_system;
}

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system)
{
    MASSERT(material_system);

    memory_system_memory_free(memory_system, material_system, MEMORY_SUBSYSTEM_RESOURCE);

    return true;
}

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_3D* render_packet_3d)
{
    render_packet_3d->prb = material_system->prb;
    render_packet_3d->prb_count = MAX_DEFAULT_MATERIAL;
    render_packet_3d->prb_bytes = sizeof(Material_Default) * MAX_DEFAULT_MATERIAL;

    render_packet_3d->oqaque_batch = material_system->oqaque_batch;
    render_packet_3d->oqaque_batch_count = material_system->oqaque_batch_count;


    return true;
}

/*Material_Handle material_system_create_material(Material_System* material_system)
{
    return (Material_Handle){material_system->pbr_count++};
}*/


Material_Default* material_system_create_default_pbr(Material_System* material_system, Material_Handle* material_handle)
{
    material_handle->handle = material_system->pbr_count;
    Material_Default* pbr = &material_system->prb[material_system->pbr_count++];
    material_system_pbr_init(pbr);
    return pbr;
}

Material_Default* material_system_pbr_get(Material_System* material_system, Material_Handle material_handle)
{
    return &material_system->prb[material_handle.handle];
}

void material_system_pbr_init(Material_Default* out_data)
{
    out_data->color = glms_vec4_one();
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
