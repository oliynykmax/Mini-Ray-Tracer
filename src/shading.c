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

	return (add3(s->f0, scale3(add3(one, s->f0), factor)));
}

// Check if a light is visible from a shading point (located at `ro`). This is
// done by tracing a ray toward the light, stopping as soon as an occluding
// object is hit.

static bool	light_visible(t_ray *r, t_object *light, t_vec3 ro)
{
	const t_vec3	rand = random_point_on_sphere(r->rng, light->radius);
	const t_vec3	light_pos = add3(light->pos, rand);
	const t_vec3	rd = sub3(light_pos, ro);
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
	t_vec3	specular;
	t_vec3	diffuse;
	t_vec3	radiance;

	s->view_dir = scale3(r->rd, -1.0f);
	s->light = sub3(light->pos, s->point);
	s->light_dir = norm3(s->light);
	s->halfway = norm3(add3(s->view_dir, s->light_dir));
	s->ndotv = saturate(dot3(s->normal, s->view_dir));
	s->ndotl = saturate(dot3(s->normal, s->light_dir));
	s->ndoth = saturate(dot3(s->normal, s->halfway));
	s->hdotv = saturate(dot3(s->halfway, s->view_dir));
	specular = brdf_fresnel(s);
	diffuse = sub3(vec3(1.0f, 1.0f, 1.0f), specular);
	diffuse = scale3(diffuse, (1.0f - s->metallic) / M_PI);
	diffuse = mul3(diffuse, s->albedo);
	specular = scale3(specular, brdf_geo_dist(s));
	specular = scale3(specular, 1.0f / (4.0f * s->ndotv * s->ndotl + 1e-4f));
	radiance = scale3(light->color, 2.0f);
	return (scale3(mul3(add3(diffuse, specular), radiance), s->ndotl));
}

void	apply_bumpmap(t_shading *s, t_texture bumpmap, t_vec3 tc)
{
	const float		delta = 1e-5f;
	const float		h = get_texture(bumpmap, tc.x, tc.y);
	const float		du = (get_texture(bumpmap, tc.x + delta, tc.y) - h) / delta;
	const float		dv = (get_texture(bumpmap, tc.x, tc.y + delta) - h) / delta;
	const t_vec3	m = norm3(vec3(du, dv, 1.0f / BUMP_MAP_STRENGTH));

	s->bitangent = cross3(s->normal, s->tangent);
	s->normal.x = dot3(m, vec3(s->tangent.x, s->bitangent.x, s->normal.x));
	s->normal.y = dot3(m, vec3(s->tangent.y, s->bitangent.y, s->normal.y));
	s->normal.z = dot3(m, vec3(s->tangent.z, s->bitangent.z, s->normal.z));
	s->normal = norm3(s->normal);
}

t_vec3	shade_point(t_shading *s, t_ray *r, t_object *object)
{
	t_vec3		color;
	t_object	*light;
	size_t		i;

	s->albedo.r = get_texture(object->texture, s->texcoord.x, s->texcoord.y);
	s->albedo = scale3(object->color, s->albedo.r);
	s->metallic = object->metallic;
	s->rough = fmaxf(MIN_ROUGHNESS, object->rough);
	s->f0 = lerp3(vec3(0.04f, 0.04f, 0.04f), s->albedo, s->metallic);
	s->normal = scale3(s->normal, copysignf(1.0f, -dot3(r->rd, s->normal)));
	if (object->bump != TEXTURE_NONE)
		apply_bumpmap(s, object->bump, s->texcoord);
	color = mul3(s->ambient, s->albedo);
	i = -1;
	while (++i < r->scene->object_count)
	{
		light = &r->scene->objects[i];
		if (light->type == OBJECT_LIGHT && light_visible(r, light, s->point))
			color = add3(color, one_light(r, light, s));
	}
	return (color);
}
