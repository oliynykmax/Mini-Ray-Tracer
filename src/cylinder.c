#include "minirt.h"

// Get the distance along a ray to a cylinder body.

static float	body_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	const float	a = (rd.x * rd.x + rd.z * rd.z);
	const float	b = (rd.x * ro.x + rd.z * ro.z) * 2.0f;
	const float	c = (ro.x * ro.x + ro.z * ro.z) - o->radius * o->radius;
	const float	t = solve_quadratic(a, b, c);

	if (fabsf(ro.y + t * rd.y) > o->height * 0.5f)
		return (1e9f);
	return (t);
}

// Get the distance along a ray to a cylinder end cap (a disk).

static float	disk_distance(t_object *o, t_vec3 ro, t_vec3 rd, float h)
{
	const float		t = (h - ro.y) / rd.y;
	const float		x = ro.x + t * rd.x;
	const float		z = ro.z + t * rd.z;

	if (t < 0.0f || x * x + z * z > o->radius * o->radius)
		return (1e9f);
	return (t);
}

// Get the distance along a ray to a full cylinder.

float	cylinder_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	const float	body = body_distance(o, ro, rd);
	const float	top = disk_distance(o, ro, rd, -0.5f * o->height);
	const float	bot = disk_distance(o, ro, rd, +0.5f * o->height);

	return (fminf(body, fminf(top, bot)));
}

// Get the normal of a cylinder at a point `p` on its surface.

t_vec3	cylinder_normal(t_object *o, t_vec3 p)
{
	if (fabsf(p.y) < o->height * 0.5f - 1e-6f)
		return (vec3_scale(vec3(p.x, 0.0f, p.z), 1.0f / o->radius));
	return (vec3(0.0f, 1.0f, 0.0f));
}

// Get the texture coordinates of a cylinder at a point `p` on its surface.

t_vec3	cylinder_texcoord(t_object *o, t_vec3 p)
{
	const float	h = o->height * 0.5f - 1e-6f;

	p.y = clamp(p.y, -h, h);
	p.x = atan2f(p.x, p.z) / M_PI * 0.5f + 0.5f;
	return (p);
}
