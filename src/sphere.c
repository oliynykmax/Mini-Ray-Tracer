#include "minirt.h"

float	sphere_distance(t_object *s, t_vec3 ro, t_vec3 rd)
{
	const t_vec3	oc = vec3_sub(s->pos, ro);
	const float		a = vec3_dot(rd, rd);
	const float		b = vec3_dot(rd, oc) * -2.0f;
	const float		c = vec3_dot(oc, oc) - s->radius * s->radius;

	return (solve_quadratic(a, b, c));
}

t_vec3	sphere_normal(t_object *s, t_vec3 p)
{
	return (vec3_scale(vec3_sub(p, s->pos), 1.0f / s->radius));
}

t_vec3	sphere_texcoord(t_object *s, t_vec3 p)
{
	const t_vec3	d = vec3_sub(p, s->pos);
	const float		u = fmaxf(0.0f, atan2f(d.x, d.z) / M_PI * 0.5f + 0.5f);
	const float		v = fmaxf(0.0f, asinf(d.y / s->radius) / M_PI + 0.5f);

	return (vec3(u, v, 0.0f));
}
