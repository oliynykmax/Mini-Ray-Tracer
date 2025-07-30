#ifndef INLINE_MATH_H
# define INLINE_MATH_H

# include <math.h>

// Clamp a value to the range [lower, upper].
static inline float	clamp(float value, float lower, float upper)
{
	return (fmax(lower, fmin(upper, value)));
}

// Clamp a value to the range [0, 1].
static inline float	saturate(float value)
{
	return (clamp(value, 0.0f, 1.0f));
}

// Convert an angle from degrees to radians.
static inline float	radians(float degrees)
{
	return (degrees * (M_PI / 180.0f));
}

// Get the fractional part of a floating point number. (Discard the digits
// before the decimal point.)
static inline float	fract(float x)
{
	return (x - floorf(x));
}

// Solve the quadratic equation ax² + bx + c. Returns the lower of the two
// roots, or if the lower root is negative, returns the higher root. If no
// solution exists, returns a huge float value.
static inline float	solve_quadratic(float a, float b, float c)
{
	const float	d = b * b - 4.0f * a * c;

	if (d < 0.0f)
		return (1e9f);
	return ((copysignf(sqrtf(d), b + sqrtf(d)) - b) / (2.0f * a));
}

#endif