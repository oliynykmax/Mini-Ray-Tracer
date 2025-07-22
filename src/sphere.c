#include "minirt.h"

float	sphere_distance(t_object *s, t_vec3 ro, t_vec3 rd)
{
	const t_vec3	o = vec3_sub(s->pos, ro);
	const float		a = vec3_dot(rd, rd);
	const float		h = vec3_dot(rd, o);
	const float		d = h * h - a * (vec3_dot(o, o) - s->radius * s->radius);
	float			depth;

	if (d < 0.0f)
		return (1e9f);
	depth = sqrtf(d);
	return ((h + copysignf(depth, depth - h)) / a);
}

t_vec3	sphere_normal(t_object *s, t_vec3 p)
{
	return (vec3_scale(vec3_sub(p, s->pos), 1.0f / s->radius));
}

t_vec3	sphere_texcoord(t_object *s, t_vec3 p)
{
	const t_vec3	d = vec3_sub(p, s->pos);
	const float		u = atan2f(d.x, d.z) / M_PI * 0.5f + 0.5f;
	const float		v = asinf(d.y / s->radius) / M_PI + 0.5f;

	return (vec3(u, v, 0.0f));
}
