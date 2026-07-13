#ifndef MATH_TYPES_H
#define MATH_TYPES_H


#include "defines.h"

#define CGLM_ALL_UNALIGNED
#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
// #define CGLM_FORCE_LEFT_HANDED

#include <cglm/cglm.h>
#include <cglm/struct.h>

//extensions for glm

MINLINE mat4s glms_build_local_matrix(vec3s translation, versors rotation, vec3s scale)
{
    /*mat4s m = glms_mat4_identity();
    m = glms_translate(m, translation);
    m = glms_quat_rotate(m, rotation);
    m = glms_scale(m, scale);
    return m;*/


    return glms_mat4_mul(glms_mat4_mul(glms_translate_make(translation), glms_quat_mat4(rotation)),
                         glms_scale_make(scale));
}

//divide by a scalar value
MINLINE vec2s glms_vec2_scale_div(vec2s vec, float scalar)
{
    vec2s out;
    glm_vec2_zero(out.raw);
    out.raw[0] = vec.raw[0] / scalar;
    out.raw[0] = vec.raw[1] / scalar;
    return out;
}


/* NOTE: not in use anymore
typedef union
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

} mvec2;


typedef union
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
} mvec3;


typedef union
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
} mvec4;


typedef union
{
    s32 elements[2];

    struct
    {
        union
        {
            s32 x, r, u;
        };

        union
        {
            s32 y, g, v;
        };
    };
} mvec2i;

typedef union
{
    s32 elements[3];

    struct
    {
        union
        {
            s32 x, r, u;
        };

        union
        {
            s32 y, g, v;
        };

        union
        {
            s32 z, b, w;
        };
    };
} mvec3i;

typedef union
{
    s32 elements[4];

    struct
    {
        union
        {
            s32 x, r, u;
        };

        union
        {
            s32 y, g, v;
        };

        union
        {
            s32 z, b;
        };

        union
        {
            s32 w, a;
        };
    };
} mvec4i;


typedef union
{
    float data[6];
    float n[2][2];
    mvec3 rows[2];
} mmat2;


//col major
typedef union
{
    float data[9];
    float n[3][3];
    mvec3 rows[3];
} mmat3;


// typedef union alignas(16) mat4_u // apparently this is a thing
typedef union
{
    float data[16];
    // alignas(16) float two_d_arr[4][4];
    mvec4 rows[4];
} mmat4;

//generally avoid typedef, only really makes sense for quat
typedef mvec4 mquat;
*/

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

#define DEG2RAD     (PI/180.0f)
#define RAD2DEG     (180.0f / PI)

#define MS_TO_SEC(x) (1000.0f * x)
#define SEC_TO_MS(x) (x / 1000.0f)
#define INF	    1e30f // largest value available

#endif //MATH_TYPES_H
