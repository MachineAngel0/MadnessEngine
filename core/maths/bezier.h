#ifndef BEZIER_H
#define BEZIER_H


vec2 quadratic_bezier(const vec2 p0, const vec2 p1, const vec2 p2, const float t)
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

    vec2 a = {p0.x - 2 * p1.x + p2.x, p0.y - 2 * p1.y + p2.y};
    vec2 b = {-2 * p0.x + 2 * p1.x, -2 * p0.y + 2 * p1.y};

    vec2 result;
    result.x = a.x * t * t + b.x * t + p0.x;
    result.y = a.y * t * t + b.y * t + p0.y;
    return result;
}

vec2* quadratic_bezier_sample(Arena* arena, vec2 p0, vec2 p1, vec2 p2, u32 steps)
{
    //the out size is the step size
    vec2* out_bezier_points = arena_alloc(arena, sizeof(vec2) * steps);
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / (float)steps;
        out_bezier_points[i] = quadratic_bezier(p0, p1, p2, t);
    }
    return out_bezier_points;
}


vec2 cubic_bezier(const vec2 p0, const vec2 p1, const vec2 p2, const vec2 p3, const float t)
{
    //Polynomial Version =  p0 + t(-3P0 + 3P1) + t² (-3P0 + 6P1+3P2) + t³ (-P0+3P1-3P2+P3)
    //Polynomial Version =  p0 + 3t(-P0 + P1) + t² *3(-P0 + 2P1 + P2) + t³ (-P0+3P1-3P2+P3)??????

    vec2 p0x3 = vec2_mul_scalar(p0, 3);
    vec2 p0x3_negative = vec2_mul_scalar(p0x3, -1);
    vec2 p1x3 = vec2_mul_scalar(p1, 3);
    vec2 p1x6 = vec2_mul_scalar(p1, 6);
    vec2 p2x3 = vec2_mul_scalar(p2, 3);

    float t2 = t * t;
    float t3 = t2 * t;

    //t(-3P0 + 3P1)
    vec2 part2 = vec2_add(p0, p1x3);
    // t² (-3P0 + 6P1+3P2)
    vec2 part3 = vec2_mul_scalar(vec2_add(vec2_add(p0x3_negative, p1x6), p2x3), t2);
    // t³ (-P0+3P1-3P2+P3)
    vec2 part4 = vec2_mul_scalar(vec2_add(vec2_add(vec2_add(p0x3_negative, p1x3), p2x3), p3), t3);

    return vec2_add(vec2_add(vec2_add(p0, part2), part3), part4);
}

vec2* cubic_bezier_sample(Arena* arena, vec2 p0, vec2 p1, vec2 p2, vec2 p3, u32 steps)
{
    //the out size is the step size
    vec2* out_bezier_points = arena_alloc(arena, sizeof(vec2) * steps);
    for (int i = 0; i <= steps; i++)
    {
        float t = (float)i / (float)steps;
        out_bezier_points[i] = cubic_bezier(p0, p1, p2, p3, t);
    }
    return out_bezier_points;
}


#endif //BEZIER_H
