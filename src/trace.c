#include "minirt.h"

static _Thread_local uint16_t	rng;

// Generate a random point in the unit square [0, 1]².

static t_vec3	random_point_in_square(void)
{
	const float	x = fract(PLASTIC_RATIO_X * rng);
	const float	y = fract(PLASTIC_RATIO_Y * rng);

	return (vec3(x, y, 0.0f));
}

// Generate a random point in a disk centered at the origin with the given
// radius.

static t_vec3	random_point_in_disk(float radius)
{
	const t_vec3	p = random_point_in_square();
	const float		x = sqrtf(p.x) * cosf(p.y * TAU) * radius;
	const float		y = sqrtf(p.x) * sinf(p.y * TAU) * radius;

	return (vec3(x, y, 0.0f));
}

// Generate a random point on a sphere centered at the origin with the given
// radius.

static t_vec3	random_point_on_sphere(float radius)
{
	const t_vec3	p = random_point_in_square();
	const float		x = sinf(p.x * M_PI) * cosf(p.y * TAU);
	const float		y = sinf(p.x * M_PI) * sinf(p.y * TAU);
	const float		z = cosf(p.x * M_PI);

	return (vec_scale(vec3(x, y, z), radius));
}

static float	object_distance(t_object *object, t_vec3 ro, t_vec3 rd)
{
	static const t_distance_function	functions[] = {
		plane_distance,
		sphere_distance,
		cylinder_distance,
		para_distance,
	};

	ro = quat_rotate_vec3(quat_inverse(object->rot), vec_sub(ro, object->pos));
	rd = quat_rotate_vec3(quat_inverse(object->rot), rd);
	return (functions[object->type](object, ro, rd));
}

static t_vec3	object_normal(t_object *obj, t_vec3 point)
{
	static const t_normal_function	functions[] = {
		plane_normal,
		sphere_normal,
		cylinder_normal,
		para_normal,
	};

	point = quat_rotate_vec3(quat_inverse(obj->rot), vec_sub(point, obj->pos));
	return (quat_rotate_vec3(obj->rot, functions[obj->type](obj, point)));
}

static t_vec3	object_texcoord(t_object *obj, t_vec3 point)
{
	static const t_normal_function	functions[] = {
		plane_texcoord,
		sphere_texcoord,
		cylinder_texcoord,
		para_texcoord,
	};

	point = quat_rotate_vec3(quat_inverse(obj->rot), vec_sub(point, obj->pos));
	return (functions[obj->type](obj, point));
}

static t_vec3	single_light(t_object *light, t_vec3 rd, t_vec3 n, t_vec3 p)
{
	const t_vec3	light_vec = vec_sub(light->pos, p);
	const t_vec3	light_dir = vec_normalize(light_vec);
	const t_vec3	half = vec_normalize(vec_sub(light_dir, rd));
	const float		diffuse = saturate(vec_dot(light_dir, n));
	const float		specular = 5.0f * powf(fmaxf(0.0f, vec_dot(n, half)), 30);
	t_vec3			color;

	color = vec_scale(light->color, diffuse);
	color = vec_add(color, vec3(specular, specular, specular));
	color = vec_scale(color, 300.0f / vec_dot(light_vec, light_vec));
	return (color);
}

static float	checkboard(float u, float v)
{
	u = floorf(u * 10.0f);
	v = floorf(v * 10.0f);
	return (0.1f + 0.9f * fract((u + v) * 0.5f));
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

// Apply ambient/diffuse/specular lighting to a traced ray.

t_vec3	lighting(t_object *object, t_vec3 p, t_scene *s, t_vec3 rd)
{
	t_vec3	light;
	t_vec3	n;
	size_t	i;

	i = -1;
	light = s->ambient;
	n = object_normal(object, p);
	n = vec_scale(n, copysignf(1.0f, -vec_dot(rd, n)));
	p = vec_add(p, vec_scale(n, 1e-5f));
	while (++i < s->object_count)
	{
		if (s->objects[i].type == OBJECT_LIGHT)
		{
			t_vec3 rand = random_point_on_sphere(2.0f);
			t_vec3 light_pos = vec_add(s->objects[i].pos, rand);
			t_vec3 light_dir = vec_sub(light_pos, p);
			if (scene_distance(s, p, light_dir, NULL) >= 1.0f)
				light = vec_add(light, single_light(&s->objects[i], rd, n, p));
		}
	}
	return (light);
}

static t_vec3	texturing(t_object *object, t_vec3 point)
{
	const t_vec3	texcoord = object_texcoord(object, point);
	const float		value = checkboard(texcoord.x, texcoord.y);

	return (vec3(value, value, value));
}

static t_vec3	trace_scene(t_scene *s, t_vec3 ro, t_vec3 rd, int limit)
{
	t_object	*object;
	t_vec3		point;
	t_vec3		color;
	const float	t = scene_distance(s, ro, rd, &object);

	if (object == NULL)
		return (s->ambient);
	point = vec_add(ro, vec_scale(rd, t));
	color = object->color;
	color = vec_mul(color, texturing(object, point));
	color = vec_mul(color, lighting(object, point, s, rd));
	if (object->type == OBJECT_CYLINDER)
	{
		t_vec3 n = object_normal(object, point);
		n = vec_scale(n, copysignf(1.0f, -vec_dot(rd, n)));
		t_vec3 p = vec_add(point, vec_scale(n, 1e-6f));
		n = vec_reflect(rd, n);
		n = vec_normalize(vec_add(n, random_point_on_sphere(0.0f)));
		if (limit > 0)
			color = trace_scene(s, p, n, limit - 1);
		else
			color = vec3(0.0f, 0.0f, 0.0f);
	}
	return (color);
}

t_vec3	get_viewport_ray(t_render *r, float x, float y, bool jitter)
{
	const float		u = (x + r->jitter_x * jitter) / r->image->width;
	const float		v = (y + r->jitter_y * jitter) / r->image->height;
	const t_vec3	v0 = vec_lerp(r->viewport[0], r->viewport[1], u);
	const t_vec3	v1 = vec_lerp(r->viewport[2], r->viewport[3], u);

	return (vec_normalize(vec_lerp(v0, v1, v)));
}

// Trace the color of the pixel at (x, y) in the image.

t_vec3	trace_pixel(t_render *r, float x, float y)
{
	t_vec3	disk;
	t_vec3	ro;
	t_vec3	rd;

	rng = r->frame_samples + (int) x + (int) y * r->image->width;
	disk = random_point_in_disk(r->scene->aperture_size);
	ro = vec_add(r->scene->pos, vec_scale(r->camera_x, disk.x));
	ro = vec_add(ro, vec_scale(r->camera_y, disk.y));
	rd = get_viewport_ray(r, x, y, true);
	rd = vec_add(r->scene->pos, vec_scale(rd, r->scene->focus_depth));
	rd = vec_normalize(vec_sub(rd, ro));
	return (trace_scene(r->scene, ro, rd, 1));
}
