#include "minirt.h"

// Get the normal of a sphere, given a point on its surface.

static t_vec3	sphere_normal(t_object *s, t_vec3 point)
{
	return (vec3_normalize(vec3_sub(point, s->pos)));
}

// Trace a ray with origin `ro` and direction `rd` toward a sphere. Returns the
// parametric distance along the ray where the first intersection occurs, or
// infinity if there's no intersection.

static float	trace_sphere(t_object *s, t_vec3 ro, t_vec3 rd)
{
	const t_vec3	o = vec3_sub(s->pos, ro);
	const float		a = vec3_dot(rd, rd);
	const float		h = vec3_dot(rd, o);
	const float		d = h * h - a * (vec3_dot(o, o) - s->radius * s->radius);

	if (d < 0.0f)
		return (INFINITY);
	return ((h - sqrtf(d)) / a);
}

// Clamp a value to the range [0, 1].

static float	saturate(float value)
{
	return (fmaxf(0.0f, fminf(1.0f, value)));
}

// Trace the scene with ray origin `ro` and ray direction `rd`.

static uint32_t	trace_scene(t_scene *s, t_vec3 ro, t_vec3 rd)
{
	const t_vec3	light = vec3_normalize(vec3(1.0f, 0.0f, 0.0f));
	size_t			i;
	float			t;
	float			t_min;
	float			diffuse;
	t_vec3			color;
	t_vec3			normal;
	t_vec3			point;

	i = -1;
	t_min = INFINITY;
	color = s->ambient;
	normal = light;
	while (++i < s->object_count)
	{
		t = INFINITY;
		if (s->objects[i].type == OBJECT_SPHERE)
			t = trace_sphere(&s->objects[i], ro, rd);
		if (t < t_min)
		{
			t_min = t;
			color = s->objects[i].color;
			point = vec3_add(ro, vec3_scale(rd, t));
			normal = sphere_normal(&s->objects[i], point);
		}
	}
	diffuse = saturate(vec3_dot(light, normal));
	color = vec3_lerp(s->ambient, color, diffuse);
	return (vec3_to_color(color));
}

// Trace the color of the pixel at (x, y) in the image.

uint32_t	trace_pixel(t_render *r, float x, float y)
{
	const float		u = x / (r->image->width - 1);
	const float		v = y / (r->image->height - 1);
	const t_vec3	v0 = vec3_lerp(r->viewport[0], r->viewport[1], u);
	const t_vec3	v1 = vec3_lerp(r->viewport[2], r->viewport[3], u);
	const t_vec3	rd = vec3_lerp(v0, v1, v);

	return (trace_scene(r->scene, r->scene->camera_pos, rd));
}
