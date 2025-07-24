#include "minirt.h"

// Create a quaternion from imaginary and real coordinates.

t_quat	quat(float x, float y, float z, float w)
{
	return ((t_quat){{x, y, z, w}});
}

// Create a quaternion from an axis and and an angle (in radians).

t_quat	quat_from_axis_angle(t_vec3 axis, float angle)
{
	const float	s = sinf(angle * 0.5f);
	const float	c = cosf(angle * 0.5f);

	return (quat(axis.x * s, axis.y * s, axis.z * s, c));
}

// Multiply two quaternions. This composes the rotations represented by the two
// quaternions.

t_quat	quat_multiply(t_quat a, t_quat b)
{
	const t_vec3	ab = vec3_scale(a.xyz, b.w);
	const t_vec3	ba = vec3_scale(b.xyz, a.w);
	const t_vec3	imag = vec3_add(vec3_cross(a.xyz, b.xyz), vec3_add(ab, ba));
	const float		real = a.w * b.w - vec3_dot(a.xyz, b.xyz);

	return (quat(imag.x, imag.y, imag.z, real));
}

// Apply the rotation represented by the quaternion `q` to the 3D vector `v`.

t_vec3	quat_rotate_vec3(t_quat q, t_vec3 v)
{
	const t_vec3	a = vec3_add(vec3_cross(q.xyz, v), vec3_scale(v, q.w));

	return (vec3_add(v, vec3_scale(vec3_cross(v, a), 2.0f)));
}
