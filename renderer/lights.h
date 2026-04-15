#ifndef LIGHTS_H
#define LIGHTS_H


Light_System* light_system_init(Renderer* renderer);


void light_system_update(Renderer* renderer, Light_System* light_system, vulkan_command_buffer* transfer_command_buffer);

void directional_light_init(Directional_Light* light);
void point_light_init(Point_Light* light);
void spot_light_init(Spot_Light* light);


#endif //LIGHTS_H
