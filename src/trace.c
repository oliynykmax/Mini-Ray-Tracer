#include "minirt.h"

float	scene_distance(t_scene *s, t_vec3 ro, t_vec3 rd, t_object **object)
{
	float	t_min;
	float	t;
	size_t	i;

	i = -1;
	if (object != NULL)
		*object = NULL;
	t_min = 1e9f;
	while (++i < s->object_count)
	{
		t = object_distance(&s->objects[i], ro, rd);
		if (t < 0.0f || t >= t_min)
			continue ;
		t_min = t;
		if (object != NULL)
			*object = &s->objects[i];
	}
	return (t_min);
}

t_vec3	scatter(t_ray r, t_vec3 p, t_vec3 n, t_object *o)
{
	const t_vec3	random = random_point_on_sphere(r.rng);

	if (random_float(r.rng) > o->metallic)
		r.rd = random;
	else
		r.rd = norm3(add3(reflect3(r.rd, n), scale3(random, o->rough)));
	r.ro = add3(p, scale3(n, 1e-4f));
	r.rd = scale3(r.rd, copysignf(1.0f, dot3(r.rd, n)));
	return (trace_scene(&r));
}

static t_vec3	ambient_color(t_scene *s, t_vec3 rd)
{
	t_vec3	color;
	float	u;
	float	v;

	color = lerp3(s->ambient, s->ambient2, fabsf(rd.y));
	if (s->amb_texture == NULL)
		return (color);
	u = fmaxf(0.0f, atan2f(rd.x, rd.z) / M_PI * 0.5f + 0.5f);
	v = fmaxf(0.0f, 1.0f - asinf(rd.y) / M_PI + 0.5f);
	return (mul3(color, sample_png_color(s->amb_texture, u, v)));
}

t_vec3	trace_scene(t_ray *r)
{
	t_shading	s;
	t_object	*object;
	const float	t = scene_distance(r->scene, r->ro, r->rd, &object);

	if (object == NULL)
		return (ambient_color(r->scene, r->rd));
	if (object->type == OBJECT_LIGHT)
		return (object->color);
	s.point = add3(r->ro, scale3(r->rd, t));
	object_params(object, &s);
	if (object->bump_img)
		apply_bumpmap(&s, object);
	s.ambient = vec3(0.0f, 0.0f, 0.0f);
	if (r->bounce-- > 0)
		s.ambient = scatter(*r, s.point, s.normal, object);
	return (shade_point(&s, r, object));
}

// Trace the color of the pixel at (x, y) in the image.

t_vec3	trace_pixel(t_render *r, float x, float y, int frame)
{
	t_vec3	disk;
	t_ray	ray;

	ray.scene = r->scene;
	ray.rng = frame + (int)x + (int)y * r->image->width;
	disk = scale3(random_point_in_disk(ray.rng), r->scene->aperture_size);
	ray.ro = r->scene->pos;
	ray.ro = add3(ray.ro, scale3(r->camera_x, disk.x));
	ray.ro = add3(ray.ro, scale3(r->camera_y, disk.y));
	ray.rd = get_viewport_ray(r, vec3(x, y, 0.0f), ray.rng, frame);
	ray.rd = add3(r->scene->pos, scale3(ray.rd, r->scene->focus_depth));
	ray.rd = norm3(sub3(ray.rd, ray.ro));
	ray.bounce = MAX_RAY_BOUNCES;
	return (trace_scene(&ray));
}
