#include "minirt.h"

// Generate a random point in the unit square [0, 1]². Uses a low-discrepancy
// sequence to generate the coordinates of the point. The constants used to
// randomize the x and y coordinates are equal to p¯¹ and  p¯², where p is the
// "plastic ratio." The plastic ratio is the unique real solution to the
// equation x³ = x + 1, a number with properties that are useful for generating
// numbers that look "random," but don't cluster together like truly random
// numbers do.

t_vec3	random_point_in_square(uint16_t rng)
{
	const float	x = fract(0.7548776662466927 * rng);
	const float	y = fract(0.5698402909980532 * rng);

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
