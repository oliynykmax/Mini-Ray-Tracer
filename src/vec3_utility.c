#include "minirt.h"

// Convenience function for making a 3D vector from xyz-coordinates.

t_vec3	vec3(float x, float y, float z)
{
	return ((t_vec3){{x, y, z}});
}

// Do a linear interpolation between two 3D vectors. Returns a if t = 0, b if
// t = 1, and a point on the line between a and b when 0 ≤ t ≤ 1.

t_vec3	vec3_lerp(t_vec3 a, t_vec3 b, float t)
{
	a = vec3_scale(a, 1.0f - t);
	b = vec3_scale(b, 0.0f + t);
	return (vec3_add(a, b));
}

// Convert a color vector with components in [0, 1] to a 32-bit MLX pixel value.
// Applies an approximation of an sRGB gamma curve before converting to 8-bit
// range.

uint32_t	vec3_to_color(t_vec3 color)
{
	const float		gamma = 1.0f / 2.3f;
	const uint32_t	r = 255.5f * saturate(powf(color.r, gamma));
	const uint32_t	g = 255.5f * saturate(powf(color.g, gamma));
	const uint32_t	b = 255.5f * saturate(powf(color.b, gamma));

	return ((r << 24) | (g << 16) | (b << 8) | 255);
}
