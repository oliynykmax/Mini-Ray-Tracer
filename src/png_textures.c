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

static t_vec3	texel_fetch(mlx_texture_t *tex, uint32_t x, uint32_t y)
{
	uint8_t	*p;

	p = &tex->pixels[4 * (x % tex->width + y % tex->height * tex->width)];
	return (scale3(vec3(p[0], p[1], p[2]), 1.0f / 255.0f));
}

// Sample an image texture with bilinear filtering.

t_vec3	sample_png_color(mlx_texture_t *tex, float u, float v)
{
	t_vec3	c[4];

	u *= tex->width;
	v *= tex->height;
	c[0] = texel_fetch(tex, (int) u + 0, (int) v + 0);
	c[1] = texel_fetch(tex, (int) u + 1, (int) v + 0);
	c[2] = texel_fetch(tex, (int) u + 0, (int) v + 1);
	c[3] = texel_fetch(tex, (int) u + 1, (int) v + 1);
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
