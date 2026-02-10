#ifndef LIGHTS_H
#define LIGHTS_H


Light_System* light_system_init(renderer* renderer);

void directional_light_init(Directional_Light* light);
void point_light_init(Point_Light* light);
void spot_light_init(Spot_Light* light);


#endif //LIGHTS_H
