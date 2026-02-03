#ifndef SPHERE_H
#define SPHERE_H

#include "math_types.h"
#include "math_lib.h"


//NOTE:
// r = radius
// d = diameter

//2d circle

typedef struct circle
{
    vec2 center;
    float radius;
} circle;


//3d sphere

typedef struct sphere
{
    vec3 center;
    float radius;
} sphere;


MINLINE float distance_of_point_from_sphere_center(const sphere* s, vec3 point)
{
    //p is any point
    // c is the center
    // r is radius
    // || || magnitude
    // || p-c || = r

    vec3 a = vec3_sub(point, s->center);
    return vec3_magnitude(a) - s->radius;
}

MINLINE float sphere_diameter(const sphere* s)
{
    // 2*r
    return 2 * s->radius;
}
MINLINE float sphere_circumference(const sphere* s)
{
    //2*pi*r or 2*d
    return 2 * s->radius * PI;
}
MINLINE float sphere_area(const sphere* s)
{
    //pi*r^2
    return PI * s->radius * s->radius;
}



MINLINE float sphere_surface_area(const sphere* s)
{
    // 4 * pi  * r^2
    return (4) * PI * (s->radius * s->radius);
}

MINLINE float sphere_volume(const sphere* s)
{
    //4/3 * pi  * r^3
    return (4 / 3) * PI * (s->radius * s->radius * s->radius);
}


#endif //SPHERE_H
