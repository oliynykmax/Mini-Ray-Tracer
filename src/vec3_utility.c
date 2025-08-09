#include "minirt.h"

// Convenience function for making a 3D vector from xyz-coordinates.

t_vec3	vec3(float x, float y, float z)
{
	return ((t_vec3){{x, y, z}});
}

// Do a linear interpolation between two 3D vectors. Returns a if t = 0, b if
// t = 1, and a point on the line between a and b when 0 ≤ t ≤ 1.

t_vec3	lerp3(t_vec3 a, t_vec3 b, float t)
{
	a = scale3(a, 1.0f - t);
	b = scale3(b, 0.0f + t);
	return (add3(a, b));
}

// Apply basic Reinhard tone mapping to a color.

t_vec3	tonemap3(t_vec3 v)
{
	return (div3(v, add3(v, vec3(1.0f, 1.0f, 1.0f))));
}

// Convert a color vector to sRGB, applying an approximate gamma curve.

t_vec3	to_srgb3(t_vec3 color)
{
	const float		gamma = 1.0f / 2.2f;

	color.r = powf(color.r, gamma);
	color.g = powf(color.g, gamma);
	color.b = powf(color.b, gamma);
	return (color);
}

// Convert a color vector to a 32-bit MLX pixel value. Clamps all color channels
// to [0, 1] before converting to 8 bits per channel. sRGB conversion and
// dithering should be done before this step!

uint32_t	to_color3(t_vec3 color)
{
	const uint32_t	r = 255.5f * saturate(color.r);
	const uint32_t	g = 255.5f * saturate(color.g);
	const uint32_t	b = 255.5f * saturate(color.b);

	return ((r << 24) | (g << 16) | (b << 8) | 255);
}
