#include "minirt.h"

static float	object_distance(t_object *object, t_vec3 ro, t_vec3 rd)
{
	static const t_distance_function	functions[] = {
		plane_distance,
		sphere_distance,
		cylinder_distance,
	};

	return (functions[object->type](object, ro, rd));
}

static t_vec3	object_normal(t_object *object, t_vec3 point)
{
	static const t_normal_function	functions[] = {
		plane_normal,
		sphere_normal,
		cylinder_normal,
	};

	return (functions[object->type](object, point));
}

static t_vec3	object_texcoord(t_object *object, t_vec3 point)
{
	static const t_normal_function	functions[] = {
		plane_texcoord,
		sphere_texcoord,
		cylinder_texcoord,
	};

	return (functions[object->type](object, point));
}

static t_vec3	single_light(t_object *light, t_vec3 rd, t_vec3 n, t_vec3 p)
{
	const t_vec3	light_dir = vec3_normalize(vec3_sub(light->pos, p));
	const t_vec3	half = vec3_normalize(vec3_sub(light_dir, rd));
	const float		diffuse = saturate(vec3_dot(light_dir, n));
	const float		specular = powf(fmaxf(0.0f, vec3_dot(n, half)), 30);
	t_vec3			color;

	color = vec3(specular, specular, specular);
	color = vec3_add(color, vec3_scale(color, diffuse));
	return (color);
}

static float	checkboard(float u, float v)
{
	u = floorf(u * 10.0f);
	v = floorf(v * 10.0f);
	return (0.1f + 0.9f * fract((u + v) * 0.5f));
}

// Apply ambient/diffuse/specular lighting to a traced ray.

t_vec3	lighting(t_object *object, t_vec3 p, t_scene *s, t_vec3 rd)
{
	t_vec3	light;
	t_vec3	n;
	size_t	i;

	i = -1;
	light = s->ambient;
	n = object_normal(object, p);
	n = vec3_scale(n, -sign(vec3_dot(rd, n)));
	while (++i < s->object_count)
		if (s->objects[i].type == OBJECT_LIGHT)
			light = vec3_add(light, single_light(&s->objects[i], rd, n, p));
	return (light);
}

static t_vec3	texturing(t_object *object, t_vec3 point)
{
	const t_vec3	texcoord = object_texcoord(object, point);
	const float		value = checkboard(texcoord.x, texcoord.y);

	return (vec3(value, value, value));
}

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
		if (s->objects[i].type == OBJECT_LIGHT)
			continue ;
		t = object_distance(&s->objects[i], ro, rd);
		if (t < 0.0f || t >= t_min)
			continue ;
		t_min = t;
		if (object != NULL)
			*object = &s->objects[i];
	}
	return (t_min);
}

static t_vec3	trace_scene(t_scene *s, t_vec3 ro, t_vec3 rd)
{
	t_object	*object;
	t_vec3		point;
	t_vec3		color;
	const float	t = scene_distance(s, ro, rd, &object);

	if (object == NULL)
		return (s->ambient);
	point = vec3_add(ro, vec3_scale(rd, t));
	color = object->color;
	color = vec3_mul(color, texturing(object, point));
	color = vec3_mul(color, lighting(object, point, s, rd));
	return (color);
}

// Trace the color of the pixel at (x, y) in the image.

t_vec3	trace_pixel(t_render *r, float x, float y)
{
	const float		u = (r->jitter_x + x) / r->image->width;
	const float		v = (r->jitter_y + y) / r->image->height;
	const t_vec3	v0 = vec3_lerp(r->viewport[0], r->viewport[1], u);
	const t_vec3	v1 = vec3_lerp(r->viewport[2], r->viewport[3], u);
	const t_vec3	rd = vec3_lerp(v0, v1, v);

	return (trace_scene(r->scene, r->scene->pos, vec3_normalize(rd)));
}
