#include "minirt.h"

// Clamp a value to the range [lower, upper].

float	clamp(float value, float lower, float upper)
{
	return (fmax(lower, fmin(upper, value)));
}

// Clamp a value to the range [0, 1].

float	saturate(float value)
{
	return (clamp(value, 0.0f, 1.0f));
}

// Convert an angle from degrees to radians.

float	radians(float degrees)
{
	return (degrees * (M_PI / 180.0f));
}
