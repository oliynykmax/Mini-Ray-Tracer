#include "minirt.h"

float	plane_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	const float	denom = vec3_dot(o->normal, rd);
	const float	t = vec3_dot(vec3_sub(o->pos, ro), o->normal) / denom;

	if (t < 0.0f)
		return (1e9f);
	return (t);
}

t_vec3	plane_normal(t_object *o, t_vec3 p)
{
	(void) p;
	return (o->normal);
}

t_vec3	plane_texcoord(t_object *o, t_vec3 p)
{
	t_vec3	n;
	t_vec3	a;
	t_vec3	du;
	t_vec3	dv;

	n = o->normal;
	a = vec3(0.0f, 0.0f, 0.0f);
	if (fabsf(n.x) <= fminf(fabsf(n.y), fabsf(n.z)))
		a.x = 1.0f;
	else if (fabsf(n.y) <= fminf(fabsf(n.x), fabsf(n.z)))
		a.y = 1.0f;
	else if (fabsf(n.z) <= fminf(fabsf(n.x), fabsf(n.y)))
		a.z = 1.0f;
	du = vec3_normalize(vec3_cross(n, a));
	dv = vec3_cross(n, du);
	return (vec3(vec3_dot(p, du), vec3_dot(p, dv), 0.0f));
}
