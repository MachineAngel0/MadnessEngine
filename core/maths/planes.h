#ifndef PLANES_H
#define PLANES_H

#include "maths/math_types.h"
#include "maths/math_lib.h"

//(dot) = dot product (*)

// P = 3d Point
// N = normal vector
// Q = any set of points such that:
// N (dot) (Q - P) = 0
// NOTE: you can think of Q-P, as a line/line segment/ ray that goes out perpendicularly from N

//PLANE EQUATION: Ax + By + Cx + D = 0
// ABC are the xyz coordinates of the normal plane (N)
// D = -N (dot) P

//NOTE: its useful to describe a plane as a 4d vector as (N,D) normal(vec2) and the distance


MINLINE void plane_distance_from_point(const vec3 normal_plane, const vec3 point, float* out_distance)
{
    //if d = 0, point Q lies in the plane
    //if d > 0, point Q lies on the positive side of the plane
    //if d < 0, point Q lies on the negative side of the plane

    // a = q*n(normalized) - d
    // a = distance
    // q = point that is not on the plane
    // d = distnace fo

    *out_distance = vec3_dot(normal_plane, point);
    *out_distance = -(*out_distance);
}




MINLINE void vec4_plane(const vec3 v1, const vec3 v2, const vec3 v3, vec4* out_plane)
{
    //get the edges
    vec3 edge3 = vec3_sub(v2, v1);
    vec3 edge1 = vec3_sub(v3, v2);
    // vec3 edge2 = vec3_sub(v3, v1);

    //cross product for the plane/perpendicular
    vec3 normal = vec3_cross(edge3, edge1);
    vec3_normalize(&normal);

    //TODO: could be a memcpy
    out_plane->x = normal.x;
    out_plane->y = normal.y;
    out_plane->z = normal.z;

    //distance
    plane_distance_from_point(normal, v1, &out_plane->w);
}


//p stands for point
MINLINE void normal_vector_of_triangle(vec3 p1, vec3 p2, vec3 p3, vec3* out_normal)
{
    *out_normal = vec3_cross(vec3_sub(p1, p2), vec3_sub(p3, p1));
}

//used for things like polygons with many connecting triangles
vec3 normal_vector_compute_best_fit(const vec3 v[], size_t number_of_vec3)
{
    //basically just get the a
    // Zero out sum
    vec3 result = vec3_zero;

    // Start with the ``previous'' vertex as the last one.
    // This avoids an if-statement in the loop
    const vec3* p = &v[number_of_vec3 - 1];

    // Iterate through the vertices
    for (int i = 0; i < number_of_vec3; ++i)
    {
        // Get shortcut to the ``current'' vertex
        const vec3* c = &v[i];

        // Add in edge vector products appropriately
        result.x += (p->z + c->z) * (p->y - c->y);
        result.y += (p->x + c->x) * (p->z - c->z);
        result.z += (p->y + c->y) * (p->x - c->x);

        // Next vertex, please
        p = c;
    }

    // Normalize the result and return it
    vec3_normalize(&result);
    return result;
}

//returns the new normal when a vec3 is transformed by a matrix
MINLINE vec3 new_normal_from_matrix_transformation(const vec3 normal, const mat3 transformation_matrix)
{
    return mat3_mul_vec3(mat3_inverse(transformation_matrix), normal);
}

#endif //PLANES_H
