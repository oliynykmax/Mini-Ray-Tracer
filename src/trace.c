#include "minirt.h"

float	scene_distance(t_scene *s, t_vec3 ro, t_vec3 rd, t_object **object)
{
	float		t_min;
	float		t;
	size_t		i;

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

static t_vec3	trace_scene(t_ray *r);

t_vec3	reflection(t_ray *r, t_vec3 p, t_vec3 n)
{
	const float	fuzziness = 0.0f;

	if (r->bounce-- == 0)
		return vec3(0,0,0);
	n = reflect3(r->rd, n);
	n = norm3(add3(n, random_point_on_sphere(r->rng, fuzziness)));
	r->ro = p;
	r->rd = n;
	return (trace_scene(r));
}

static t_vec3	trace_scene(t_ray *r)
{
	t_shading	s;
	t_object	*object;
	const float	t = scene_distance(r->scene, r->ro, r->rd, &object);

	if (object == NULL)
		return (r->scene->ambient);
	if (object->type == OBJECT_LIGHT || !r->fancy)
		return (object->color);
	s.point = add3(r->ro, scale3(r->rd, t));
	object_params(object, &s);
	// s.normal = scale3(s.normal, copysignf(1.0f, -dot3(s.normal, r->rd)));
	s.point = add3(s.point, scale3(s.normal, 1e-4f));
	apply_bumpmap(&s, TEXTURE_POLKADOT, s.texcoord);
	t_ray copy = *r;
	s.ambient = reflection(r, s.point, s.normal);
	*r = copy; // FIXME
	return (shade_point(&s, r, object));
}

t_vec3	get_viewport_ray(t_render *r, float x, float y, bool jitter)
{
	const float		u = (x + r->jitter.x * jitter) / r->image->width;
	const float		v = (y + r->jitter.y * jitter) / r->image->height;
	const t_vec3	v0 = lerp3(r->viewport[0], r->viewport[1], u);
	const t_vec3	v1 = lerp3(r->viewport[2], r->viewport[3], u);

	return (norm3(lerp3(v0, v1, v)));
}

// Trace the color of the pixel at (x, y) in the image.

t_vec3	trace_pixel(t_render *r, float x, float y)
{
	t_vec3	disk;
	t_ray	ray;

	ray.fancy = r->fancy;
	ray.scene = r->scene;
	ray.rng = r->frame_samples + (int) x + (int) y * r->image->width;
	disk = random_point_in_disk(ray.rng, r->scene->aperture_size);
	ray.ro = add3(r->scene->pos, scale3(r->camera_x, disk.x));
	ray.ro = add3(ray.ro, scale3(r->camera_y, disk.y));
	ray.rd = get_viewport_ray(r, x, y, true);
	ray.rd = add3(r->scene->pos, scale3(ray.rd, r->scene->focus_depth));
	ray.rd = norm3(sub3(ray.rd, ray.ro));
	ray.bounce = 3;
	return (trace_scene(&ray));
}
