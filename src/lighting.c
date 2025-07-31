#include "minirt.h"

static t_vec3	specular(t_vec3 light_dir, t_vec3 ray_dir, t_vec3 normal)
{
	const t_vec3	h = vec3_normalize(vec3_sub(light_dir, ray_dir));
	const float		s = 5.0f * powf(fmaxf(0.0f, vec3_dot(normal, h)), 30);

	return (vec3(s, s, s));
}

static t_vec3	one_light(t_ray *r, t_object *light, t_vec3 p, t_vec3 n)
{
	const t_vec3	rand = random_point_on_sphere(r->rng, light->radius);
	const t_vec3	light_pos = vec3_add(light->pos, rand);
	const t_vec3	light_vec = vec3_sub(light_pos, p);
	const t_vec3	light_dir = vec3_normalize(light_vec);
	t_vec3			color;

	if (scene_distance(r->scene, p, light_dir, NULL) < 1.0f)
		return (vec3(0.0f, 0.0f, 0.0f));
	color = vec3_scale(light->color, saturate(vec3_dot(light_dir, n)));
	color = vec3_add(color, specular(light_dir, r->rd, n));
	return (vec3_scale(color, 300.0f / vec3_dot(light_vec, light_vec)));
}

t_vec3	apply_lighting(t_ray *r, t_object *object, t_vec3 p)
{
	t_vec3	light;
	t_vec3	n;
	size_t	i;

	i = -1;
	light = r->scene->ambient;
	n = object_normal(object, p);
	n = vec3_scale(n, copysignf(1.0f, -vec3_dot(r->rd, n)));
	p = vec3_add(p, vec3_scale(n, 1e-5f));
	while (++i < r->scene->object_count)
		if (r->scene->objects[i].type == OBJECT_LIGHT)
			light = vec3_add(light, one_light(r, &r->scene->objects[i], p, n));
	return (light);
}
