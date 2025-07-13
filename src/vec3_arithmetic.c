#include "minirt.h"

// Add two 3D vectors component-wise.

t_vec3	vec3_add(t_vec3 a, t_vec3 b)
{
	return (vec3(a.x + b.x, a.y + b.y, a.z + b.z));
}

// Subtract two 3D vectors component-wise.

t_vec3	vec3_sub(t_vec3 a, t_vec3 b)
{
	return (vec3(a.x - b.x, a.y - b.y, a.z - b.z));
}

// Multiply two 3D vectors component-wise.

t_vec3	vec3_mul(t_vec3 a, t_vec3 b)
{
	return (vec3(a.x * b.x, a.y * b.y, a.z * b.z));
}

// Divide two 3D vectors component-wise.

t_vec3	vec3_div(t_vec3 a, t_vec3 b)
{
	return (vec3(a.x / b.x, a.y / b.y, a.z / b.z));
}

// Scale a 3D vector by some value. (Multiply all of its components by some
// value.)

t_vec3	vec3_scale(t_vec3 v, float s)
{
	return (vec3(v.x * s, v.y * s, v.z * s));
}
