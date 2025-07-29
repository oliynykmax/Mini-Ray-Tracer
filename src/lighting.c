#include "minirt.h"

static t_vec3	single_light(t_object *light, t_vec3 rd, t_vec3 n, t_vec3 p)
{
	const t_vec3	light_vec = vec3_sub(light->pos, p);
	const t_vec3	light_dir = vec3_normalize(light_vec);
	const t_vec3	half = vec3_normalize(vec3_sub(light_dir, rd));
	const float		specular = 5.0f * powf(fmaxf(0.0f, vec3_dot(n, half)), 30);
	t_vec3			color;

	color = vec3_scale(light->color, saturate(vec3_dot(light_dir, n)));
	color = vec3_add(color, vec3(specular, specular, specular));
	color = vec3_scale(color, 300.0f / vec3_dot(light_vec, light_vec));
	return (color);
}

static t_vec3	shadow(t_ray *r, t_object *light, t_vec3 p, t_vec3 n)
{
	const t_vec3	rand = random_point_on_sphere(r->rng, 2.0f);
	const t_vec3	light_pos = vec3_add(light->pos, rand);
	const t_vec3	light_dir = vec3_sub(light_pos, p);

	if (scene_distance(r->scene, p, light_dir, NULL) >= 1.0f)
		return (single_light(light, r->rd, n, p));
	return (vec3(0.0f, 0.0f, 0.0f));
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
			light = vec3_add(light, shadow(r, &r->scene->objects[i], p, n));
	return (light);
}
