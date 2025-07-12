#include "minirt.h"

// Convenience function for making a 3D vector from xyz-coordinates.

t_vec3	vec3(float x, float y, float z)
{
	return ((t_vec3){{x, y, z}});
}

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
