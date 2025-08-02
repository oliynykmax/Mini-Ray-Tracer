#include "minirt.h"

typedef struct s_pbr	t_pbr;

// Structure holding PBR lighting parameters during shading.

struct s_pbr
{
	t_vec3	f0;			// Surface reflection at zero incidence (for fresnel)
	t_vec3	albedo;		// Surface albedo at shaded point
	float	metallic;	// PBR metallic parameter
	float	roughness;	// PBR roughness parameter
	float	NdotV;		// (surface normal) · (view vector)
	float	NdotL;		// (surface normal) · (light vector)
	float	NdotH;		// (surface normal) · (halfway vector)
	float	HdotV;		// (halfway vector) · (view vector)
	t_vec3	diffuse;	// Diffuse contribution
	t_vec3	specular;	// Specular contribution
};

static float	distribution(float ndoth, float roughness)
{
	const float	roughness2 = roughness * roughness;
	const float	denom = ((ndoth * ndoth) * (roughness2 - 1.0f) + 1.0f);

	return (roughness2 / (M_PI * denom * denom));
}

static t_vec3	fresnel(float hdotv, t_vec3 f0)
{
	const float	factor = powf(1.0f - hdotv, 5.0f);

	return (vec3_add(f0, vec3_scale(vec3_add(vec3(1, 1, 1), f0), factor)));
}

static float	geometry(float ndotv, float ndotl, float k)
{
	const float	ggx1 = ndotv / (ndotv * (1.0f - k) + k);
	const float	ggx2 = ndotl / (ndotl * (1.0f - k) + k);

	return (ggx1 * ggx2);
}

static bool	is_not_in_shadow(t_ray *r, t_object *light, t_vec3 p)
{
	const t_vec3	rand = random_point_on_sphere(r->rng, light->radius);
	const t_vec3	light_pos = vec3_add(light->pos, rand);
	const t_vec3	light_vec = vec3_sub(light_pos, p);
	const t_vec3	light_dir = vec3_normalize(light_vec);

	return (scene_distance(r->scene, p, light_dir, NULL) >= 1.0f);
}

static t_vec3	one_light(t_ray *r, t_object *light, t_vec3 p, t_vec3 n, t_pbr *pbr)
{
	// Calculate radiance.
	t_vec3	lv = vec3_sub(light->pos, p);
	t_vec3	V = vec3_scale(r->rd, -1.0f);
	t_vec3	L = vec3_normalize(lv);
	t_vec3	H = vec3_normalize(vec3_add(V, L));
	t_vec3	radiance = vec3_scale(light->color, 1.0f / vec3_dot(lv, lv));

	// Cook-Torrance BRDF
	float	ndotv = saturate(vec3_dot(n, V));
	float	ndotl = saturate(vec3_dot(n, L));
	float	ndoth = saturate(vec3_dot(n, H));
	float	hdotv = saturate(vec3_dot(H, V));
	float	D = distribution(ndoth, pbr->roughness);
	float	G = geometry(ndotv, ndotl, pbr->roughness);
	t_vec3	F = fresnel(hdotv, pbr->f0);
	t_vec3	diffuse = vec3_sub(vec3(1.0f, 1.0f, 1.0f), F);
	diffuse = vec3_scale(diffuse, 1.0f - pbr->metallic);
	t_vec3	num = vec3_scale(F, D * G);
	t_vec3	specular = vec3_scale(num, 1.0f / (4.0f * ndotv * ndotl + 1e-4f));
	return vec3_scale(vec3_mul(vec3_add(vec3_scale(vec3_mul(diffuse, pbr->albedo), 1.0f / M_PI), specular), radiance), ndotl);
}

t_vec3	apply_lighting(t_ray *r, t_object *object, t_vec3 p, t_vec3 albedo)
{
	t_pbr		pbr;
	t_vec3		color;
	t_vec3		n;
	t_object	*light;
	size_t		i;

	i = -1;
	pbr.metallic = 0.1f;
	pbr.roughness = 0.2f;
	pbr.f0 = vec3_lerp(vec3(0.04f, 0.04f, 0.04f), albedo, pbr.metallic);
	pbr.albedo = albedo;
	color = vec3_mul(albedo, r->scene->ambient);
	n = object_normal(object, p);
	n = vec3_scale(n, copysignf(1.0f, -vec3_dot(r->rd, n)));
	p = vec3_add(p, vec3_scale(n, 1e-5f));
	while (++i < r->scene->object_count)
	{
		light = &r->scene->objects[i];
		if (light->type == OBJECT_LIGHT && is_not_in_shadow(r, light, p))
			color = vec3_add(color, one_light(r, light, p, n, &pbr));
	}
	return (color);
}
