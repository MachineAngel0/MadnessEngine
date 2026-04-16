#ifndef RESOURCE_SYSTEM_H
#define RESOURCE_SYSTEM_H

#include <stdbool.h>

#include "resource_types.h"
#include "ring_queue.h"




Resource_System* resource_system_init(Memory_System* memory_system);

bool resource_system_shutdown(Resource_System* resource_system, Memory_System* memory_system);
bool resource_system_update_and_create_render_packet(Resource_System* resource_system);

bool resource_system_generate_render_packet(Resource_System* resource_system);
MAPI void render_packet_clear(Render_Packet* renderer_packets);






#endif //RESOURCE_SYSTEM_H
