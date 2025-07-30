#include "minirt.h"

// Body intersection: ray vs para shell

static float	body_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	float	k;
	float	a;
	float	b;
	float	t;
	float	y;

	k = (o->radius * o->radius) / o->height;
	ro.y += 0.5f * o->height;
	a = rd.x * rd.x + rd.z * rd.z;
	b = 2.0f * (ro.x * rd.x + ro.z * rd.z) - k * rd.y;
	t = solve_quadratic(a, b, ro.x * ro.x + ro.z * ro.z - k * ro.y);
	y = ro.y + t * rd.y;
	if (t < 0.0f || y < 0.0f || y > o->height)
		return (1e9f);
	return (t);
}

// Optional cap at top (y = +h/2), disk
static float	disk_distance(t_object *o, t_vec3 ro, t_vec3 rd, float h)
{
	const float	t = (h - ro.y) / rd.y;
	const float	x = ro.x + t * rd.x;
	const float	z = ro.z + t * rd.z;

	if (t < 0.0f || x * x + z * z > o->radius * o->radius)
		return (1e9f);
	return (t);
}

// Full intersection function
float	para_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	ro.y -= 0.5f * o->height;
	return (fminf(body_distance(o, ro, rd), disk_distance(o, ro, rd, +0.5f
				* o->height)));
}

t_vec3	para_normal(t_object *o, t_vec3 p)
{
	const float	k = (o->radius * o->radius) / o->height;

	if (fabsf(p.y - 0.5f * o->height) < 1e-4f)
		return (vec3(0, 1, 0));
	p.y += 0.5f * o->height;
	return (vec3_normalize(vec3(p.x, -k, p.z)));
}

// UV: cylindrical mapping
t_vec3	para_texcoord(t_object *o, t_vec3 p)
{
	p.y = clamp(p.y, 0, o->height);
	p.x = atan2f(p.x, p.z) / M_PI * 0.5f + 0.5f;
	return (p);
}
