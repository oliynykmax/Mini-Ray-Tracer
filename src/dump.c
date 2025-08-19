#include "minirt.h"

static int	to_u8(float x)
{
	if (x < 0.0f) x = 0.0f;
	if (x > 1.0f) x = 1.0f;
	return (int)(x * 255.0f + 0.5f);
}

static void	fprint_trimmed_float(FILE *f, float x)
{
	char	buf[64];
	int		len;

	/* Snap very small to zero to avoid 1.7e-07 style noise */
	if (fabsf(x) < 1e-6f)
		x = 0.0f;
	/* If very close to integer, print as integer */
	if (fabsf(x - roundf(x)) < 1e-5f)
	{
		fprintf(f, "%.0f", roundf(x));
		return ;
	}
	/* Fixed 6 decimals, then trim trailing zeros */
	snprintf(buf, sizeof(buf), "%.6f", x);
	len = (int)strlen(buf);
	while (len > 0 && buf[len - 1] == '0')
		buf[--len] = '\0';
	if (len > 0 && buf[len - 1] == '.')
		buf[--len] = '\0';
	fprintf(f, "%s", buf);
}

static void	fprint_vec3_csv(FILE *f, t_vec3 v)
{
	fprint_trimmed_float(f, v.x);
	fputc(',', f);
	fprint_trimmed_float(f, v.y);
	fputc(',', f);
	fprint_trimmed_float(f, v.z);
}

static void	fprint_color_csv_scaled(FILE *f, t_vec3 v_scaled)
{
	/* v_scaled is assumed in linear [0, ~1]; clamp & scale */
	fprintf(f, "%d,%d,%d", to_u8(v_scaled.r), to_u8(v_scaled.g), to_u8(v_scaled.b));
}

static t_vec3	reconstruct_normal_from_quat(t_quat q)
{
	/* World "up" basis (0,1,0) rotated by the stored orientation */
	return norm3(quat_rotate_vec3(q, vec3(0.0f, 1.0f, 0.0f)));
}

static void	dump_ambient(FILE *f, t_scene *scene)
{
	const t_vec3 a1 = scene->ambient;
	const t_vec3 a2 = scene->ambient2;
	const float maxc1 = fmaxf(a1.r, fmaxf(a1.g, a1.b));
	const float maxc2 = fmaxf(a2.r, fmaxf(a2.g, a2.b));
	const float eps = 1e-6f;
	float		ratio;
	t_vec3		base1;
	t_vec3		base2;
	bool		has_second;

	ratio = maxc1;
	if (ratio < eps)
		ratio = 0.0f;
	if (ratio > 0.0f)
	{
		base1 = vec3(a1.r / ratio, a1.g / ratio, a1.b / ratio);
		base1 = scale3(base1, 255.0f);
	}
	else
		base1 = vec3(0, 0, 0);
	/* Ambient2 assumed to share ratio; if its max differs a lot, we still
	   expand with its own (safer for gradients). */
	if (maxc2 > eps)
	{
		base2 = vec3(a2.r / maxc2, a2.g / maxc2, a2.b / maxc2);
		base2 = scale3(base2, 255.0f);
	}
	else
		base2 = vec3(0, 0, 0);
	has_second = (fabsf(a1.r - a2.r) > 1e-5f
		|| fabsf(a1.g - a2.g) > 1e-5f
		|| fabsf(a1.b - a2.b) > 1e-5f);
	fprintf(f, "A ");
	fprint_trimmed_float(f, ratio);
	fputc(' ', f);
	fprint_color_csv_scaled(f, scale3(base1, 1.0f / 255.0f));
	if (has_second || scene->amb_texture)
	{
		fputc(' ', f);
		if (has_second)
			fprint_color_csv_scaled(f, scale3(base2, 1.0f / 255.0f));
		else
			fprintf(f, "_");
		fprintf(f, " _"); /* Texture placeholder */
	}
	fputc('\n', f);
}

static void	dump_camera(FILE *f, t_scene *scene)
{
	fprintf(f, "C ");
	fprint_vec3_csv(f, scene->pos);
	fputc(' ', f);
	fprint_vec3_csv(f, scene->dir);
	fputc(' ', f);
	fprint_trimmed_float(f, scene->fov);
	fputc(' ', f);
	fprint_trimmed_float(f, scene->focus_depth);
	fputc(' ', f);
	fprint_trimmed_float(f, scene->aperture_size);
	fputc('\n', f);
}

static void	dump_light(FILE *f, t_object *o)
{
	const float	br = fmaxf(o->color.r, fmaxf(o->color.g, o->color.b));
	t_vec3		base;

	fprintf(f, "L ");
	fprint_vec3_csv(f, o->pos);
	fputc(' ', f);
	fprint_trimmed_float(f, br);
	fputc(' ', f);
	if (br > 0.0f)
	{
		base = vec3(o->color.r / br, o->color.g / br, o->color.b / br);
		base = scale3(base, 255.0f);
		fprint_color_csv_scaled(f, scale3(base, 1.0f / 255.0f));
	}
	else
		fprintf(f, "0,0,0");
	if (o->radius > 0.0f)
	{
		fputc(' ', f);
		fprint_trimmed_float(f, o->radius);
	}
	fputc('\n', f);
}

static void	dump_sphere(FILE *f, t_object *o)
{
	fprintf(f, "sp ");
	fprint_vec3_csv(f, o->pos);
	fputc(' ', f);
	fprint_trimmed_float(f, o->radius * 2.0f);
	fputc(' ', f);
	fprint_color_csv_scaled(f, o->color);
	fputc(' ', f);
	fprint_trimmed_float(f, o->rough);
	fputc(' ', f);
	fprint_trimmed_float(f, o->metallic);
	fprintf(f, " _ _\n");
}

static void	dump_plane(FILE *f, t_object *o)
{
	t_vec3 n = reconstruct_normal_from_quat(o->rot);

	fprintf(f, "pl ");
	fprint_vec3_csv(f, o->pos);
	fputc(' ', f);
	fprint_vec3_csv(f, n);
	fputc(' ', f);
	fprint_color_csv_scaled(f, o->color);
	fputc(' ', f);
	fprint_trimmed_float(f, o->rough);
	fputc(' ', f);
	fprint_trimmed_float(f, o->metallic);
	fprintf(f, " _ _\n");
}

static void	dump_cylinder(FILE *f, t_object *o)
{
	t_vec3 n = reconstruct_normal_from_quat(o->rot);

	fprintf(f, "cy ");
	fprint_vec3_csv(f, o->pos);
	fputc(' ', f);
	fprint_vec3_csv(f, n);
	fputc(' ', f);
	fprint_trimmed_float(f, o->radius * 2.0f);
	fputc(' ', f);
	fprint_trimmed_float(f, o->height);
	fputc(' ', f);
	fprint_color_csv_scaled(f, o->color);
	fputc(' ', f);
	fprint_trimmed_float(f, o->rough);
	fputc(' ', f);
	fprint_trimmed_float(f, o->metallic);
	fprintf(f, " _ _\n");
}

static void	dump_paraboloid(FILE *f, t_object *o)
{
	t_vec3 n = reconstruct_normal_from_quat(o->rot);

	fprintf(f, "pa ");
	fprint_vec3_csv(f, o->pos);
	fputc(' ', f);
	fprint_vec3_csv(f, n);
	fputc(' ', f);
	fprint_trimmed_float(f, o->radius * 2.0f);
	fputc(' ', f);
	fprint_trimmed_float(f, o->height);
	fputc(' ', f);
	fprint_color_csv_scaled(f, o->color);
	fputc(' ', f);
	fprint_trimmed_float(f, o->rough);
	fputc(' ', f);
	fprint_trimmed_float(f, o->metallic);
	fprintf(f, " _ _\n");
}

static void	dump_box(FILE *f, t_object *o)
{
	t_vec3 n = reconstruct_normal_from_quat(o->rot);

	fprintf(f, "bx ");
	fprint_vec3_csv(f, o->pos);
	fputc(' ', f);
	fprint_vec3_csv(f, n);
	fputc(' ', f);
	fprint_vec3_csv(f, o->size);
	fputc(' ', f);
	fprint_color_csv_scaled(f, o->color);
	fputc(' ', f);
	fprint_trimmed_float(f, o->rough);
	fputc(' ', f);
	fprint_trimmed_float(f, o->metallic);
	fprintf(f, " _ _\n");
}

static void	dump_object(FILE *f, t_object *o)
{
	switch (o->type)
	{
		case OBJECT_LIGHT:    dump_light(f, o); break;
		case OBJECT_SPHERE:   dump_sphere(f, o); break;
		case OBJECT_PLANE:    dump_plane(f, o); break;
		case OBJECT_CYLINDER: dump_cylinder(f, o); break;
		case OBJECT_PARA:     dump_paraboloid(f, o); break;
		case OBJECT_BOX:      dump_box(f, o); break;
		default: /* Unknown type - ignore */ break;
	}
}

bool	dump_scene_to_file(t_scene *scene, const char *path)
{
	FILE	*f;
	size_t	i;

	if (scene == NULL || path == NULL)
		return (false);
	f = fopen(path, "w");
	if (!f)
		return (false);

	fprintf(f, "# Dump generated by runtime scene dumper (approximate)\n");
	dump_ambient(f, scene);
	dump_camera(f, scene);
	i = 0;
	while (i < scene->object_count)
	{
		dump_object(f, &scene->objects[i]);
		++i;
	}
	fclose(f);
	return (true);
}

bool	dump_scene_prompt(t_render *r)
{
	char	name[128];
	size_t	len;
	char	path[256];

	if (r == NULL || r->scene == NULL)
		return (false);
	fprintf(stderr, "Enter map name (without extension, empty to cancel): ");
	fflush(stderr);
	if (fgets(name, sizeof(name), stdin) == NULL)
	{
		fprintf(stderr, "Scene dump canceled (no input)\n");
		return (false);
	}
	len = strlen(name);
	while (len > 0 && (name[len - 1] == '\n' || name[len - 1] == '\r'))
		name[--len] = '\0';
	if (len == 0)
	{
		fprintf(stderr, "Scene dump canceled (empty name)\n");
		return (false);
	}
	for (size_t i = 0; i < len; ++i)
	{
		char c = name[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
				|| (c >= '0' && c <= '9') || c == '_' || c == '-'))
			name[i] = '_';
	}
	snprintf(path, sizeof(path), "maps/%s.rt", name);
	if (dump_scene_to_file(r->scene, path))
	{
		fprintf(stderr, "Dumped scene to %s\n", path);
		return (true);
	}
	fprintf(stderr, "Scene dump failed\n");
	return (false);
}
