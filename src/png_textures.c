#include "minirt.h"

static bool	validate_png_extension(const char *name)
{
	size_t	len;

	if (!name)
		return (false);
	len = ft_strlen(name);
	if (len < 4)
		return (false);
	return (ft_strcmp(name + len - 4, ".png") == 0);
}

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
	fatal_if(map, !validate_png_extension(filename),
		"Expected a .png texture file, got '%s'\n", filename);
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
t_vec3	sample_png_color(const mlx_texture_t *t, float u, float v)
{
	size_t		i;
	uint32_t	x;
	uint32_t	y;

	if (!t || t->width == 0 || t->height == 0)
		return (vec3(1, 1, 1));
	u -= floorf(u);
	v -= floorf(v);
	x = (uint32_t)(u * t->width) % t->width;
	y = (uint32_t)(v * t->height) % t->height;
	i = ((size_t)y * t->width + x) * t->bytes_per_pixel;
	return (vec3(t->pixels[i + 0] / 255.0f, t->pixels[i + 1] / 255.0f,
			t->pixels[i + 2] / 255.0f));
}

// Scalar luminance (optional; keep only if you still use it for bump).
float	sample_png_luminance(const mlx_texture_t *t, float u, float v)
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
