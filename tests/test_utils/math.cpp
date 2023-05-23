#include <math.h>

#include "./math.h"

double get_mean(const double* data, size_t data_size)
{
    double sum = 0;
    for (size_t i = 0; i < data_size; ++i)
        sum += data[i];
    return sum / (double) data_size;
}

__always_inline
static double squared(double x) { return x * x; }

double get_stddev(const double* data, double mean, size_t data_size)
{
    double variance = 0;
    for (size_t i = 0; i < data_size; ++i)
        variance += squared(data[i] - mean);

    return sqrt(variance / (double)(data_size - 1));
}

double get_round_exponent(double value)
{
    const double exponent = pow(10, round(log10(fabs(value))));
    if (fabs(exponent) < 1e-9)
        return 1e9;
    const double mantissa = value / exponent;
    
    return (fabs(mantissa) < 0.3 ? 100 : 10) / exponent;
}

double round_with_exponent(double value, double exponent)
{
    const double val = round(value * exponent) / exponent;
    return isfinite(val) ? val : value;
}

