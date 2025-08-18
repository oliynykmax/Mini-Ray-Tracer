#include "minirt.h"

// Get the smaller of two integer values.

int	min(int a, int b)
{
	return (a * (a < b) + b * (a >= b));
}

// Get the larger of two integer values.

int	max(int a, int b)
{
	return (a * (a > b) + b * (a <= b));
}

// Clamp a floating point value to the range [lower, upper].

float	clamp(float value, float lower, float upper)
{
	return (fmax(lower, fmin(upper, value)));
}

// Clamp a value to the range [0, 1].

float	saturate(float value)
{
	return (clamp(value, 0.0f, 1.0f));
}

// Get the fractional part of a floating point number. (Discard the digits
// before the decimal point.)

float	fract(float x)
{
	return (x - floorf(x));
}
