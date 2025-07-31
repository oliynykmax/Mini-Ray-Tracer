#include "minirt.h"

// Get the dot product (scalar product) of two 3D vectors. The resulting value
// is proportional to the product of the lengths of the two vectors, and to the
// cosine of the angle between them.

float	vec_dot(t_vec3 a, t_vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

// Get the length (magnitude) of a 3D vector.

float	vec_length(t_vec3 v)
{
	return (sqrtf(vec_dot(v, v)));
}

// Get a normalized copy of a 3D vector. The resulting vector has the same
// direction as the input, but a length (magnitude) of 1. Results are undefined
// for the null vector (0, 0, 0).

t_vec3	vec_normalize(t_vec3 v)
{
	return (vec_scale(v, 1.0f / vec_length(v)));
}

// Get the cross product of two 3D vectors. The resulting vector is
// perpendicular to the two vectors, and its length is equal to the product of
// the lengths of the input vectors.

t_vec3	vec_cross(t_vec3 a, t_vec3 b)
{
	const float	x = a.y * b.z - b.y * a.z;
	const float	y = a.z * b.x - b.z * a.x;
	const float	z = a.x * b.y - b.x * a.y;

	return (vec3(x, y, z));
}

// Get the reflected ray direction for an incident ray `i` and a surface normal
// `n`.

t_vec3	vec_reflect(t_vec3 i, t_vec3 n)
{
	return (vec_sub(i, vec_scale(n, 2.0f * vec_dot(n, i))));
}
