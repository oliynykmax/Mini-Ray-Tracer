#include "minirt.h"

// Get the dot product (scalar product) of two 3D vectors. The resulting value
// is proportional to the product of the lengths of the two vectors, and to the
// cosine of the angle between them.

float	dot3(t_vec3 a, t_vec3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

// Get the length (magnitude) of a 3D vector.

float	len3(t_vec3 v)
{
	return (sqrtf(dot3(v, v)));
}

// Get a normalized copy of a 3D vector. The resulting vector has the same
// direction as the input, but a length (magnitude) of 1. Results are undefined
// for the null vector (0, 0, 0).

t_vec3	norm3(t_vec3 v)
{
	return (scale3(v, 1.0f / len3(v)));
}

// Get the cross product of two 3D vectors. The resulting vector is
// perpendicular to the two vectors, and its length is equal to the product of
// the lengths of the input vectors.

t_vec3	cross3(t_vec3 a, t_vec3 b)
{
	const float	x = a.y * b.z - b.y * a.z;
	const float	y = a.z * b.x - b.z * a.x;
	const float	z = a.x * b.y - b.x * a.y;

	return (vec3(x, y, z));
}

// Get the reflected ray direction for an incident ray `i` and a surface normal
// `n`.

t_vec3	reflect3(t_vec3 i, t_vec3 n)
{
	return (sub3(i, scale3(n, 2.0f * dot3(n, i))));
}
