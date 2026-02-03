#ifndef AABB_H
#define AABB_H

#include <float.h>

#include "math_types.h"
#include "math_lib.h"
#include "../../../../../AppData/Local/Programs/CLion/bin/mingw/x86_64-w64-mingw32/include/stdlib.h"

//NOTE: i could rename this to bounding box
typedef struct AABB3
{
    //stands for axially aligned bounding box.
    //Axially aligned bounding boxes have the restriction that their sides be perpendicular to principal axes.
    vec3 min;
    vec3 max;
} AABB3;


inline void aabb_empty(AABB3* aabb)
{
    aabb->min.x = aabb->min.y = aabb->min.z = FLT_MAX;
    aabb->max.x = aabb->max.y = aabb->max.z = -FLT_MAX;
}

inline void aabb_add(AABB3* aabb, const vec3* p)
{
    if (p->x < aabb->min.x) { aabb->min.x = p->x; }
    if (p->x > aabb->max.x) { aabb->max.x = p->x; }
    if (p->y < aabb->min.x) { aabb->min.y = p->y; }
    if (p->y > aabb->max.x) { aabb->max.y = p->y; }
    if (p->z < aabb->min.x) { aabb->min.z = p->z; }
    if (p->z > aabb->max.x) { aabb->max.z = p->z; }
}


inline vec3 aabb_center(const AABB3* aabb)
{
    vec3 intermediate = vec3_add(aabb->min, aabb->max);
    vec3 center = vec3_div_scalar(intermediate, 2.0f);

    return center;
}

inline vec3 aabb_size(const AABB3* aabb)
{
    return vec3_sub(aabb->max, aabb->min);
}

inline vec3 aabb_radius_vector(const AABB3* aabb)
{
    vec3 temp = vec3_sub(aabb->max, aabb_center(aabb));
    return vec3_div_scalar(temp, 2.0f);
}

void bounding_box_create()
{
    // Our list of points
    const int vec3_points = 0;
    vec3 list[vec3_points];

    // First, empty the box
    //TODO: REPLACE MALLOC
    AABB3* box = malloc(sizeof(AABB3));
    aabb_empty(box);

    // Add each point into the box
    for (int i = 0; i < vec3_points; ++i)
    {
        aabb_add(box, &list[i]);
    }
}

/*
void aabb3_setToTransformedBox(const AABB3 &box, const Matrix4x3 &m) {

    // Start with the last row of the matrix, which is the translation
    // portion, i.e. the location of the origin after transformation.
    min = max = getTranslation(m);

    //
    // Examine each of the 9 matrix elements
    // and compute the new AABB
    //

    if (m.m11 > 0.0f) {
        min.x += m.m11 * box.min.x; max.x += m.m11 * box.max.x;
    } else {
        min.x += m.m11 * box.max.x; max.x += m.m11 * box.min.x;
    }

    if (m.m12 > 0.0f) {
        min.y += m.m12 * box.min.x; max.y += m.m12 * box.max.x;
    } else {
        min.y += m.m12 * box.max.x; max.y += m.m12 * box.min.x;
    }

    if (m.m13 > 0.0f) {
        min.z += m.m13 * box.min.x; max.z += m.m13 * box.max.x;
    } else {
        min.z += m.m13 * box.max.x; max.z += m.m13 * box.min.x;
    }

    if (m.m21 > 0.0f) {
        min.x += m.m21 * box.min.y; max.x += m.m21 * box.max.y;
    } else {
        min.x += m.m21 * box.max.y; max.x += m.m21 * box.min.y;
    }

    if (m.m22 > 0.0f) {
        min.y += m.m22 * box.min.y; max.y += m.m22 * box.max.y;
    } else {
        min.y += m.m22 * box.max.y; max.y += m.m22 * box.min.y;
    }

    if (m.m23 > 0.0f) {
        min.z += m.m23 * box.min.y; max.z += m.m23 * box.max.y;
    } else {
        min.z += m.m23 * box.max.y; max.z += m.m23 * box.min.y;
    }

    if (m.m31 > 0.0f) {
        min.x += m.m31 * box.min.z; max.x += m.m31 * box.max.z;
    } else {
        min.x += m.m31 * box.max.z; max.x += m.m31 * box.min.z;
    }

    if (m.m32 > 0.0f) {
        min.y += m.m32 * box.min.z; max.y += m.m32 * box.max.z;
    } else {
        min.y += m.m32 * box.max.z; max.y += m.m32 * box.min.z;
    }

    if (m.m33 > 0.0f) {
        min.z += m.m33 * box.min.z; max.z += m.m33 * box.max.z;
    } else {
        min.z += m.m33 * box.max.z; max.z += m.m33 * box.min.z;
    }
}*/


#endif //AABB_H
