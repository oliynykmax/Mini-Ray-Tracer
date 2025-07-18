#include "minirt.h"

// Test if a given depth value is in front of the current object traced by a
// ray. If it is, update the depth, color, and intersection point of the ray,
// and return true. Otherwise, leave the ray data untouched and return false.

bool	ray_depth_test(t_ray *ray, t_object *object, float depth)
{
	if (depth < 0.0f || depth >= ray->depth)
		return (false);
	ray->depth = depth;
	ray->color = object->color;
	ray->point = vec3_add(ray->ro, vec3_scale(ray->rd, depth));
	return (true);
}

void	trace_plane(t_ray *r, t_object *p)
{
	const float	denom = vec3_dot(p->normal, r->rd);
	float		depth;

	if (fabsf(denom) < 1e-6f)
		return ;
	depth = vec3_dot(vec3_sub(p->pos, r->ro), p->normal) / denom;
	if (!ray_depth_test(r, p, depth))
		return ;
	r->normal = p->normal;
	if (denom > 0.0f)
		r->normal = vec3_scale(r->normal, -1.0f);
}

void	trace_sphere(t_ray *r, t_object *s)
{
	const t_vec3	o = vec3_sub(s->pos, r->ro);
	const float		a = vec3_dot(r->rd, r->rd);
	const float		h = vec3_dot(r->rd, o);
	const float		d = h * h - a * (vec3_dot(o, o) - s->radius * s->radius);
	float			depth;

	if (d < 0.0f)
		return ;
	depth = (h - sqrtf(d)) / a;
	if (depth < 0.001f)
		depth = (h + sqrtf(d)) / a;
	if (!ray_depth_test(r, s, depth))
		return ;
	r->normal = vec3_normalize(vec3_sub(r->point, s->pos));
}

void	trace_cylinder(t_ray *r, t_object *c)
{
	const t_vec3	top_cap_center = vec3_add(c->pos,
			vec3_scale(c->normal, c->height / 2.0f));
	const t_vec3	bottom_cap_center = vec3_add(c->pos,
			vec3_scale(c->normal, -c->height / 2.0f));

	intersect_disc(r, c, top_cap_center, c->normal);
	intersect_disc(r, c, bottom_cap_center, vec3_scale(c->normal, -1.0f));
	intersect_cylinder_body(r, c);
}
