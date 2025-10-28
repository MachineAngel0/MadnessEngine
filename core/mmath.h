#ifndef KMATH_H
#define KMATH_H

//TODO: literally everything
#include "math.h"
#include <float.h>

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


int32_t clamp_i(const int32_t a, const int32_t b)
{
    return a > b ? b : a;
}

float clamp_f(const float a, const float b)
{
    return a > b ? b : a;
}


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

static float randf()
{
    return (float) (rand()) / (float)(RAND_MAX);
}

#endif //KMATH_H
