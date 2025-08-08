#include "minirt.h"

// Calculate the geometry (G) and normal distribution (D) factors of the
// Cook-Torrance BRDF.

static float	brdf_geo_dist(t_shading *s)
{
	const float	rough2 = s->rough * s->rough;
	const float	denom = ((s->ndoth * s->ndoth) * (rough2 - 1.0f) + 1.0f);
	const float	ggx1 = s->ndotv / (s->ndotv * (1.0f - s->rough) + s->rough);
	const float	ggx2 = s->ndotl / (s->ndotl * (1.0f - s->rough) + s->rough);
	const float	dist = rough2 / (M_PI * denom * denom);

	return (ggx1 * ggx2 * dist);
}

// Calculate the fresnel factor (F) of the Cook-Torrance BRDF.

static t_vec3	brdf_fresnel(t_shading *s)
{
	const float		factor = powf(1.0f - s->hdotv, 5.0f);
	const t_vec3	one = vec3(1.0f, 1.0f, 1.0f);

	return (vec3_add(s->f0, vec3_scale(vec3_add(one, s->f0), factor)));
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

static t_vec3	one_light(t_ray *r, t_object *light, t_shading *s)
{
	t_vec3	radiance;
	t_vec3	spec;
	t_vec3	diff;

	s->view_dir = vec3_scale(r->rd, -1.0f);
	s->light = vec3_sub(light->pos, s->point);
	s->light_dir = vec3_normalize(s->light);
	s->halfway = vec3_normalize(vec3_add(s->view_dir, s->light_dir));
	s->ndotv = saturate(vec3_dot(s->normal, s->view_dir));
	s->ndotl = saturate(vec3_dot(s->normal, s->light_dir));
	s->ndoth = saturate(vec3_dot(s->normal, s->halfway));
	s->hdotv = saturate(vec3_dot(s->halfway, s->view_dir));
	spec = brdf_fresnel(s);
	diff = vec3_sub(vec3(1.0f, 1.0f, 1.0f), spec);
	diff = vec3_scale(diff, (1.0f - s->metallic) / M_PI);
	diff = vec3_mul(diff, s->albedo);
	diff = s->albedo;
	spec = vec3_scale(spec, brdf_geo_dist(s));
	spec = vec3_scale(spec, 1.0f / (4.0f * s->ndotv * s->ndotl + 1e-4f));
	radiance = vec3_scale(light->color, 1.0f / vec3_dot(s->light, s->light));
	return (vec3_scale(vec3_mul(vec3_add(diff, spec), radiance), s->ndotl));
}

void	apply_bumpmap(t_shading *s, t_texture bumpmap, t_vec3 tc)
{
	const float		delta = 1e-5f;
	const float		h = get_texture(bumpmap, tc.x, tc.y);
	const float		du = (get_texture(bumpmap, tc.x + delta, tc.y) - h) / delta;
	const float		dv = (get_texture(bumpmap, tc.x, tc.y + delta) - h) / delta;
	const t_vec3	m = vec3_normalize(vec3(du, dv, 100.0f));

	s->normal.x = vec3_dot(m, vec3(s->tangent.x, s->bitangent.x, s->normal.x));
	s->normal.y = vec3_dot(m, vec3(s->tangent.y, s->bitangent.y, s->normal.y));
	s->normal.z = vec3_dot(m, vec3(s->tangent.z, s->bitangent.z, s->normal.z));
	s->normal = vec3_normalize(s->normal);
}

t_vec3	shade_point(t_ray *r, t_object *object, t_vec3 point)
{
	t_shading	s;
	t_vec3		color;
	t_object	*light;
	size_t		i;

	s.point = point;
	object_params(object, &s);
	s.albedo.r = get_texture(TEXTURE_CHECKED, s.texcoord.x, s.texcoord.y);
	s.albedo = vec3_scale(object->color, 0.5f + 0.5f * s.albedo.r);
	s.metallic = 1.0f;
	s.rough = 0.1f;
	s.f0 = vec3_lerp(vec3(0.04f, 0.04f, 0.04f), s.albedo, s.metallic);
	s.point = vec3_add(s.point, vec3_scale(s.normal, 1e-5f));
	apply_bumpmap(&s, TEXTURE_POLKADOT, vec3_scale(s.texcoord, 5.0f));
	color = r->scene->ambient;
	i = -1;
	while (++i < r->scene->object_count)
	{
		light = &r->scene->objects[i];
		if (light->type == OBJECT_LIGHT && light_visible(r, light, s.point))
			color = vec3_add(color, one_light(r, light, &s));
	}
	return (color);
}
