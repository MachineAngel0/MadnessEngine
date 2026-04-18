#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H

#include "resource_types.h"


Material_System* material_system_init(Memory_System* memory_system);

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system);

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_Material* render_packet_material);

//you have to create a material before requesting an add material type to it
Material_Handle material_system_create_material(Material_System* material_system);

void material_system_enable_flag(Material_System* material_system, Material_Handle material_handle, Material_Flag flags);
void material_system_disable_flag(Material_System* material_system, Material_Handle material_handle,
                                  Material_Flag flags);

Material_PBR* material_system_add_pbr(Material_System* material_system, Material_Handle material_handle);
Material_PBR* material_system_pbr_get(Material_System* material_system, Material_Handle material_handle);


void material_system_pbr_init(Material_PBR* out_data);


#endif //MATERIAL_SYSTEM_H
