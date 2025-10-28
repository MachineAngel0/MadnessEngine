#ifndef MATH_TYPES_H
#define MATH_TYPES_H

#include "defines.h"

typedef struct vec2f
{
    f32 elements[2];

    struct
    {
        union
        {
            f32 x, r, u;
        };

        union
        {
            f32 y, g, v;
        };
    };
} vec2;

typedef struct vec3f
{
    f32 elements[3];

    struct
    {
        union
        {
            f32 x, r, u;
        };

        union
        {
            f32 y, g, v;
        };

        union
        {
            f32 z, b, w;
        };
    };
} vec3;

typedef struct vec4f
{
    f32 elements[4];

    struct
    {
        union
        {
            f32 x, r, u;
        };

        union
        {
            f32 y, g, v;
        };

        union
        {
            f32 z, b;
        };

        union
        {
            f32 w, a;
        };
    };
} vec4;

typedef vec4 quat;

typedef struct vec2i
{
    i32 elements[2];

    struct
    {
        union
        {
            i32 x, r, u;
        };

        union
        {
            i32 y, g, v;
        };
    };
} vec2i;

typedef struct vec3i
{
    i32 elements[3];

    struct
    {
        union
        {
            i32 x, r, u;
        };

        union
        {
            i32 y, g, v;
        };

        union
        {
            i32 z, b, w;
        };
    };
} vec3i;

typedef struct vec4i
{
    i32 elements[4];

    struct
    {
        union
        {
            i32 x, r, u;
        };

        union
        {
            i32 y, g, v;
        };

        union
        {
            i32 z, b;
        };

        union
        {
            i32 w, a;
        };
    };
} vec4i;


#endif //MATH_TYPES_H
