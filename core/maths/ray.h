#ifndef RAY_H
#define RAY_H

#include "maths/math_types.h"
#include "maths/math_lib.h"

// P = point
// LINE: p(t) = (1-t)P1 + P2
// OR
// LINE/RAY: p(t) = P1 + t(P2-P1)

//U = q-p

// point p
// direction v

float distance_of_point_to_line(const vec3 q, const vec3 p, const vec3 v)
{
    // d = sqrt[ u^2((u*v)^2 / v^2) ] or
    // d = sqrt[ (u*v)^2 / v^2 ]
    const vec3 a = vec3_cross(vec3_sub(q, p), v);
    return sqrt(vec3_dot(a, a) / vec3_dot(v, v));
}


float distance_of_point_to_point(const vec3 p1, const vec3 v1, const vec3 p2, const vec3 v2)
{
    vec3 dp = vec3_sub(p2, p1);

    float v12 = vec3_dot(v1, v1);
    float v22 = vec3_dot(v2, v2);
    float v1v2 = vec3_dot(v1, v2);

    float det = v1v2 * v22 - v12 * v12;

    if (abs(det) > FLT_MIN)
    {
        det = 1.0f / det;

        float dpv1 = vec3_dot(dp, v1);
        float dpv2 = vec3_dot(dp, v2);
        float t1 = (v1v2 * dpv2 - v12 * dpv1) * det;
        float t2 = (v12 * dpv2 - v1v2 * dpv1) * det;

        // dp + (v2 * t2) - (v1 * t1)
        vec3 v1t1 = vec3_mul_scalar(v1, t1);
        vec3 v2t2 = vec3_mul_scalar(v2, t2);
        vec3 dpv2t2 = vec3_add(dp, v2t2);
        vec3 dpv2t2v1t1 = vec3_sub(dpv2t2, v1t1);
        return vec3_length(dpv2t2v1t1);
    }

    //lines are nearly parallel
    const vec3 a = vec3_cross(dp, v1);
    return sqrt(vec3_dot(a,a) / v12);
}

#endif //RAY_H
