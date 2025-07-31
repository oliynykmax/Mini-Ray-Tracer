#include "minirt.h"

float	sphere_distance(t_object *s, t_vec3 ro, t_vec3 rd)
{
	const float		a = vec_dot(rd, rd);
	const float		b = vec_dot(rd, ro) * 2.0f;
	const float		c = vec_dot(ro, ro) - s->radius * s->radius;

	return (solve_quadratic(a, b, c));
}

t_vec3	sphere_normal(t_object *s, t_vec3 p)
{
	return (vec_scale(p, 1.0f / s->radius));
}

t_vec3	sphere_texcoord(t_object *s, t_vec3 p)
{
	p.x = fmaxf(0.0f, atan2f(p.x, p.z) / M_PI * 0.5f + 0.5f);
	p.y = fmaxf(0.0f, asinf(p.y / s->radius) / M_PI + 0.5f);
	return (p);
}
