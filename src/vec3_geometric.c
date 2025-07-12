#include "minirt.h"

// Scale a 3D vector by some value. (Multiply all of its components by some
// scalar.)

t_vec3	vec3_scale(t_vec3 v, float s)
{
	return (vec3(v.x * s, v.y * s, v.z * s));
}

// Get the dot product (scalar product) of two 3D vectors. The resulting value
// is equal to the product of the lengths of the two vectors, and to the cosine
// of the angle between them.

float	vec3_dot(t_vec3 a, t_vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

// Get the length (magnitude) of a 3D vector.

float	vec3_length(t_vec3 v)
{
	return (sqrtf(vec3_dot(v, v)));
}

// Get a normalized copy of a 3D vector. The resulting vector has the same
// direction as the input, but a length (magnitude) of 1. Results are undefined
// for the null vector (0, 0, 0).

t_vec3	vec3_normalize(t_vec3 v)
{
	return (vec3_scale(v, 1.0f / vec3_length(v)));
}

// Get the cross product of two 3D vectors. The resulting vector's length is
// perpendicular to the two vectors, and its length is equal to the product of
// the lengths of the two vectors.

t_vec3	vec3_cross(t_vec3 a, t_vec3 b)
{
	const float	x = a.y * b.z - b.y * a.z;
	const float	y = a.z * b.x - b.z * a.x;
	const float	z = a.x * b.y - b.x * a.y;

	return (vec3(x, y, z));
}
