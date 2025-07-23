#include "minirt.h"

// Get the distance along a ray to a cylinder body.

static float	body_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	const t_vec3	oc = vec3_sub(ro, o->pos);
	const float		rn = vec3_dot(rd, o->normal);
	const float		on = vec3_dot(oc, o->normal);
	const float		c[] = {
		(vec3_dot(rd, rd) - rn * rn),
		2.0f * (vec3_dot(rd, oc) - rn * on),
		vec3_dot(oc, oc) - on * on - o->radius * o->radius,
	};
	const float		t = solve_quadratic(c[0], c[1], c[2]);

	if (fabsf(on + t * rn) > o->height * 0.5f)
		return (1e9f);
	return (t);
}

// Get the distance along a ray to a cylinder end cap (a disk).

static float	disk_distance(t_object *o, t_vec3 ro, t_vec3 rd, float h)
{
	const t_vec3	c = vec3_add(o->pos, vec3_scale(o->normal, h));
	const float		denom = vec3_dot(o->normal, rd);
	const float		t = vec3_dot(vec3_sub(c, ro), o->normal) / denom;
	const t_vec3	p = vec3_sub(c, vec3_add(ro, vec3_scale(rd, t)));

	if (t < 0.0f || vec3_dot(p, p) > o->radius * o->radius)
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
	const float		h = vec3_dot(vec3_sub(p, o->pos), o->normal);
	const t_vec3	c = vec3_add(o->pos, vec3_scale(o->normal, h));

	if (fabsf(h) >= o->height * 0.5f - 1e-6f)
		return (o->normal);
	return (vec3_scale(vec3_sub(p, c), 1.0f / o->radius));
}

// Get the texture coordinates of a cylinder at a point `p` on its surface.

t_vec3	cylinder_texcoord(t_object *o, t_vec3 p)
{
	const t_vec3	d = vec3_sub(p, o->pos);
	const float		u = vec3_dot(d, o->normal);
	const float		v = atan2f(d.x, d.z) / M_PI * 0.5f + 0.5f;

	return (vec3(u, v, 0.0f));
}
