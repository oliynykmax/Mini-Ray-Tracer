#include "minirt.h"

static void	trace_sphere(t_ray *r, t_object *s)
{
	const t_vec3	o = vec3_sub(s->pos, r->ro);
	const float		a = vec3_dot(r->rd, r->rd);
	const float		h = vec3_dot(r->rd, o);
	const float		d = h * h - a * (vec3_dot(o, o) - s->radius * s->radius);
	float			depth;

	if (d < 0.0f)
		return ;
	depth = (h - sqrtf(d)) / a;
	if (depth < 0.0f || depth >= r->depth)
		return ;
	r->depth = depth;
	r->color = s->color;
	r->point = vec3_add(r->ro, vec3_scale(r->rd, depth));
	r->normal = vec3_normalize(vec3_sub(r->point, s->pos));
}

// Apply ambient/diffuse/specular lighting to a traced ray.

static void	lighting(t_scene *s, t_ray *r, t_vec3 light)
{
	const t_vec3	reflect = vec3_reflect(light, r->normal);
	const float		diffuse = saturate(vec3_dot(light, r->normal));
	const float		specular = powf(fmaxf(0.0f, vec3_dot(r->rd, reflect)), 20);

	r->color = vec3_lerp(s->ambient, r->color, diffuse);
	r->color = vec3_lerp(r->color, vec3(1.0f, 1.0f, 1.0f), specular);
}

// Trace the scene with ray origin `ro` and ray direction `rd`.

static t_vec3	trace_scene(t_scene *s, t_vec3 ro, t_vec3 rd)
{
	t_ray			ray;
	size_t			i;

	i = -1;
	ray.ro = ro;
	ray.rd = rd;
	ray.depth = INFINITY;
	ray.color = s->ambient;
	while (++i < s->object_count)
	{
		if (s->objects[i].type == OBJECT_SPHERE)
			trace_sphere(&ray, &s->objects[i]);
	}
	if (ray.depth < 1e9f)
		lighting(s, &ray, vec3(1.0f, 0.0f, 0.0f));
	return (ray.color);
}

// Trace the color of the pixel at (x, y) in the image.

t_vec3	trace_pixel(t_render *r, float x, float y)
{
	const float		u = x / (r->image->width - 1);
	const float		v = y / (r->image->height - 1);
	const t_vec3	v0 = vec3_lerp(r->viewport[0], r->viewport[1], u);
	const t_vec3	v1 = vec3_lerp(r->viewport[2], r->viewport[3], u);
	const t_vec3	rd = vec3_lerp(v0, v1, v);

	return (trace_scene(r->scene, r->scene->pos, vec3_normalize(rd)));
}
