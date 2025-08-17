#include "minirt.h"

/* Build path "textures/<filename>".
 * Caller owns returned malloc'd string.
 */
static char	*build_texture_path(t_parse *map, const char *filename)
{
	const char	*prefix = "textures/";
	size_t		len_prefix;
	size_t		len_file;
	char		*out;

	len_prefix = ft_strlen(prefix);
	len_file = ft_strlen(filename);
	out = malloc(len_prefix + len_file + 1);
	fatal_if(map, out == NULL, "Allocation failure building texture path\n");
	ft_memcpy(out, prefix, len_prefix);
	ft_memcpy(out + len_prefix, filename, len_file + 1);
	return (out);
}

/*
 * load_png_texture
 *  Loads a PNG file from mrt/textures/<filename>.
 *  On any failure, invokes fatal_if (exits).
 *  On success: *out receives the loaded texture pointer.
 */
bool	load_png_texture(t_parse *map, const char *filename,
		mlx_texture_t **out)
{
	char			*full;
	mlx_texture_t	*tex;

	fatal_if(map, out == NULL, "Internal: NULL output texture pointer\n");
	*out = NULL;
	full = build_texture_path(map, filename);
	tex = mlx_load_png(full);
	if (tex == NULL)
	{
		free(full);
		fatal_if(map, true, "Failed to load PNG '%s'\n", filename);
	}
	if (tex->pixels == NULL || tex->width == 0 || tex->height == 0)
	{
		free(full);
		mlx_delete_texture(tex);
		fatal_if(map, true, "Corrupt PNG '%s'\n", filename);
	}
	free(full);
	*out = tex;
	return (true);
}

/* Bilinear filtered, repeat-wrapped sampling of RGB.
 * Returns linear RGB in [0,1].
 */
// Nearest-neighbor, repeat-wrapped RGB sample (very short version).
static inline t_vec3	png_fetch(const mlx_texture_t *t, int x, int y)
{
	size_t	i;

	x = (x % (int)t->width + (int)t->width) % (int)t->width;
	y = (y % (int)t->height + (int)t->height) % (int)t->height;
	i = ((size_t)y * t->width + (uint32_t)x) * t->bytes_per_pixel;
	return (vec3(t->pixels[i] / 255.0f, t->pixels[i + 1] / 255.0f,
			t->pixels[i + 2] / 255.0f));
}

t_vec3	sample_png_color(const mlx_texture_t *t, float u, float v)
{
	int		w;
	float	uu;
	float	vv;
	int		a[4];
	t_vec3	c;

	if (!t || t->width == 0 || t->height == 0)
		return (vec3(1, 1, 1));
	u -= floorf(u);
	v -= floorf(v);
	w = (int)t->width;
	uu = u * w;
	vv = v * (int)t->height;
	a[0] = (int)floorf(uu);
	a[2] = (int)floorf(vv);
	a[1] = (a[0] + 1) % w;
	a[3] = (a[2] + 1) % (int)t->height;
	uu -= a[0];
	vv -= a[2];
	c = add3(
			add3(scale3(png_fetch(t, a[0], a[2]), (1.0f - uu) * (1.0f - vv)),
				scale3(png_fetch(t, a[1], a[2]), uu * (1.0f - vv))),
			add3(scale3(png_fetch(t, a[0], a[3]), (1.0f - uu) * vv),
				scale3(png_fetch(t, a[1], a[3]), uu * vv)));
	return (c);
}

// Scalar luminance (optional; keep only if you still use it for bump).
float	sample_lumin(const mlx_texture_t *t, float u, float v)
{
	t_vec3	c;

	c = sample_png_color(t, u, v);
	return (saturate(c.r * 0.2126f + c.g * 0.7152f + c.b * 0.0722f));
}

/* Free any loaded image textures attached to an object. */
void	free_object_textures(t_object *obj)
{
	if (!obj)
		return ;
	if (obj->texture_img)
	{
		mlx_delete_texture(obj->texture_img);
		obj->texture_img = NULL;
	}
	if (obj->bump_img)
	{
		mlx_delete_texture(obj->bump_img);
		obj->bump_img = NULL;
	}
}
