#ifndef TRIANGLES_H
#define TRIANGLES_H

#include "maths/math_types.h"
#include "maths/math_lib.h"


float triangle_area(float base, float height)
{
    return base * height / 2.0f;
}

float triangle_area_herons(vec3 p1, vec3 p2, vec3 p3)
{
    //calculate the length of the sides using the cross product
    //calculate edge1 and 2 then take the cross product then the magnitude/length divided by 2
    //formula: ||e1*e2||/2

    vec3 edge1 = vec3_sub(p3, p2);
    // vec3 edge2 = vec3_sub(p1, p3);
    vec3 edge3 =  vec3_sub(p2, p1);

    vec3 a = vec3_cross(edge1, edge3);
    return vec3_magnitude(a)/2.0f;
}


#endif //TRIANGLES_H
