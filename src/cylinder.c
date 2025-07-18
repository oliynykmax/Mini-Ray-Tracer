#include "minirt.h"

/**
 * @brief Intersects a ray with the wall of a cylinder.
 */
void	intersect_cylinder_body(t_ray *r, t_object *c)
{
	const t_vec3	oc = vec3_sub(r->ro, c->pos);
	const float		coef[3] = {vec3_dot(r->rd, r->rd) - powf(vec3_dot(r->rd,
				c->normal), 2), 2.0f * (vec3_dot(r->rd, oc)
			- vec3_dot(r->rd, c->normal) * vec3_dot(oc, c->normal)),
		vec3_dot(oc, oc) - powf(vec3_dot(oc, c->normal), 2) - c->radius
		* c->radius};
	const float		discriminant = coef[1] * coef[1] - 4.0f * coef[0] * coef[2];
	float			t;
	float			y;

	if (discriminant < 0.0f)
		return ;
	t = (-coef[1] - sqrtf(discriminant)) / (2.0f * coef[0]);
	if (t < 0.001f)
		t = (-coef[1] + sqrtf(discriminant)) / (2.0f * coef[0]);
	y = vec3_dot(oc, c->normal) + t * vec3_dot(r->rd, c->normal);
	if (y < -c->height / 2.0f || y > c->height / 2.0f)
		return ;
	if (ray_depth_test(r, c, t))
	{
		r->normal = vec3_normalize(vec3_sub(r->point, vec3_add(c->pos,
						vec3_scale(c->normal, y))));
		if (vec3_dot(r->rd, r->normal) > 0)
			r->normal = vec3_scale(r->normal, -1.0f);
	}
}

void	intersect_disc(t_ray *r, t_object *c, t_vec3 center, t_vec3 normal)
{
	const float	denom = vec3_dot(r->rd, normal);
	float		t;
	t_vec3		p;

	if (fabsf(denom) < 1e-6f)
		return ;
	t = vec3_dot(vec3_sub(center, r->ro), normal) / denom;
	p = vec3_add(r->ro, vec3_scale(r->rd, t));
	if (vec3_length(vec3_sub(p, center)) > c->radius)
		return ;
	if (ray_depth_test(r, c, t))
	{
		r->normal = normal;
		if (vec3_dot(r->rd, r->normal) > 0)
			r->normal = vec3_scale(r->normal, -1.0f);
	}
}
