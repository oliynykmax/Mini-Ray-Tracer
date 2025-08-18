#include "minirt.h"

// Generate a random float between 0 and 1.

float	random_float(uint16_t rng)
{
	return (fract(rng * (1.0f / 1.6180339887498948482f)));
}

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

// Generate a random point inside a disk centered at the origin with radius 1.

t_vec3	random_point_in_disk(uint16_t rng)
{
	const t_vec3	p = random_point_in_square(rng);
	const float		x = sqrtf(p.x) * cosf(p.y * (2.0f * M_PI));
	const float		y = sqrtf(p.x) * sinf(p.y * (2.0f * M_PI));

	return (vec3(x, y, 0.0f));
}

// Generate a random point on a sphere centered at the origin with radius 1.

t_vec3	random_point_on_sphere(uint16_t rng)
{
	const t_vec3	p = random_point_in_square(rng);
	const float		x = sinf(p.x * M_PI) * cosf(p.y * (2.0f * M_PI));
	const float		y = sinf(p.x * M_PI) * sinf(p.y * (2.0f * M_PI));
	const float		z = cosf(p.x * M_PI);

	return (vec3(x, y, z));
}
