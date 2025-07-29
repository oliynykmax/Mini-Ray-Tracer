#include "minirt.h"

// Generate a random point in the unit square [0, 1]².

t_vec3	random_point_in_square(uint16_t rng)
{
	const float	x = fract(PLASTIC_RATIO_X * rng);
	const float	y = fract(PLASTIC_RATIO_Y * rng);

	return (vec3(x, y, 0.0f));
}

// Generate a random point in a disk centered at the origin with the given
// radius.

t_vec3	random_point_in_disk(uint16_t rng, float radius)
{
	const t_vec3	p = random_point_in_square(rng);
	const float		x = sqrtf(p.x) * cosf(p.y * TAU) * radius;
	const float		y = sqrtf(p.x) * sinf(p.y * TAU) * radius;

	return (vec3(x, y, 0.0f));
}

// Generate a random point on a sphere centered at the origin with the given
// radius.

t_vec3	random_point_on_sphere(uint16_t rng, float radius)
{
	const t_vec3	p = random_point_in_square(rng);
	const float		x = sinf(p.x * M_PI) * cosf(p.y * TAU);
	const float		y = sinf(p.x * M_PI) * sinf(p.y * TAU);
	const float		z = cosf(p.x * M_PI);

	return (vec3_scale(vec3(x, y, z), radius));
}
