#include "minirt.h"

float	clamp(float x, float lower, float upper)
{
	return (fmax(lower, fmin(upper, x)));
}

float	radians(float degrees)
{
	return (degrees * (M_PI / 180.0f));
}
