#ifndef BEZIER_H
#define BEZIER_H


vec2s quadratic_bezier(const vec2s p0, const vec2s p1, const vec2s p2, const float t)
{
    //De Casteljau's algorithm: B(t) = (1-t)² · P0  +  2(1-t)t · P1  +  t² · P2

    /*
    float u = 1.0f - t;
    float uu = u * u;
    float ut = 2.0f * u * t;
    float tt = t * t;

    vec2 result;
    result.x = uu * p0.x + ut * p1.x + tt * p2.x;
    result.y = uu * p0.y + ut * p1.y + tt * p2.y;
    return result;
     */

    //slight more efficient, do to reoccuring values, whihc can be cached
    //Polynomial Version Formula:  B(t) = (P0 - 2P1 + P2)t²  +  (-2P0 + 2P1)t  +  P0

    vec2s a = {p0.x - 2 * p1.x + p2.x, p0.y - 2 * p1.y + p2.y};
    vec2s b = {-2 * p0.x + 2 * p1.x, -2 * p0.y + 2 * p1.y};

    vec2s result;
    result.x = a.x * t * t + b.x * t + p0.x;
    result.y = a.y * t * t + b.y * t + p0.y;
    return result;
}

vec2s* quadratic_bezier_sample(Allocator* arena, vec2s p0, vec2s p1, vec2s p2, u32 steps)
{
    //the out size is the step size
    vec2s* out_bezier_points = allocator_alloc(arena, sizeof(vec2s) * steps);
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / (float)steps;
        out_bezier_points[i] = quadratic_bezier(p0, p1, p2, t);
    }
    return out_bezier_points;
}


vec2s cubic_bezier(const vec2s p0, const vec2s p1, const vec2s p2, const vec2s p3, const float t)
{
    //Polynomial Version =  p0 + t(-3P0 + 3P1) + t² (3P0 - 6P1 + 3P2) + t³ (-P0 + 3P1 - 3P2 + P3)

    vec2s p0_negative = glms_vec2_scale(p0, -1);
    vec2s p0x3 = glms_vec2_scale(p0, 3);
    vec2s p0x3_negative = glms_vec2_scale(p0x3, -1);
    vec2s p1x3 = glms_vec2_scale(p1, 3);
    vec2s p1x6_negative = glms_vec2_scale(glms_vec2_scale(p1, 6), -1);
    vec2s p2x3 = glms_vec2_scale(p2, 3);
    vec2s p2x3_negative = glms_vec2_scale(p2x3, -1);

    float t2 = t * t;
    float t3 = t * t * t;

    //t(-3P0 + 3P1)
    vec2s part2 = glms_vec2_scale(glms_vec2_add(p0x3_negative, p1x3), t);
    // t² (3P0 - 6P1 + 3P2)
    vec2s part3 = glms_vec2_scale(glms_vec2_add(glms_vec2_add(p0x3, p1x6_negative), p2x3), t2);
    // t³ (-P0+3P1-3P2+P3)
    vec2s part4 = glms_vec2_scale(glms_vec2_add(glms_vec2_add(glms_vec2_add(p0_negative, p1x3), p2x3_negative), p3), t3);

    return glms_vec2_add(glms_vec2_add(glms_vec2_add(p0, part2), part3), part4);
}

vec2s* cubic_bezier_sample(Allocator* arena, vec2s p0, vec2s p1, vec2s p2, vec2s p3, u32 steps)
{
    //the out size is the step size
    vec2s* out_bezier_points = allocator_alloc(arena, sizeof(vec2s) * steps);
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / (float)steps;
        out_bezier_points[i] = cubic_bezier(p0, p1, p2, p3, t);
    }
    return out_bezier_points;
}


#endif //BEZIER_H
