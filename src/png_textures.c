#include "minirt.h"

// Load a PNG texture.

mlx_texture_t	*load_png_texture(t_parse *map, char *filename)
{
	char			*path;
	mlx_texture_t	*texture;

	path = ft_strjoin("assets/textures/", filename);
	fatal_if(map, path == NULL, "Out of memory\n");
	texture = mlx_load_png(path);
	free(path);
	fatal_if(map, texture == NULL, "Failed to load PNG '%s'\n", filename);
	return (texture);
}

// Sample an image texture without filtering (nearest neighbor).

static t_vec3	texel_fetch(mlx_texture_t *t, uint32_t x, uint32_t y, bool srgb)
{
	uint8_t	*p;
	t_vec3	c;

	p = &t->pixels[4 * (x % t->width + y % t->height * t->width)];
	c = scale3(vec3(p[0], p[1], p[2]), 1.0f / 255.0f);
	if (srgb)
	{
		c.r = powf(c.r, 2.3f);
		c.g = powf(c.g, 2.3f);
		c.b = powf(c.b, 2.3f);
	}
	return (c);
}

// Sample an image texture with bilinear filtering.

t_vec3	sample_png_color(mlx_texture_t *tex, float u, float v, bool srgb)
{
	t_vec3	c[4];

	u *= tex->width;
	v *= tex->height;
	c[0] = texel_fetch(tex, (int) u + 0, (int) v + 0, srgb);
	c[1] = texel_fetch(tex, (int) u + 1, (int) v + 0, srgb);
	c[2] = texel_fetch(tex, (int) u + 0, (int) v + 1, srgb);
	c[3] = texel_fetch(tex, (int) u + 1, (int) v + 1, srgb);
	c[0] = lerp3(c[0], c[1], fract(u));
	c[1] = lerp3(c[2], c[3], fract(u));
	return (lerp3(c[0], c[1], fract(v)));
}

// Free any loaded image textures attached to an object.

void	free_object_textures(t_object *obj)
{
	if (obj->texture_img != NULL)
	{
		mlx_delete_texture(obj->texture_img);
		obj->texture_img = NULL;
	}
	if (obj->bump_img != NULL)
	{
		mlx_delete_texture(obj->bump_img);
		obj->bump_img = NULL;
	}
}

void	apply_bumpmap(t_shading *s, t_object *object)
{
	t_vec3	m;

	m = sample_png_color(object->bump_img, s->texcoord.x, s->texcoord.y, false);
	m = norm3(add3(mul3(m, vec3(-2.0f, 2.0f, 2.0f)), vec3(1.0f, -1.0f, -1.0f)));
	s->bitangent = cross3(s->normal, s->tangent);
	s->normal.x = dot3(m, vec3(s->tangent.x, s->bitangent.x, s->normal.x));
	s->normal.y = dot3(m, vec3(s->tangent.y, s->bitangent.y, s->normal.y));
	s->normal.z = dot3(m, vec3(s->tangent.z, s->bitangent.z, s->normal.z));
	s->normal = norm3(s->normal);
}
