#include "minirt.h"

float	sphere_distance(t_object *s, t_vec3 ro, t_vec3 rd)
{
	const float		a = dot3(rd, rd);
	const float		b = dot3(rd, ro) * 2.0f;
	const float		c = dot3(ro, ro) - s->radius * s->radius;

	return (solve_quadratic(a, b, c));
}

void	sphere_params(t_object *o, t_shading *s)
{
	const t_vec3	p = s->point;

	s->normal = scale3(p, 1.0f / o->radius);
	s->tangent = norm3(cross3(s->normal, vec3(0.0f, 1.0f, 0.0f)));
	s->texcoord.x = fmaxf(0.0f, atan2f(p.x, p.z) / M_PI * 0.5f + 0.5f);
	s->texcoord.y = fmaxf(0.0f, 1.0f - asinf(p.y / o->radius) / M_PI + 0.5f);
}
