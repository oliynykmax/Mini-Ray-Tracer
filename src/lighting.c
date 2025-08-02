#include "minirt.h"

// Calculate the geometry (G) and normal distribution (D) factors of the
// Cook-Torrance BRDF.

static float	brdf_geo_dist(t_pbr *p)
{
	const float	rough2 = p->rough * p->rough;
	const float	denom = ((p->ndoth * p->ndoth) * (rough2 - 1.0f) + 1.0f);
	const float	ggx1 = p->ndotv / (p->ndotv * (1.0f - p->rough) + p->rough);
	const float	ggx2 = p->ndotl / (p->ndotl * (1.0f - p->rough) + p->rough);
	const float	dist = rough2 / (M_PI * denom * denom);

	return (ggx1 * ggx2 * dist);
}

// Calculate the fresnel factor (F) of the Cook-Torrance BRDF.

static t_vec3	brdf_fresnel(float hdotv, t_vec3 f0)
{
	const float	factor = powf(1.0f - hdotv, 5.0f);

	return (vec3_add(f0, vec3_scale(vec3_add(vec3(1, 1, 1), f0), factor)));
}

// Check if a light is visible from a shading point (located at `ro`). This is
// done by tracing a ray toward the light, stopping as soon as an occluding
// object is hit.

static bool	light_visible(t_ray *r, t_object *light, t_vec3 ro)
{
	const t_vec3	rand = random_point_on_sphere(r->rng, light->radius);
	const t_vec3	light_pos = vec3_add(light->pos, rand);
	const t_vec3	rd = vec3_sub(light_pos, ro);
	float			t;
	size_t			i;

	i = -1;
	while (++i < r->scene->object_count)
	{
		if (r->scene->objects[i].type == OBJECT_LIGHT)
			continue ;
		t = object_distance(&r->scene->objects[i], ro, rd);
		if (t >= 0.0f && t < 1.0f - 1e-3f)
			return (false);
	}
	return (true);
}

static t_vec3	one_light(t_ray *r, t_object *light, t_vec3 p, t_vec3 n, t_pbr *pbr)
{
	t_vec3	lv = vec3_sub(light->pos, p);
	t_vec3	v = vec3_scale(r->rd, -1.0f);
	t_vec3	l = vec3_normalize(lv);
	t_vec3	h = vec3_normalize(vec3_add(v, l));
	t_vec3	radiance = vec3_scale(light->color, 1.0f / vec3_dot(lv, lv));
	pbr->ndotv = saturate(vec3_dot(n, v));
	pbr->ndotl = saturate(vec3_dot(n, l));
	pbr->ndoth = saturate(vec3_dot(n, h));
	pbr->hdotv = saturate(vec3_dot(h, v));
	t_vec3	fresnel = brdf_fresnel(pbr->hdotv, pbr->f0);
	t_vec3	diffuse = vec3_sub(vec3(1.0f, 1.0f, 1.0f), fresnel);
	diffuse = vec3_scale(diffuse, 1.0f - pbr->metallic);
	t_vec3	specular = vec3_scale(fresnel, brdf_geo_dist(pbr));
	specular = vec3_scale(specular, 1.0f / (4.0f * pbr->ndotv * pbr->ndotl + 1e-4f));
	return vec3_scale(vec3_mul(vec3_add(vec3_scale(vec3_mul(diffuse, pbr->albedo), 1.0f / M_PI), specular), radiance), pbr->ndotl);
}

t_vec3	apply_lighting(t_ray *r, t_object *object, t_vec3 p, t_vec3 albedo)
{
	t_pbr		pbr;
	t_vec3		color;
	t_vec3		n;
	t_object	*light;
	size_t		i;

	i = -1;
	pbr.metallic = 0.9f;
	pbr.rough = 0.1f;
	pbr.f0 = vec3_lerp(vec3(0.04f, 0.04f, 0.04f), albedo, pbr.metallic);
	pbr.albedo = albedo;
	color = vec3_mul(albedo, r->scene->ambient);
	n = object_normal(object, p);
	n = vec3_scale(n, copysignf(1.0f, -vec3_dot(r->rd, n)));
	p = vec3_add(p, vec3_scale(n, 1e-5f));
	while (++i < r->scene->object_count)
	{
		light = &r->scene->objects[i];
		if (light->type == OBJECT_LIGHT && light_visible(r, light, p))
			color = vec3_add(color, one_light(r, light, p, n, &pbr));
	}
	return (color);
}
