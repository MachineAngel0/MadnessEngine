#ifndef MATH_TYPES_H
#define MATH_TYPES_H


#include "defines.h"


typedef union vec2f
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


typedef union vec3f
{
    f32 elements[3];

    struct
    {
        union
        {
            f32 x, r;
        };

        union
        {
            f32 y, g;
        };

        union
        {
            f32 z, b;
        };
    };
} vec3;


typedef union vec4f
{
    f32 elements[4];

    struct
    {
        union
        {
            f32 x, r;
        };

        union
        {
            f32 y, g;
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


typedef union vec2i
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

typedef union vec3i
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

typedef union vec4i
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


typedef union mat2_f
{
    float data[6];
    float n[2][2];
    vec3 rows[2];
} mat2;


//col major
typedef union mat3_f
{
    float data[9];
    float n[3][3];
    vec3 rows[3];
} mat3;


// typedef union alignas(16) mat4_u // apparently this is a thing
typedef union mat4_f
{
    float data[16];
    // alignas(16) float two_d_arr[4][4];
    vec4 rows[4];
} mat4;

//generally avoid typedef, only really makes sense for quat
typedef vec4 quat;


typedef enum Direction
{
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
} Direction;




#define MATH_E		2.7182818284590452354
#define LOG2E		1.4426950408889634074
#define LOG10E	    0.43429448190325182765
#define LN2		    0.69314718055994530942
#define LN10		2.30258509299404568402

#define PI		    3.14159265358979323846
#define PI_SQUARE	PI * 2
#define PI_SQRT_ROOT 1.77245385091
#define PI_HALF		1.57079632679489661923
#define PI_FOURTH	0.78539816339744830962

#define TWO_SQRT 1.414213562373095
#define THREE_SQRT 1.732050807568877

#define DEG2RAD     PI/180.0f
#define RAD2DEG     180.0f / PI

#define MS_TO_SEC(x) (1000.0f * x)
#define SEC_TO_MS(x) (x / 1000.0f)
#define INF	    1e30f // largest value available

#endif //MATH_TYPES_H
