#include "minirt.h"

static t_vec3	single_light(t_scene *scene, t_ray *r, t_object *light)
{
	const t_vec3	light_dir = vec3_normalize(vec3_sub(light->pos, r->point));
	const t_vec3	reflect = vec3_reflect(light_dir, r->normal);
	const float		diffuse = saturate(vec3_dot(light_dir, r->normal));
	const float		specular = powf(fmaxf(0.0f, vec3_dot(r->rd, reflect)), 30);
	t_vec3			color;

	color = vec3_mul(scene->ambient, r->color);
	color = vec3_add(color, vec3_scale(vec3(1.0f, 1.0f, 1.0f), specular));
	color = vec3_add(color, vec3_scale(r->color, diffuse * (1.0f - specular)));
	return (color);
}

// Apply ambient/diffuse/specular lighting to a traced ray.

static void	lighting(t_scene *s, t_ray *r)
{
	size_t	i;
	t_vec3	color;

	color = vec3(0.0f, 0.0f, 0.0f);
	i = -1;
	while (++i < s->object_count)
		if (s->objects[i].type == OBJECT_LIGHT)
			color = vec3_add(color, single_light(s, r, &s->objects[i]));
	r->color = color;
}

// Trace the scene with ray origin `ro` and ray direction `rd`.

static t_vec3	trace_scene(t_scene *s, t_vec3 ro, t_vec3 rd)
{
	t_ray			ray;
	size_t			i;

	i = -1;
	ray.ro = ro;
	ray.rd = rd;
	ray.depth = 1e9f;
	ray.color = s->ambient;
	while (++i < s->object_count)
	{
		if (s->objects[i].type == OBJECT_SPHERE)
			trace_sphere(&ray, &s->objects[i]);
		if (s->objects[i].type == OBJECT_PLANE)
			trace_plane(&ray, &s->objects[i]);
		if (s->objects[i].type == OBJECT_CYLINDER)
			trace_cylinder(&ray, &s->objects[i]);
	}
	if (ray.depth < 1e9f)
		lighting(s, &ray);
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
