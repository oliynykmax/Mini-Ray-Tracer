#include "minirt.h"

// Create a quaternion from an axis and and an angle (in radians).

t_quat	quat_from_axis_angle(t_vec3 axis, float angle)
{
	const float	s = sinf(angle * 0.5f);
	const float	c = cosf(angle * 0.5f);

	return ((t_quat){{axis.x * s, axis.y * s, axis.z * s, c}});
}

// Create an orientation quaternion from a (normalized) direction vector.

t_quat	quat_from_direction(t_vec3 d)
{
	t_vec3	r;
	t_quat	q;

	if (fabsf(d.y + 1.0f) < 1e-6f)
		return ((t_quat){{1.0f, 0.0f, 0.0f, 0.0f}});
	if (fabsf(d.y - 1.0f) < 1e-6f)
		return ((t_quat){{0.0f, 0.0f, 0.0f, 1.0f}});
	r = vec3_normalize(vec3(d.x, d.y + 1.0f, d.z));
	q.xyz = vec3_cross(r, d);
	q.w = vec3_dot(r, d);
	return (q);
}

// Multiply two quaternions. This composes the rotations represented by the two
// quaternions.

t_quat	quat_multiply(t_quat a, t_quat b)
{
	const t_vec3	ab = vec3_scale(a.xyz, b.w);
	const t_vec3	ba = vec3_scale(b.xyz, a.w);
	const t_vec3	imag = vec3_add(vec3_cross(a.xyz, b.xyz), vec3_add(ab, ba));
	const float		real = a.w * b.w - vec3_dot(a.xyz, b.xyz);

	return ((t_quat){{imag.x, imag.y, imag.z, real}});
}

// Invert a quaternion. Conceptually, this reverses the rotation that the
// quaternion represents.

t_quat	quat_inverse(t_quat q)
{
	return ((t_quat){{-q.x, -q.y, -q.z, q.w}});
}

// Apply the rotation represented by the quaternion `q` to the 3D vector `v`.

t_vec3	quat_rotate_vec3(t_quat q, t_vec3 v)
{
	const t_vec3	a = vec3_add(vec3_cross(q.xyz, v), vec3_scale(v, q.w));

	return (vec3_add(v, vec3_scale(vec3_cross(q.xyz, a), 2.0f)));
}
