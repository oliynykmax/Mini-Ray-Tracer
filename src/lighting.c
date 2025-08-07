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

static t_vec3	brdf_fresnel(t_pbr *p)
{
	const float		factor = powf(1.0f - p->hdotv, 5.0f);
	const t_vec3	one = vec3(1.0f, 1.0f, 1.0f);

	return (vec3_add(p->f0, vec3_scale(vec3_add(one, p->f0), factor)));
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

static t_vec3	one_light(t_ray *r, t_object *light, t_pbr *p)
{
	t_vec3	radiance;
	t_vec3	spec;
	t_vec3	diff;

	p->view_dir = vec3_scale(r->rd, -1.0f);
	p->light = vec3_sub(light->pos, p->point);
	p->light_dir = vec3_normalize(p->light);
	p->halfway = vec3_normalize(vec3_add(p->view_dir, p->light_dir));
	p->ndotv = saturate(vec3_dot(p->normal, p->view_dir));
	p->ndotl = saturate(vec3_dot(p->normal, p->light_dir));
	p->ndoth = saturate(vec3_dot(p->normal, p->halfway));
	p->hdotv = saturate(vec3_dot(p->halfway, p->view_dir));
	spec = brdf_fresnel(p);
	diff = vec3_sub(vec3(1.0f, 1.0f, 1.0f), spec);
	diff = vec3_scale(diff, (1.0f - p->metallic) / M_PI);
	diff = vec3_mul(diff, p->albedo);
	diff = p->albedo;
	spec = vec3_scale(spec, brdf_geo_dist(p));
	spec = vec3_scale(spec, 1.0f / (4.0f * p->ndotv * p->ndotl + 1e-4f));
	radiance = vec3_scale(light->color, 1.0f / vec3_dot(p->light, p->light));
	return (vec3_scale(vec3_mul(vec3_add(diff, spec), radiance), p->ndotl));
}

t_vec3	bumpmap_normal(t_texture bumpmap, t_vec3 tc)
{
	tc = vec3_scale(tc, 5.0f);
	const float	eps = 1e-5f;
	const float	h = get_texture(bumpmap, tc.x, tc.y);
	const float	du = (get_texture(bumpmap, tc.x + eps, tc.y) - h) / eps;
	const float dv = (get_texture(bumpmap, tc.x, tc.y + eps) - h) / eps;

	return (vec3_normalize(vec3(du, -dv, 1e2f)));
}

t_vec3	apply_bumpmap(t_object *object, t_texture bumpmap, t_vec3 tc, t_vec3 n)
{
	(void) object;
	t_vec3	t = vec3_cross(n, vec3(0,1,0));
	if (fabsf(n.y) > 1.0f - 1e-5f)
		t = vec3_cross(n, vec3(0,0,1));
	t = vec3_normalize(t);
	t_vec3	b = vec3_cross(t, n);
	t_vec3 m = bumpmap_normal(bumpmap, tc);
	t_vec3 p;
	p.x = vec3_dot(m, vec3(t.x, b.x, n.x));
	p.y = vec3_dot(m, vec3(t.y, b.y, n.y));
	p.z = vec3_dot(m, vec3(t.z, b.z, n.z));
	return (vec3_normalize(p));
}

t_vec3	apply_lighting(t_ray *r, t_object *object, t_vec3 point)
{
	t_pbr		p;
	t_vec3		color;
	t_vec3		tc;
	t_object	*light;
	size_t		i;

	tc = object_texcoord(object, point);
	p.point = point;
	p.albedo = vec3_scale(object->color, get_texture(TEXTURE_NONE, tc.x, tc.y));
	p.metallic = 1.0f;
	p.rough = 0.1f;
	p.f0 = vec3_lerp(vec3(0.04f, 0.04f, 0.04f), p.albedo, p.metallic);
	p.normal = object_normal(object, p.point);
	p.point = vec3_add(p.point, vec3_scale(p.normal, 1e-5f));
	p.normal = apply_bumpmap(object, TEXTURE_POLKADOT, tc, p.normal);
	// p.normal = vec3_scale(p.normal, copysignf(1, -vec3_dot(r->rd, p.normal)));
#if 0
	return vec3_add(vec3_scale(p.normal, 0.5f), vec3(0.5f, 0.5f, 0.5f));
#endif
	// color = vec3_mul(p.albedo, r->scene->ambient);
	color = r->scene->ambient;
	i = -1;
	while (++i < r->scene->object_count)
	{
		light = &r->scene->objects[i];
		if (light->type == OBJECT_LIGHT && light_visible(r, light, p.point))
			color = vec3_add(color, one_light(r, light, &p));
	}
	return (color);
}
