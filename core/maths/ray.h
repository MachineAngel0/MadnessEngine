#ifndef RAY_H
#define RAY_H

#include <float.h>

#include "maths/math_types.h"
#include "maths/math_lib.h"
#include "maths/sphere.h"
#include "maths/AABB.h"
#include "maths/triangle.h"

// P = point
// LINE: p(t) = (1-t)P1 + P2
// OR
// LINE/RAY: p(t) = P1 + t(P2-P1)

//U = q-p

// point p
// direction v


typedef struct Ray
{
    vec3 origin;
    vec3 direction;
}Ray;


MINLINE bool ray_intersect_sphere(Ray* ray, Sphere* sphere, float* out_t)
{
    //TODO: this is probably right handed and should be left handed

    // Vector from ray origin to sphere center
    vec3 ray_origin_to_sphere_center = vec3_sub(sphere->center, ray->origin);

    // Quadratic equation coefficients
    float a = vec3_dot(ray->direction, ray->direction);  // Always 1 if direction is normalized
    float b = 2.0f * vec3_dot(ray_origin_to_sphere_center, ray->direction);
    float c = vec3_dot(ray_origin_to_sphere_center, ray_origin_to_sphere_center) - sphere->radius * sphere->radius;

    // Discriminant
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        // No intersection
        return false;
    }

    // Find the nearest intersection point
    float sqrtDiscriminant = sqrt(discriminant);
    float t0 = (-b - sqrtDiscriminant) / (2 * a);
    float t1 = (-b + sqrtDiscriminant) / (2 * a);

    // Check if intersection is in front of the ray
    if (t0 > 0) {
        out_t = t0;
        return true;
    }

    if (t1 > 0) {
        out_t = t1;
        return true;
    }

    // Both intersections are behind the ray
    return false;
}

bool ray_intersects_aabb(const Ray ray, const AABB3 aabb, float* out_tMin, float* out_tMax) {
    // Compute intersection with each slab
    vec3 invDir = (vec3){1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z};
    vec3 t0 = vec3_mul(vec3_sub(aabb.min,ray.origin), invDir);
    vec3 t1 = vec3_mul(vec3_sub(aabb.max,ray.origin), invDir);

    // Handle negative directions
    vec3 tSmaller = vec3_min(t0, t1);
    vec3 tBigger = vec3_max(t0, t1);

    // Find entry and exit points
    *out_tMin = max_f(tSmaller.x, max_f(tSmaller.y, tSmaller.z));
    *out_tMax = max_f(tBigger.x, max_f(tBigger.y, tBigger.z));

    // Check if there's a valid intersection
    return out_tMax >= out_tMin && out_tMax > 0;
}


bool ray_intersects_triangle(const Ray ray, const Triangle triangle, float* t, vec2 barycentricCoords) {
    // Möller–Trumbore algorithm
    vec3 edge1 = vec3_sub(triangle.v1, triangle.v0);
    vec3 edge2 = vec3_sub(triangle.v2, triangle.v0);
    vec3 h = vec3_cross(ray.direction, edge2);
    float a = vec3_dot(edge1, h);

    // Check if ray is parallel to triangle
    if (a > -0.00001f && a < 0.00001f) {
        return false;
    }

    float f = 1.0f / a;
    vec3 s = vec3_sub(ray.origin, triangle.v0);
    float u = f * vec3_dot(s, h);

    // Check if intersection is outside triangle
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    vec3 q = vec3_cross(s, edge1);
    float v = f * vec3_dot(ray.direction, q);

    // Check if intersection is outside triangle
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    // Compute intersection distance
    *t = f * vec3_dot(edge2, q);

    // Check if intersection is behind the ray
    if (*t <= 0.0f) {
        return false;
    }

    // Store barycentric coordinates for interpolation
    barycentricCoords = (vec2){u, v};
    return true;
}


//idk what these have to do with rays
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
