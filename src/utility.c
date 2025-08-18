#include "minirt.h"

// Dithering function for masking color banding artifacts. The specific method
// is called Interleaved Gradient Noise; there are some good articles on it on
// the web.

t_vec3	dither(float x, float y)
{
	x *= 0.06711056f;
	y *= 0.00583715f;
	x = fract(52.9829189f * fract(x + y)) / 255.0f;
	return (vec3(x, x, x));
}

// Convert an angle from degrees to radians.

float	radians(float degrees)
{
	return (degrees * (M_PI / 180.0f));
}

// Solve the quadratic equation ax² + bx + c. Returns the lower of the two
// roots, or if the lower root is negative, returns the higher root. If no
// solution exists, returns a huge float value.

float	solve_quadratic(float a, float b, float c)
{
	const float	d = b * b - 4.0f * a * c;

	if (d < 0.0f)
		return (1e9f);
	return ((copysignf(sqrtf(d), b + sqrtf(d)) - b) / (2.0f * a));
}
