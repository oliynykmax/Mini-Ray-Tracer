#include "minirt.h"

const float	metallic = 0.1f;
const float	roughness = 0.2f;

static float	distribution(t_vec3 n, t_vec3 h, float a)
{
	const float	ndoth = fmaxf(0.0f, vec3_dot(n, h));
	const float	denom = ((ndoth * ndoth) * (a * a - 1.0f) + 1.0f);

	return (a * a) / (M_PI * denom * denom);
}

static t_vec3	fresnel(float cos_theta, t_vec3 f0)
{
	const float	factor = powf(1.0f - cos_theta, 5.0f);

	return (vec3_add(f0, vec3_scale(vec3_add(vec3(1, 1, 1), f0), factor)));
}

static float	geometry(t_vec3 n, t_vec3 v, t_vec3 l, float k)
{
	const float	ndotv = fmaxf(0.0f, vec3_dot(n, v));
	const float	ndotl = fmaxf(0.0f, vec3_dot(n, l));
	const float	ggx1 = ndotv / (ndotv * (1.0f - k) + k);
	const float	ggx2 = ndotl / (ndotl * (1.0f - k) + k);

	return (ggx1 * ggx2);
}

static bool	in_shadow(t_ray *r, t_object *light, t_vec3 p)
{
	const t_vec3	rand = random_point_on_sphere(r->rng, light->radius);
	const t_vec3	light_pos = vec3_add(light->pos, rand);
	const t_vec3	light_vec = vec3_sub(light_pos, p);
	const t_vec3	light_dir = vec3_normalize(light_vec);

	return (scene_distance(r->scene, p, light_dir, NULL) < 1.0f);
}

static t_vec3	one_light(t_ray *r, t_object *light, t_vec3 p, t_vec3 n, t_vec3 f0, t_vec3 albedo)
{
	if (in_shadow(r, light, p))
		return (vec3(0.0f, 0.0f, 0.0f));

	// Calculate radiance.
	t_vec3	lv = vec3_sub(light->pos, p);
	t_vec3	V = vec3_scale(r->rd, -1.0f);
	t_vec3	L = vec3_normalize(lv);
	t_vec3	H = vec3_normalize(vec3_add(V, L));
	t_vec3	radiance = vec3_scale(light->color, 1.0f / vec3_dot(lv, lv));

	// Cook-Torrance BRDF
	float	NDF = distribution(n, H, roughness);
	float	G = geometry(n, V, L, roughness);
	t_vec3	F = fresnel(fmaxf(0.0f, vec3_dot(H, V)), f0);
	t_vec3	kD = vec3_sub(vec3(1.0f, 1.0f, 1.0f), F);
	kD = vec3_scale(kD, 1.0f - metallic);
	t_vec3	num = vec3_scale(F, NDF * G);
	float	den = 4.0f * fmaxf(0.0f, vec3_dot(n, V)) * fmaxf(0.0f, vec3_dot(n, L)) + 1e-4f;
	t_vec3	specular = vec3_scale(num, 1.0f / den);
	float ndotl = fmaxf(0.0f, vec3_dot(n, L));
	return vec3_scale(vec3_mul(vec3_add(vec3_scale(vec3_mul(kD, albedo), 1.0f / M_PI), specular), radiance), ndotl);
}

t_vec3	apply_lighting(t_ray *r, t_object *object, t_vec3 p, t_vec3 albedo)
{
	t_vec3	color;
	t_vec3	n;
	size_t	i;

	t_vec3 f0 = vec3_lerp(vec3(0.04f, 0.04f, 0.04f), albedo, metallic);

	i = -1;
	color = vec3_mul(albedo, r->scene->ambient);
	n = object_normal(object, p);
	n = vec3_scale(n, copysignf(1.0f, -vec3_dot(r->rd, n)));
	p = vec3_add(p, vec3_scale(n, 1e-5f));
	while (++i < r->scene->object_count)
		if (r->scene->objects[i].type == OBJECT_LIGHT)
			color = vec3_add(color, one_light(r, &r->scene->objects[i], p, n, f0, albedo));
	return (color);
}
