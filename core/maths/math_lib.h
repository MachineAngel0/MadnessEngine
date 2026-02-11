#ifndef MATH_LIB_H
#define MATH_LIB_H


#include <math.h>
#include "platform.h"

//FUTURE: rn im not using glm/cglm, should probably switch
//NOTE: Y Up, left handed
// Left-handed system: +X right, +Y up, +Z forward (into the screen)
// Column Based Matrix's: meaning we go from top to bottom (for each column), then move onto the right (to the new row)

// VEC2
MINLINE vec2 vec2_zero()
{
    return (vec2){0.0f, 0.0f};
}

MINLINE vec2 vec2_one()
{
    return (vec2){1.0f, 1.0f};
}

MINLINE vec2 vec2_up()
{
    return (vec2){0.0f, 1.0f};
}

MINLINE vec2 vec2_down()
{
    return (vec2){0.0f, -1.0f};
}

MINLINE vec2 vec2_left()
{
    return (vec2){-1.0f, 0.0f};
}

MINLINE vec2 vec2_right()
{
    return (vec2){1.0f, 0.0f};
}


MINLINE vec2 vec2_add(const vec2 a, const vec2 b)
{
    //its important you dont get rid of the paranthesis
    return (vec2){a.x + b.x, a.y + b.y};
}

MINLINE vec2 vec2_sub(const vec2 a, const vec2 b)
{
    return (vec2){a.x - b.x, a.y - b.y};
}

MINLINE vec2 vec2_mul(const vec2 a, const vec2 b)
{
    return (vec2){a.x * b.x, a.y * b.y};
}

MINLINE vec2 vec2_div(const vec2 a, const vec2 b)
{
    return (vec2){a.x / b.x, a.y / b.y};
}

MINLINE vec2 vec2_div_scalar(const vec2 a, float scalar)
{
    return (vec2){a.x / scalar, a.y / scalar};
}


MINLINE float vec2_length_squared(const vec2 v)
{
    return (v.x * v.x) + (v.y * v.y);
}

MINLINE float vec2_length(const vec2 v)
{
    return sqrt((v.x * v.x) + (v.y * v.y));
}

MINLINE void vec2_normalize(vec2* v)
{
    const float length = vec2_length(*v);
    v->x /= length;
    v->y /= length;
}

MINLINE vec2 vec2_normalize_functional(const vec2 v)
{
    const float length = vec2_length(v);
    return (vec2){v.x / length, v.y / length};
}

MINLINE bool vec2_compare(const vec2 a, const vec2 b, const float tolerance)
{
    if (abs(a.x - b.x) > tolerance)
    {
        return false;
    }
    if (abs(a.y - b.y) > tolerance)
    {
        return false;
    }
    return true;
}

MINLINE float vec2_distance(const vec2 a, const vec2 b)
{
    const vec2 temp = {a.x - b.x, a.y - b.y};
    return vec2_length(temp);
}

// VEC3
MINLINE vec3 vec3_zero()
{
    return (vec3){0.0f, 0.0f, 0.0f};
}

MINLINE vec3 vec3_one()
{
    return (vec3){1.0f, 1.0f, 1.0f};
}


MINLINE vec3 vec3_up()
{
    return (vec3){0.0f, 1.0f, 0.0f};
}

MINLINE vec3 vec3_down()
{
    return (vec3){0.0f, -1.0f, 0.0f};
}

MINLINE vec3 vec3_left()
{
    return (vec3){-1.0f, 0.0f, 0.0f};
}

MINLINE vec3 vec3_right()
{
    return (vec3){1.0f, 0.0f, 0.0f};
}

MINLINE vec3 vec3_forward()
{
    return (vec3){0.0f, 0.0f, 1.0f};
}

MINLINE vec3 vec3_back()
{
    return (vec3){0.0f, 0.0f, -1.0f};
}

MINLINE vec3 vec3_add(const vec3 a, const vec3 b)
{
    //its important you dont get rid of the paranthesis
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

MINLINE vec3 vec3_sub(const vec3 a, const vec3 b)
{
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

MINLINE vec3 vec3_mul(const vec3 a, const vec3 b)
{
    return (vec3){a.x * b.x, a.y * b.y, a.z * b.z};
}


MINLINE vec3 vec3_mul_scalar(const vec3 a, const float scalar)
{
    return (vec3){a.x * scalar, a.y * scalar, a.z * scalar};
}

MINLINE vec3 vec3_flip_sign(const vec3 a)
{
    return vec3_mul_scalar(a, -1.0f);
}


MINLINE vec3 vec3_div(const vec3 a, const vec3 b)
{
    return (vec3){a.x / b.x, a.y / b.y, a.z / b.z};
}

MINLINE vec3 vec3_div_scalar(const vec3 a, const float s)
{
    return (vec3){a.x / s, a.y / s, a.z / s};
}


MINLINE float vec3_dot(const vec3 a, const vec3 b)
{
    float result = 0.0f;
    result += a.x * b.x;
    result += a.y * b.y;
    result += a.z * b.z;
    return result;
}

MINLINE vec3 vec3_cross(const vec3 a, const vec3 b)
{
    return (vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

MINLINE vec3 vec3_projection(const vec3 a, const vec3 b)
{
    // b * (dot(a,b) / dot(b,b))
    const float dot_div_val = vec3_dot(a, b) / vec3_dot(b, b);
    return vec3_mul_scalar(b, dot_div_val);
}

MINLINE vec3 vec3_negation(const vec3 a, const vec3 b)
{
    // a - b * (dot(a,b) / dot(b,b))
    const float dot_div_val = vec3_dot(a, b) / vec3_dot(b, b);
    const vec3 vec_sub_val = vec3_sub(a, b);
    return vec3_mul_scalar(vec_sub_val, dot_div_val);
}


MINLINE float vec3_length_squared(const vec3 v)
{
    return ((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

MINLINE float vec3_length(const vec3 v)
{
    return sqrt(vec3_length_squared(v));
}

MINLINE float vec3_magnitude(const vec3 v)
{
    return vec3_length(v);
}
MINLINE float vec3_magnitude_squared(const vec3 v)
{
    return vec3_length_squared(v);
}


MINLINE void vec3_normalize(vec3* v)
{
    const float length = vec3_length(*v);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

MINLINE vec3 vec3_normalize_functional(const vec3 v)
{
    const float length = vec3_length(v);
    return (vec3){
        v.x / length,
        v.y / length,
        v.z / length
    };
}

MINLINE bool vec3_compare(const vec3 a, const vec3 b, const float tolerance)
{
    if (abs(a.x - b.x) > tolerance)
    {
        return false;
    }
    if (abs(a.y - b.y) > tolerance)
    {
        return false;
    }
    if (abs(a.z - b.z) > tolerance)
    {
        return false;
    }
    return true;
}

MINLINE float vec3_distance(const vec3 a, const vec3 b)
{
    const vec3 temp = {a.x - b.x, a.y - b.y, a.z - b.z};
    return vec3_length(temp);
}


//VEC4

MINLINE vec4 vec4_zero()
{
    return (vec4){0.0f, 0.0f, 0.0f, 0.0f};
}

MINLINE vec4 vec4_one()
{
    return (vec4){1.0f, 1.0f, 1.0f, 1.0f,};
}

MINLINE vec4 vec4_add(const vec4 a, const vec4 b)
{
    vec4 result;
    for (u64 i = 0; i < 4; i++)
    {
        result.elements[i] = a.elements[i] + b.elements[i];
    }
    return result;
}

MINLINE vec4 vec4_sub(const vec4 a, const vec4 b)
{
    vec4 result;
    for (u64 i = 0; i < 4; i++)
    {
        result.elements[i] = a.elements[i] - b.elements[i];
    }
    return result;
}

MINLINE vec4 vec4_mul(const vec4 a, const vec4 b)
{
    vec4 result;
    for (u64 i = 0; i < 4; i++)
    {
        result.elements[i] = a.elements[i] * b.elements[i];
    }
    return result;
}

MINLINE vec4 vec4_div(const vec4 a, const vec4 b)
{
    vec4 result;
    for (u64 i = 0; i < 4; i++)
    {
        result.elements[i] = a.elements[i] / b.elements[i];
    }
    return result;
}

MINLINE float vec4_length_squared(const vec4 v)
{
    return (v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w);
}

MINLINE float vec4_length(const vec4 v)
{
    return sqrt(vec4_length_squared(v));
}

MINLINE void vec4_normalize(vec4* v)
{
    const float length = vec4_length(*v);
    v->x /= length;
    v->y /= length;
    v->z /= length;
    v->w /= length;
}

MINLINE vec4 vec4_normalize_functional(const vec4 v)
{
    const float length = vec4_length(v);
    return (vec4){
        v.x / length,
        v.y / length,
        v.z / length,
        v.w / length
    };
}

MINLINE f32 vec4_dot(const vec4 v1, const vec4 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}


MINLINE f32 vec4_dot_f32(const f32 a0, const f32 a1, const f32 a2, const f32 a3,
                         const f32 b0, const f32 b1, const f32 b2, const f32 b3)
{
    return (a0 * b0) + (a1 * b1) + (a2 * b2) + (a3 * b3);
}


// MATRIX3*3

MINLINE mat3 mat3_identity()
{
    mat3 out_mat3;
    // mat3 out_mat3 = {0};
    memset(&out_mat3, 0, sizeof(float) * 9);
    out_mat3.data[0] = 1.0f;
    out_mat3.data[4] = 1.0f;
    out_mat3.data[8] = 1.0f;
    return out_mat3;
}

MINLINE mat3 mat3_init(const float n00, const float n01, const float n02,
                       const float n10, const float n11, const float n12,
                       const float n20, const float n21, const float n22)
{
    //col major
    mat3 out_mat3;

    out_mat3.data[0] = n00;
    out_mat3.data[1] = n10;
    out_mat3.data[2] = n20;

    out_mat3.data[3] = n01;
    out_mat3.data[4] = n11;
    out_mat3.data[5] = n21;

    out_mat3.data[6] = n02;
    out_mat3.data[7] = n12;
    out_mat3.data[8] = n22;


    //NOTE: same thing as above

    // out_mat3.n[0][0] = n00;
    // out_mat3.n[0][1] = n10;
    // out_mat3.n[0][2] = n20;
    //
    // out_mat3.n[1][0] = n01;
    // out_mat3.n[1][1] = n11;
    // out_mat3.n[1][2] = n21;
    //
    // out_mat3.n[2][0] = n02;
    // out_mat3.n[2][1] = n12;
    // out_mat3.n[2][2] = n22;

    return out_mat3;
}

MINLINE mat3 mat3_vec3_init(const vec3 a, const vec3 b, const vec3 c)
{
    //col major
    mat3 out_mat3;

    out_mat3.data[0] = a.x;
    out_mat3.data[1] = a.y;
    out_mat3.data[2] = a.z;

    out_mat3.data[3] = b.x;
    out_mat3.data[4] = b.y;
    out_mat3.data[5] = b.z;

    out_mat3.data[6] = c.x;
    out_mat3.data[7] = c.y;
    out_mat3.data[8] = c.z;

    //NOTE: same thing as above

    // out_mat3.n[0][0] = a->x;
    // out_mat3.n[0][1] = a->y;
    // out_mat3.n[0][2] = a->z;
    //
    // out_mat3.n[1][0] = b->x;
    // out_mat3.n[1][1] = b->y;
    // out_mat3.n[1][2] = b->z;
    //
    // out_mat3.n[2][0] = c->x;
    // out_mat3.n[2][1] = c->y;
    // out_mat3.n[2][2] = c->z;


    return out_mat3;
}

MINLINE mat3 mat3_mul(const mat3 m1, const mat3 m2)
{
    mat3 out_mat3 = mat3_identity();
    const float* m1_ptr = m1.data;
    const float* m2_ptr = m2.data;
    float* dst_ptr = out_mat3.data;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            *dst_ptr =
                m1_ptr[0] * m2_ptr[0 + j] +
                m1_ptr[1] * m2_ptr[4 + j] +
                m1_ptr[2] * m2_ptr[8 + j];
            dst_ptr++;
        }
        m1_ptr += 3;
    }

    return out_mat3;
}

MINLINE mat3 mat3_mul_alt(const mat3 m1, const mat3 m2)
{
    return (mat3){
        .data[0] = m1.data[0] * m2.data[0] + m1.data[1] * m2.data[3] + m1.data[2] + m1.data[6],
        .data[1] = m1.data[0] * m2.data[1] + m1.data[1] * m2.data[4] + m1.data[2] + m1.data[7],
        .data[2] = m1.data[0] * m2.data[2] + m1.data[1] * m2.data[5] + m1.data[2] + m1.data[8],

        .data[3] = m1.data[3] * m2.data[0] + m1.data[4] * m2.data[3] + m1.data[5] + m1.data[6],
        .data[4] = m1.data[3] * m2.data[1] + m1.data[4] * m2.data[4] + m1.data[5] + m1.data[7],
        .data[5] = m1.data[3] * m2.data[2] + m1.data[4] * m2.data[5] + m1.data[5] + m1.data[8],

        .data[6] = m1.data[6] * m2.data[0] + m1.data[7] * m2.data[3] + m1.data[8] + m1.data[6],
        .data[7] = m1.data[6] * m2.data[1] + m1.data[7] * m2.data[4] + m1.data[8] + m1.data[7],
        .data[8] = m1.data[6] * m2.data[2] + m1.data[7] * m2.data[5] + m1.data[8] + m1.data[8],
    };
}

MINLINE vec3 mat3_mul_vec3(const mat3 m, const vec3 v)
{
    //this looks something like [x,y,z] [3*3] = vec3[x,y,z]
    //and not  [3*3] [x,y,z](vertical)  = mat3[3*3]
    return (vec3)
    {
        .x = (m.data[0] * v.x) + (m.data[1] * v.y) + (m.data[2] * v.z),
        .y = (m.data[3] * v.x) + (m.data[4] * v.y) + (m.data[5] * v.z),
        .z = (m.data[6] * v.x) + (m.data[7] * v.y) + (m.data[8] * v.z)
    };
}

MINLINE float mat3_determinant(const mat3 m)
{
    const float* a = m.data;
    return
        a[0] * (a[4] * a[8] - a[7] * a[5]) +
        a[3] * (a[7] * a[2] - a[1] * a[8]) +
        a[6] * (a[1] * a[5] - a[4] * a[2]);
}

MINLINE mat3 mat3_inverse(const mat3 m)
{
    const vec3 r0 = vec3_cross(m.rows[1], m.rows[2]);
    const vec3 r1 = vec3_cross(m.rows[2], m.rows[0]);
    const vec3 r2 = vec3_cross(m.rows[0], m.rows[1]);

    const float invdet = 1.0f / vec3_dot(r2, m.rows[2]);

    return (mat3){
        r0.x * invdet, r0.y * invdet, r0.z * invdet,
        r1.x * invdet, r1.y * invdet, r1.z * invdet,
        r2.x * invdet, r2.y * invdet, r2.z * invdet
    };
}

MINLINE mat3 mat3_make_rot_x(const float t)
{
    const float c = cos(t);
    const float s = sin(t);

    return (mat3){
        1.0f, 0.0f, 0.0f,
        0.0f, c, s,
        0.0f, -s, c,
    };
}

MINLINE mat3 mat3_make_rot_y(const float t)
{
    const float c = cos(t);
    const float s = sin(t);

    return (mat3){
        c, 0.0f, -s,
        0.0f, 1.0f, 0.0f,
        s, 0.0f, c,
    };
}

MINLINE mat3 mat3_make_rot_z(const float t)
{
    const float c = cos(t);
    const float s = sin(t);

    return (mat3){
        c, s, 0.0f,
        -s, c, 0.0f,
        0.0f, 0.0f, 1.0f,
    };
}

MINLINE mat3 mat3_make_rot(const float t, const vec3 a)
{
    const float c = cos(t);
    const float s = sin(t);
    const float d = 1.0f - c;

    const float x = a.x * d;
    const float y = a.y * d;
    const float z = a.z * d;

    const float axay = x * a.y;
    const float axaz = x * a.z;
    const float ayaz = y * a.z;


    return (mat3){
        c + x * a.x, axay + s * a.z, axaz - s * a.y,
        axay - s * a.z, c + y * a.y, ayaz + s * a.x,
        axaz + s * a.y, ayaz - s * a.x, c + z * a.z
    };
}

//NOTE: Careful on using this
MINLINE mat3 mat3_make_rot_arbitrary_axis(const float theta, const vec3 axis)
{
    //NOTE: its implied that we are rotating about the origin, that axis is a unit vector
    const float c = cos(theta);
    const float s = sin(theta);
    const float d = 1 - cos(theta);

    // vec3 normalized_axis = vec3_normalize_functional(axis);

    float nx = axis.x;
    float ny = axis.y;
    float nz = axis.z;

    float nx_square = pow(axis.x, 2);
    float ny_square = pow(axis.y, 2);
    float nz_square = pow(axis.z, 2);


    return (mat3){
        nx_square * d + c, (nx * ny * d) + (nz * s), nx * nz * d - ny * s,
        nx * ny * d - nz * s, (ny_square * d) + (c), ny * nz * d + nz * s,
        nx * nz * d - ny * s, (ny * nz * d) - nz * s, nz_square * d + c,
    };
}


MINLINE mat3 mat3_make_reflection(const vec3 a)
{
    const float x = a.x * -2.0f;
    const float y = a.y * -2.0f;
    const float z = a.z * -2.0f;

    const float axay = x * a.y;
    const float axaz = x * a.z;
    const float ayaz = y * a.z;


    return (mat3){
        x * a.x + 1.0f, axay, axaz,
        axay, y * a.y + 1.0f, ayaz,
        axaz, ayaz, z * a.z + 1.0f
    };
}

MINLINE mat3 mat3_involution(const vec3 a)
{
    const float x = a.x * -2.0f;
    const float y = a.y * -2.0f;
    const float z = a.z * -2.0f;

    const float axay = x * a.y;
    const float axaz = x * a.z;
    const float ayaz = y * a.z;


    return (mat3){
        x * a.x - 1.0f, axay, axaz,
        axay, y * a.y - 1.0f, ayaz,
        axaz, ayaz, z * a.z - 1.0f
    };
}


MINLINE mat3 mat3_make_scale_float3(const float sx, const float sy, const float sz)
{
    return (mat3){
        sx, 0.0f, 0.0f,
        0.0f, sy, 0.0f,
        0.0f, 0.0f, sz
    };
}

MINLINE mat3 mat3_make_scale(float s, const vec3 a)
{
    s -= 1.0f;
    const float x = a.x * s;
    const float y = a.y * s;
    const float z = a.z * s;

    const float axay = x * a.y;
    const float axaz = x * a.z;
    const float ayaz = y * a.z;

    return (mat3){
        x * a.x + 1.0f, axay, axaz,
        axay, y * a.y + 1.0f, ayaz,
        axaz, ayaz, z * a.z + 1.0f
    };
}

MINLINE mat3 mat3_skew(float t, const vec3 a, const vec3 b)
{
    t = tan(t);
    const float x = a.x * t;
    const float y = a.y * t;
    const float z = a.z * t;

    return (mat3){
        x * b.x + 1.0f, x * b.y, x * b.z,
        y * b.x, y * b.y + 1.0f, y * b.z,
        z * b.x, z * b.y, z * b.z + 1.0f
    };
}


// MATRIX4

MINLINE void mat4_set_value(mat4 matrix, const u8 row, const u8 col, const float val)
{
    u8 stride = 4;
    matrix.data[(row * stride) + col] = val;
}


MINLINE mat4 mat4_zero()
{
    mat4 out_mat4 = {0};
    return out_mat4;
}

MINLINE mat4 mat4_identity()
{
    mat4 out_mat4;
    memset(&out_mat4, 0, sizeof(float) * 16);
    out_mat4.data[0] = 1.0f;
    out_mat4.data[5] = 1.0f;
    out_mat4.data[10] = 1.0f;
    out_mat4.data[15] = 1.0f;
    return out_mat4;
}

MINLINE mat4 mat4_mul(const mat4 m1, const mat4 m2)
{
    mat4 out_mat = mat4_identity();
    const float* m1_ptr = m1.data;
    const float* m2_ptr = m2.data;
    float* dst_ptr = out_mat.data;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            *dst_ptr =
                m1_ptr[0] * m2_ptr[0 + j] +
                m1_ptr[1] * m2_ptr[4 + j] +
                m1_ptr[2] * m2_ptr[8 + j] +
                m1_ptr[3] * m2_ptr[12 + j];
            dst_ptr++;
        }
        m1_ptr += 4;
    }

    return out_mat;
}


MINLINE mat4 mat4_orthographic(const f32 left, const f32 right, const f32 bottom, const f32 top, const f32 near_clip,
                               const f32 far_clip)
{
    mat4 out_mat = mat4_identity();

    const f32 lr = 1.0f / (right - left);
    const f32 bt = 1.0f / (top - bottom);
    const f32 nf = 1.0f / (far_clip - near_clip);

    out_mat.data[0] = -2.0f * lr;
    out_mat.data[5] = -2.0f * bt;
    out_mat.data[10] = 2.0f * nf;

    out_mat.data[12] = (left + right) * lr;
    out_mat.data[13] = (top + bottom) * bt;
    out_mat.data[14] = (far_clip + near_clip) * nf;
    return out_mat;
}

MINLINE mat4 mat4_perspective(const f32 fov_radians, const f32 aspect_ratio, const f32 near_clip,
                              const f32 far_clip)
{
    const f32 half_tan_fov = tan(fov_radians * 0.5f);
    mat4 out_mat4;
    memset(&out_mat4, 0, sizeof(float) * 16);

    out_mat4.data[0] = 1.0f / (aspect_ratio * half_tan_fov);
    out_mat4.data[5] = 1.0f / half_tan_fov;
    out_mat4.data[10] = -((far_clip + near_clip) / (far_clip - near_clip));
    out_mat4.data[11] = -1.0f;
    out_mat4.data[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));

    return out_mat4;
}


MINLINE mat4 mat4_look_at(const vec3 position, const vec3 target, const vec3 up)
{
    mat4 out_mat = mat4_zero();
    vec3 z_axis = vec3_zero();

    z_axis.x = target.x - position.x;
    z_axis.y = target.y - position.y;
    z_axis.z = target.z - position.z;

    z_axis = vec3_normalize_functional(z_axis);
    vec3 x_axis = vec3_normalize_functional(vec3_cross(up, z_axis));
    vec3 y_axis = vec3_cross(z_axis, up);

    out_mat.data[0] = x_axis.x;
    out_mat.data[1] = y_axis.x;
    out_mat.data[2] = -z_axis.x;
    out_mat.data[3] = 0;
    out_mat.data[4] = x_axis.y;
    out_mat.data[5] = y_axis.y;
    out_mat.data[6] = -z_axis.y;
    out_mat.data[7] = 0;
    out_mat.data[8] = x_axis.z;
    out_mat.data[9] = y_axis.z;
    out_mat.data[10] = -z_axis.z;
    out_mat.data[11] = 0;
    out_mat.data[12] = -vec3_dot(x_axis, target);
    out_mat.data[13] = -vec3_dot(y_axis, target);
    out_mat.data[14] = -vec3_dot(z_axis, target);
    out_mat.data[15] = 1.0f;


    return out_mat;
}

MINLINE mat4 mat4_inverse(const mat4 matrix)
{
    // const vec3 a = (vec3){m->rows[0].x, m->rows[0].y, m->rows[0].z};
    // const vec3 b = (vec3){m->rows[1].x, m->rows[1].y, m->rows[1].z};
    // const vec3 c = (vec3){m->rows[2].x, m->rows[2].y, m->rows[2].z};
    // const vec3 d = (vec3){m->rows[3].x, m->rows[3].y, m->rows[3].z};
    //
    // const float x = m->data[5];
    // const float y = m->data[6];
    // const float z = m->data[7];
    // const float w = m->data[8];
    //
    // vec3 s = vec3_cross(a, b);
    // vec3 t = vec3_cross(c, d);
    // // a * y - b * x
    // vec3 u = vec3_sub(vec3_mul_scalar(a, y), vec3_mul_scalar(b, x));
    // vec3 v = vec3_sub(vec3_mul_scalar(c, w), vec3_mul_scalar(d, z));
    //
    // float invDet = 1.0f / (vec3_dot(s, v) + vec3_dot(t, u));
    //
    // s = vec3_mul_scalar(s, invDet);
    // t = vec3_mul_scalar(t, invDet);
    // u = vec3_mul_scalar(u, invDet);
    // v = vec3_mul_scalar(v, invDet);
    //
    // vec3 r0 = vec3_add(vec3_cross(b, v), vec3_mul_scalar(t, y));
    // vec3 r1 = vec3_sub(vec3_cross(v, a), vec3_mul_scalar(t, x));
    // vec3 r2 = vec3_add(vec3_cross(d, u), vec3_mul_scalar(s, w));
    // vec3 r3 = vec3_sub(vec3_cross(u, c), vec3_mul_scalar(s, z));
    //
    // return (mat4){
    //     r0.x, r0.y, r0.z, vec3_dot(b, t),
    //     r1.x, r1.y, r1.z, vec3_dot(a, t),
    //     r2.x, r2.y, r2.z, -vec3_dot(d, s),
    //     r3.x, r3.y, r3.z, vec3_dot(c, s)
    // };
    //
    //
    const f32* m = matrix.data;


    f32 t0 = m[10] * m[15];
    f32 t1 = m[14] * m[11];
    f32 t2 = m[6] * m[15];
    f32 t3 = m[14] * m[7];
    f32 t4 = m[6] * m[11];
    f32 t5 = m[10] * m[7];
    f32 t6 = m[2] * m[15];
    f32 t7 = m[14] * m[3];
    f32 t8 = m[2] * m[11];
    f32 t9 = m[10] * m[3];
    f32 t10 = m[2] * m[7];
    f32 t11 = m[6] * m[3];
    f32 t12 = m[8] * m[13];
    f32 t13 = m[12] * m[9];
    f32 t14 = m[4] * m[13];
    f32 t15 = m[12] * m[5];
    f32 t16 = m[4] * m[9];
    f32 t17 = m[8] * m[5];
    f32 t18 = m[0] * m[13];
    f32 t19 = m[12] * m[1];
    f32 t20 = m[0] * m[9];
    f32 t21 = m[8] * m[1];
    f32 t22 = m[0] * m[5];
    f32 t23 = m[4] * m[1];

    mat4 out_matrix;
    f32* o = out_matrix.data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    f32 d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));


    return out_matrix;
}

MINLINE mat4 mat4_transpose(const mat4 matrix)
{
    mat4 out_matrix = mat4_identity();

    out_matrix.data[0] = matrix.data[0];
    out_matrix.data[1] = matrix.data[4];
    out_matrix.data[2] = matrix.data[8];
    out_matrix.data[3] = matrix.data[12];
    out_matrix.data[4] = matrix.data[1];
    out_matrix.data[5] = matrix.data[5];
    out_matrix.data[6] = matrix.data[9];
    out_matrix.data[7] = matrix.data[13];
    out_matrix.data[8] = matrix.data[2];
    out_matrix.data[9] = matrix.data[6];
    out_matrix.data[10] = matrix.data[10];
    out_matrix.data[11] = matrix.data[14];
    out_matrix.data[12] = matrix.data[3];
    out_matrix.data[13] = matrix.data[7];
    out_matrix.data[14] = matrix.data[11];
    out_matrix.data[15] = matrix.data[15];

    return out_matrix;
}


MINLINE mat4 mat4_translation(const vec3 position)
{
    mat4 out_matrix = mat4_identity();

    out_matrix.data[12] = position.x;
    out_matrix.data[13] = position.y;
    out_matrix.data[14] = position.z;


    return out_matrix;
}

MINLINE mat4 mat4_scale(const vec3 scale)
{
    mat4 out_matrix = mat4_identity();

    out_matrix.data[0] = scale.x;
    out_matrix.data[5] = scale.y;
    out_matrix.data[10] = scale.z;

    return out_matrix;
}


MINLINE mat4 mat4_euler_x(f32 angle_radians)
{
    mat4 out_matrix = mat4_identity();

    f32 c = cos(angle_radians);
    f32 s = sin(angle_radians);

    out_matrix.data[5] = c;
    out_matrix.data[6] = s;
    out_matrix.data[9] = -s;
    out_matrix.data[10] = c;

    return out_matrix;
}


MINLINE mat4 mat4_euler_y(const f32 angle_radians)
{
    mat4 out_matrix = mat4_identity();

    const f32 c = cos(angle_radians);
    const f32 s = sin(angle_radians);

    out_matrix.data[0] = c;
    out_matrix.data[2] = -s;
    out_matrix.data[8] = s;
    out_matrix.data[10] = c;

    return out_matrix;
}


MINLINE mat4 mat4_euler_z(const f32 angle_radians)
{
    mat4 out_matrix = mat4_identity();

    f32 c = cos(angle_radians);
    f32 s = sin(angle_radians);

    out_matrix.data[0] = c;
    out_matrix.data[1] = s;
    out_matrix.data[4] = -s;
    out_matrix.data[5] = c;

    return out_matrix;
}


MINLINE mat4 mat4_euler_xyz(const f32 x_radians, const f32 y_radians, const f32 z_radians)
{
    const mat4 rx = mat4_euler_x(x_radians);
    const mat4 ry = mat4_euler_y(y_radians);
    const mat4 rz = mat4_euler_z(z_radians);

    mat4 out_matrix = mat4_mul(rx, ry);
    out_matrix = mat4_mul(out_matrix, rz);

    return out_matrix;
}


MINLINE vec3 mat4_forward(const mat4 matrix)
{
    vec3 forward;

    forward.x = -matrix.data[2];
    forward.y = -matrix.data[6];
    forward.z = -matrix.data[10];

    vec3_normalize(&forward);

    return forward;
}


MINLINE vec3 mat4_backward(const mat4 matrix)
{
    vec3 backward;

    backward.x = matrix.data[2];
    backward.y = matrix.data[6];
    backward.z = matrix.data[10];

    vec3_normalize(&backward);

    return backward;
}


MINLINE vec3 mat4_up(const mat4 matrix)
{
    vec3 up;

    up.x = matrix.data[1];
    up.y = matrix.data[5];
    up.z = matrix.data[9];

    vec3_normalize(&up);

    return up;
}


MINLINE vec3 mat4_down(const mat4 matrix)
{
    vec3 down;

    down.x = -matrix.data[1];
    down.y = -matrix.data[5];
    down.z = -matrix.data[9];

    vec3_normalize(&down);
    return down;
}


MINLINE vec3 mat4_left(const mat4 matrix)
{
    vec3 right;

    right.x = -matrix.data[0];
    right.y = -matrix.data[4];
    right.z = -matrix.data[8];

    vec3_normalize(&right);

    return right;
}


MINLINE vec3 mat4_right(const mat4 matrix)
{
    vec3 left;

    left.x = matrix.data[0];
    left.y = matrix.data[4];
    left.z = matrix.data[8];

    vec3_normalize(&left);

    return left;
}


// QUATERNIONS
MINLINE quat quat_identity()
{
    return (quat){0, 0, 0, 1.0f};
}


MINLINE f32 quat_normal(const quat q)
{
    return (float)sqrt(
        q.x * q.x +
        q.y * q.y +
        q.z * q.z +
        q.w * q.w);
}

MINLINE quat quat_normalize(const quat q)
{
    const f32 normal = quat_normal(q);

    return (quat){
        q.x / normal,
        q.y / normal,
        q.z / normal,
        q.w / normal
    };
}


MINLINE quat quat_conjugate(const quat q)
{
    return (quat){
        -q.x,
        -q.y,
        -q.z,
        q.w
    };
}


MINLINE quat quat_inverse(const quat q)
{
    return quat_normalize(quat_conjugate(q));
}

MINLINE quat quat_exponential(const quat q, const f32 exponent)
{
    // q^t operation

    quat out_quat = q;

    // Check for the case of an identity quaternion.
    // This will protect against divide by zero
    if (fabs(out_quat.w) < .9999f)
    {
        // Extract the half angle alpha (alpha = theta/2)
        float alpha = acos(out_quat.w);

        // Compute new alpha value
        float newAlpha = alpha * exponent;

        // Compute new w value
        out_quat.w = cos(newAlpha);

        // Compute new xyz values
        float mult = sin(newAlpha) / sin(alpha);
        out_quat.x *= mult;
        out_quat.y *= mult;
        out_quat.z *= mult;
        return out_quat;
    }

    //this is here in the event w is less than 1
    return q;
}


MINLINE quat quat_mul(const quat q0, const quat q1)
{
    quat out_quaternion;

    out_quaternion.x = q0.w * q1.x + q0.x * q1.w + q0.y * q1.z - q0.z * q1.y;
    out_quaternion.y = q0.w * q1.y - q0.x * q1.z + q0.y * q1.w + q0.z * q1.x;
    out_quaternion.z = q0.w * q1.z + q0.x * q1.y - q0.y * q1.x + q0.z * q1.w;
    out_quaternion.w = q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z;

    return out_quaternion;
}


MINLINE f32 quat_dot(quat q_0, quat q_1)
{
    return (q_0.x * q_1.x) +
        (q_0.y * q_1.y) +
        (q_0.z * q_1.z) +
        (q_0.w * q_1.w);
}

// Calculates a rotation matrix based on the quaternion and the passed in center point.
MINLINE mat4 quat_to_rotation_matrix(quat q, vec3 center)
{
    mat4 out_matrix;

    f32* o = out_matrix.data;

    o[0] = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[1] = 2.0f * ((q.x * q.y) + (q.z * q.w));
    o[2] = 2.0f * ((q.x * q.z) - (q.y * q.w));
    o[3] = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];
    o[4] = 2.0f * ((q.x * q.y) - (q.z * q.w));
    o[5] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[6] = 2.0f * ((q.y * q.z) + (q.x * q.w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];
    o[8] = 2.0f * ((q.x * q.z) + (q.y * q.w));
    o[9] = 2.0f * ((q.y * q.z) - (q.x * q.w));
    o[10] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];
    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;

    return out_matrix;
}

// Creates a quaternion from the given axis and angle.
MINLINE quat quat_from_axis_angle(const vec3 axis, const f32 angle, const bool normalize)
{
    const f32 half_angle = 0.5f * angle;

    const f32 s = sin(half_angle);
    const f32 c = cos(half_angle);

    const quat q = (quat){s * axis.x, s * axis.y, s * axis.z, c};

    if (normalize)
    {
        return quat_normalize(q);
    }

    return q;
}

MINLINE mat4 quat_rotate(mat4 m, float angle, vec3 axis)
{
    quat rot = quat_from_axis_angle(axis, angle, false);
    return quat_to_rotation_matrix(rot, vec3_zero());
    // glm_rotate_make(rot, angle, axis);
    // glm_mul_rot(m, rot, m);
}

//  Calculates spherical linear interpolation of a given percentage between two quaternions.
MINLINE quat quat_slerp(const quat q_0, const quat q_1, const f32 percentage)
{
    //percentage is from 0-1

    quat out_quaternion;

    // Source: https://en.wikipedia.org/wiki/Slerp
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.

    quat v0 = quat_normalize(q_0);
    quat v1 = quat_normalize(q_1);

    // Compute the cosine of the angle between the two vectors.
    f32 dot = quat_dot(v0, v1);

    // If the dot product is negative, slerp won't take the shorter path.
    // Note that v1 and -v1 are equivalent when the negation is applied to all four components.
    // Fix by reversing one quaternion.

    if (dot < 0.0f)
    {
        v1.x = -v1.x;
        v1.y = -v1.y;
        v1.z = -v1.z;
        v1.w = -v1.w;
        dot = -dot;
    }

    const f32 DOT_THRESHOLD = 0.9995f;

    if (dot > DOT_THRESHOLD)
    {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        out_quaternion = (quat){
            v0.x + ((v1.x - v0.x) * percentage),
            v0.y + ((v1.y - v0.y) * percentage),
            v0.z + ((v1.z - v0.z) * percentage),
            v0.w + ((v1.w - v0.w) * percentage)
        };

        return quat_normalize(out_quaternion);
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    const f32 theta_0 = acos(dot); // theta_0 = angle between input vectors
    const f32 theta = theta_0 * percentage; // theta = angle between v0 and result
    const f32 sin_theta = sin(theta); // compute this value only once
    const f32 sin_theta_0 = sin(theta_0); // compute this value only once
    const f32 s0 = cos(theta) - dot * sin_theta / sin_theta_0; // == sin(theta_0 - theta) / sin(theta_0)
    const f32 s1 = sin_theta / sin_theta_0;

    return (quat){
        (v0.x * s0) + (v1.x * s1),
        (v0.y * s0) + (v1.y * s1),
        (v0.z * s0) + (v1.z * s1),
        (v0.w * s0) + (v1.w * s1)
    };
}

// DEG AND RAD
MINLINE f32 deg_to_rad(const f32 degrees)
{
    return degrees * DEG2RAD;
}

MINLINE f32 rad_to_deg(const f32 radians)
{
    return radians * RAD2DEG;
}

// CONVERSIONS
MINLINE vec2 vec3_to_vec2(const vec3 v)
{
    return (vec2){v.x, v.y};
}

MINLINE vec3 vec2_to_vec3(const vec2 v, const float z)
{
    return (vec3){v.x, v.y, z};
}

MINLINE vec3 vec4_to_vec3(const vec4 v)
{
    return (vec3){v.x, v.y, v.z};
}

MINLINE vec4 vec3_to_vec4(const vec3 v, const float w)
{
    return (vec4){v.x, v.y, v.z, w};
}


MINLINE mat4 quat_to_mat4(quat q)
{
    mat4 out_matrix = mat4_identity();

    // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix

    const quat n = quat_normalize(q);

    out_matrix.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    out_matrix.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    out_matrix.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;
    out_matrix.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    out_matrix.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    out_matrix.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;
    out_matrix.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    out_matrix.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    out_matrix.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return out_matrix;
}

MINLINE mat4 quat_to_euler(quat q, vec3 out_v)
{
    // Input quaternion
    float w, x, y, z;

    // Output Euler angles (radians)
    float h, p, b;

    // Extract sin(pitch)
    float sp = -2.0f * (q.y * q.z - q.w * q.x);

    // Check for Gimbal lock, giving slight tolerance
    // for numerical imprecision
    if (fabs(sp) > 0.9999f)
    {
        // Looking straight up or down
        p = 1.570796f * sp; // pi/2

        // Compute heading, slam bank to zero
        h = atan2(-q.x * q.z + q.w * q.y, 0.5f - q.y * q.y - q.z * q.z);
        b = 0.0f;
    }
    else
    {
        // Compute angles
        p = asin(sp);
        h = atan2(q.x * q.z + q.w * q.y, 0.5f - q.x * q.x - q.y * q.y);
        b = atan2(q.x * q.y + q.w * q.z, 0.5f - q.x * q.x - q.z * q.z);
    }
}


//UTILITY
void print_vec2(void* data)
{
    //cast to int* and then dereferenced, to get the value of the pointer of size int*
    printf("vec2 x: %f", *(float*)data);
    printf("y: %f\n", *(float*)((float*)data + 1));
}

void print_vec3(void* data)
{
    //cast to int* and then dereferenced, to get the value of the pointer of size int*
    printf("vec3 x: %f ", *(float*)data);
    printf("y: %f ", *(float*)((float*)data + 1));
    printf("z: %f\n", *(float*)((float*)data + 2));
}

void print_vec4(void* data)
{
    //cast to int* and then dereferenced, to get the value of the pointer of size int*
    printf("vec4 x: %f ", *(float*)data);
    printf("y: %f ", *(float*)((float*)data + 1));
    printf("z: %f ", *(float*)((float*)data + 2));
    printf("w: %f\n", *(float*)((float*)data + 3));
}

// POLAR COORDINATES
inline void polar_canonical_form(float in_radius, float in_theta)
{
    MASSERT_MSG_FALSE("DO NOT USE THIS FUNCTION")
    // POLAR COORDINATES

    // Radial distance
    float r = in_radius;

    // Angle in RADIANS
    float theta = in_theta;

    // Declare a constant for 2*pi (360 degrees)
    const float TWOPI = 2.0f * PI;

    // Check if we are exactly at the origin
    if (r == 0.0f)
    {
        // At the origin - slam theta to zero
        theta = 0.0f;
    }
    else
    {
        // Handle negative distance
        if (r < 0.0f)
        {
            r = -r;
            theta += PI;
        }

        // Theta out of range?  Note that this if() check is not
        // strictly necessary, but we try to avoid doing floating
        // point operations if they aren't necessary.  Why
        // incur floating point precision loss if we don't
        // need to?
        if (fabs(theta) > PI)
        {
            // Offset by PI
            theta += PI;

            // Wrap in range 0...TWOPI
            theta -= floor(theta / TWOPI) * TWOPI;

            // Undo offset, shifting angle back in range -PI...PI
            theta -= PI;
        }
    }
}

inline void polar_to_cartesian(float in_radius, float in_theta, float* out_x, float* out_y)
{
    MASSERT_MSG_FALSE("DO NOT USE THIS FUNCTION")
    // Theta Angle in RADIANS

    *out_x = in_radius * cos(in_theta);
    *out_y = in_radius * sin(in_theta);
}

inline void cartesian_to_polar(float in_x, float in_y, float* out_radius, float* out_theta)
{
    MASSERT_MSG_FALSE("DO NOT USE THIS FUNCTION")
    // Input: Cartesian coordinates
    // Output: polar radial distance, and angle in RADIANS


    // Check if we are at the origin
    if (in_x == 0.0f && in_y == 0.0f)
    {
        // At the origin - slam both polar coordinates to zero
        *out_radius = 0.0f;
        *out_theta = 0.0f;
    }
    else
    {
        // Compute values.  Isn't the atan2 function great?
        *out_radius = sqrt(in_x * in_x + in_y * in_y);
        *out_theta = atan2(in_y, in_x);
    }
}

inline void spherical_canonical_form(float* r, float* heading, float* pitch)
{
    // Radial distance
    // float r;

    // Angles in radians
    //heading = theta, pitch = phi
    // float heading, pitch;

    // Declare a few constants
    const float TWOPI = 2.0f * PI; // 360 degrees
    const float PIOVERTWO = PI / 2.0f; // 90 degrees

    // Check if we are exactly at the origin
    if (*r == 0.0f)
    {
        // At the origin - slam angles to zero
        *heading = *pitch = 0.0f;
    }
    else
    {
        // Handle negative distance
        if (*r < 0.0f)
        {
            *r = -(*r);
            *heading += PI;
            *pitch = -(*pitch);
        }

        // Pitch out of range?
        if (fabs(*pitch) > PIOVERTWO)
        {
            // Offset by 90 degrees
            *pitch += PIOVERTWO;

            // Wrap in range 0...TWOPI
            *pitch -= floor(*pitch / TWOPI) * TWOPI;

            // Out of range?
            if (*pitch > PI)
            {
                // Flip heading
                *heading += PI;

                // Undo offset and also set pitch = 180-pitch
                *pitch = 3.0f * PI / 2.0f - *pitch; // p = 270 degrees - p
            }
            else
            {
                // Undo offset, shifting pitch in range
                // -90 degrees ... +90 degrees
                *pitch -= PIOVERTWO;
            }
        }

        // Gimbal lock?  Test using a relatively small tolerance
        // here, close to the limits of single precision.
        if (fabs(*pitch) >= PIOVERTWO * 0.9999)
        {
            *heading = 0.0f;
        }
        else
        {
            // Wrap heading, avoiding math when possible
            // to preserve precision
            if (fabs(*heading) > PI)
            {
                // Offset by PI
                *heading += PI;

                // Wrap in range 0...TWOPI
                *heading -= floor(*heading / TWOPI) * TWOPI;

                // Undo offset, shifting angle back in range -PI...PI
                *heading -= PI;
            }
        }
    }
}

inline void spherical_to_cartesian(float* out_x, float* out_y, float* out_z,
                                   const float radius, const float theta, const float phi)
{
    //phi = books calls its p for pitch
    //theta = books calls it h for heading
    //think of both like airplane movements
    *out_x = radius * cos(phi) * sin(theta);
    *out_y = -radius * sin(phi);
    *out_z = -radius * cos(phi) * cos(theta);
}

inline void cartesian_to_spherical(const float x, const float y, const float z, float* out_radius, float* out_theta,
                                   float* out_phi)
{
    // Input Cartesian coordinates
    // float x,y,z;

    // Output radial distance
    // float r;

    // Output angles in radians
    // float heading, pitch;

    // Declare a few constants
    const float TWOPI = 2.0f * PI; // 360 degrees
    const float PIOVERTWO = PI / 2.0f; // 90 degrees

    // Compute radial distance
    *out_radius = sqrt(x * x + y * y + z * z);

    // Check if we are exactly at the origin
    if (*out_radius > 0.0f)
    {
        // Compute pitch
        *out_phi = asin(-y / *out_radius);

        // Check for gimbal lock, since the library atan2
        // function is undefined at the (2D) origin
        if (fabs(*out_phi) >= PIOVERTWO * 0.9999)
        {
            *out_theta = 0.0f;
        }
        else
        {
            *out_theta = atan2(x, z);
        }
    }
    else
    {
        // At the origin - slam angles to zero
        *out_theta = *out_phi = 0.0f;
    }
}


/*COLOR*/
static const vec3 COLOR_BLACK = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
static const vec3 COLOR_WHITE = {1.0f, 1.0f, 1.0f};
static const vec3 COLOR_RED = {1.0f, 0.0f, 0.0f};
static const vec3 COLOR_GREEN = {0.0f, 1.0f, 0.0f};
static const vec3 COLOR_BLUE = {0.0f, 0.0f, 1.0f};

static const vec4 COLOR_BLACK_V4 = {1.0f, 1.0f, 1.0f, 1.0f};
static const vec4 COLOR_WHITE_V4 = {1.0f, 1.0f, 1.0f, 1.0f};


/*** MAX/MIN FUNCTIONS ***/

int32_t max_i(const int32_t a, const int32_t b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

float max_f(const float a, const float b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

int32_t min_i(const int32_t a, const int32_t b)
{
    if (a < b)
    {
        return a;
    }
    return b;
}

float min_f(const float a, const float b)
{
    if (a < b)
    {
        return a;
    }
    return b;
}

/*** RANDOM ***/
static bool rand_seeded = false;

//TODO: rand func
uint32_t randi()
{
    if (!rand_seeded)
    {
        srand(platform_get_absolute_time());
        rand_seeded = true;
    }
    return rand();
}

uint32_t rand_range_i(uint32_t min, uint32_t max)
{
    if (!rand_seeded)
    {
        srand(platform_get_absolute_time());
        rand_seeded = true;
    }
    return (rand() % (max - min + 1)) + min;
}

static float randf()
{
    if (!rand_seeded)
    {
        srand((u32)platform_get_absolute_time());
        rand_seeded = true;
    }
    return (rand() / (f32)RAND_MAX);
}

float rand_range_f(const float min, const float max)
{
    if (!rand_seeded)
    {
        srand((u32)platform_get_absolute_time());
        rand_seeded = true;
    }
    return min + ((float)rand() / ((f32)RAND_MAX / (max - min)));
}


/*** CLAMP ***/

MINLINE int32_t clamp_int(const int32_t cur_val, const int32_t min, const int32_t max)
{
    if (cur_val > max)
    {
        return max;
    }
    if (cur_val < min)
    {
        return min;
    }
    return cur_val;
}

MINLINE float clamp_float(const float cur_val, const float min, const float max)
{
    if (cur_val > max)
    {
        return max;
    }
    if (cur_val < min)
    {
        return min;
    }
    return cur_val;
}

/*** EQUAL ***/

bool equal_f(const float a, const float b, float tolerance)
{
    // https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/

    //  Epsilon comparisons
    // return fabs(a - b) <= tolerance;
    // Relative Epsilon comparisons

    // Calculate the absolute difference.
    float diff = fabs(a - b);
    float A = fabs(a);
    float B = fabs(b);
    // Find the largest
    float largest = (B > A) ? B : A;

    //TODO: debug ifdef
    // printf("a: %f, b: %f, A; %f, B: %f, largest: %f, tolerance: %f\n", a, b, A, B, largest, tolerance);

    if (diff <= largest * tolerance) { return true; }
    return false;
}

bool equal_d(const double a, const double b, double tolerance)
{
    // https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/

    //  Epsilon comparisons
    // return fabs(a - b) <= tolerance;
    // Relative Epsilon comparisons

    // Calculate the absolute difference.
    double diff = fabs(a - b);
    double A = fabs(a);
    double B = fabs(b);
    // Find the largest
    double largest = (B > A) ? B : A;

    //TODO: debug ifdef
    // printf("a: %f, b: %f, A; %f, B: %f, largest: %f, tolerance: %f\n", a, b, A, B, largest, tolerance);

    if (diff <= largest * tolerance) { return true; }
    return false;
}

#define EQUAL_F(a, b) equal_f(a, b, FLT_EPSILON)
#define EQUAL_D(a, b) equal_d(a, b, FLT_EPSILON)
#endif //MATH_LIB_H
