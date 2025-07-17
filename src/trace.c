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

// Apply ambient/diffuse/specular lighting to a traced ray.

static void	lighting(t_scene *s, t_ray *ray, t_vec3 rd, t_vec3 light)
{
	const t_vec3	reflect = vec3_reflect(light, ray->normal);
	const float		diffuse = saturate(vec3_dot(light, ray->normal));
	const float		specular = powf(fmaxf(0.0f, vec3_dot(rd, reflect)), 20.0f);

	ray->color = vec3_lerp(s->ambient, ray->color, diffuse);
	ray->color = vec3_lerp(ray->color, vec3(1.0f, 1.0f, 1.0f), specular);
}

// Trace the scene with ray origin `ro` and ray direction `rd`.

static t_vec3	trace_scene(t_scene *s, t_vec3 ro, t_vec3 rd)
{
	float			depth;
	t_ray			ray;
	size_t			i;

	i = -1;
	ray.depth = INFINITY;
	ray.color = s->ambient;
	while (++i < s->object_count)
	{
		depth = INFINITY;
		if (s->objects[i].type == OBJECT_SPHERE)
			depth = trace_sphere(&s->objects[i], ro, rd);
		if (depth >= 0.0f && depth < ray.depth)
		{
			ray.depth = depth;
			ray.color = s->objects[i].color;
			ray.point = vec3_add(ro, vec3_scale(rd, depth));
			ray.normal = sphere_normal(&s->objects[i], ray.point);
		}
	}
	if (ray.depth < 1e9f)
		lighting(s, &ray, rd, vec3(1.0f, 0.0f, 0.0f));
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
