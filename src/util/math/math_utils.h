#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>

const double EPS = 1e-6;

inline long long factorial(long long n)
{
    long long res = 1;
    for (long long i = 1; i <= n; i++)
        res *= i;
    return res;
}

inline int compare_double(double a, double b)
{
    if (fabs(a - b) < EPS) return 0;

    return a < b ? -1 : 1;
}

#endif