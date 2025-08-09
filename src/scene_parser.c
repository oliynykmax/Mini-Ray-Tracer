#include "minirt.h"

/* Unified scene parser with optional roughness + texture (and light radius).
 * On ANY misconfiguration: print Error\n<msg> and exit. All helper functions
 * kept under 25 lines. Old per-object parse_* functions remain unused to avoid
 * symbol conflicts. */

static void	fatal(t_scene *sc, const char *msg)
{
	mrt_assert(false, (char *)msg);
	cleanup_scene(sc);
	exit(EXIT_FAILURE);
}

static int	is_float(const char *s)
{
	int	d, p;
	if (*s == '+' || *s == '-') s++;
	if (!*s) return (0);
	d = 0; p = 0;
	while (*s)
	{
		if (*s == '.')
		{
			if (p) return (0);
			p = 1;
		}
		else if (*s < '0' || *s > '9')
			return (0);
		else
			d = 1;
		s++;
	}
	return (d);
}

static t_texture	parse_texture(const char *t)
{
	if (!t) return (TEXTURE_NONE);
	if (ft_strcmp(t, "none") == 0) return (TEXTURE_NONE);
	if (ft_strcmp(t, "checked") == 0) return (TEXTURE_CHECKED);
	if (ft_strcmp(t, "zigzag") == 0) return (TEXTURE_ZIGZAG);
	if (ft_strcmp(t, "polkadot") == 0) return (TEXTURE_POLKADOT);
	return ((t_texture)-1);
}

static void	parse_object_opts(t_object *o, char **tok, int i, int n, t_scene *sc)
{
	int	rough_set = 0, tex_set = 0;
	while (i < n)
	{
		if (!rough_set && is_float(tok[i]))
		{
			o->rough = ft_atof(tok[i]);
			if (o->rough < 0.0f || o->rough > 1.0f)
				fatal(sc, "Roughness must be in [0,1]\n");
			rough_set = 1;
		}
		else if (!tex_set)
		{
			o->texture = parse_texture(tok[i]);
			if (o->texture == (t_texture)-1)
				fatal(sc, "Unknown texture name\n");
			tex_set = 1;
		}
		else
			fatal(sc, "Too many optional tokens\n");
		i++;
	}
	if (!rough_set) o->rough = 0.1f;
}

static void	parse_light_opts(t_object *o, char **tok, int i, int n, t_scene *sc)
{
	int	radius_set = 0, rough_set = 0, tex_set = 0;
	while (i < n)
	{
		if (!radius_set && is_float(tok[i]))
		{
			o->radius = ft_atof(tok[i]);
			if (o->radius < 0.0f) fatal(sc, "Light radius must be >= 0\n");
			radius_set = 1;
		}
		else if (!rough_set && is_float(tok[i]))
		{
			o->rough = ft_atof(tok[i]);
			if (o->rough < 0.0f || o->rough > 1.0f)
				fatal(sc, "Roughness must be in [0,1]\n");
			rough_set = 1;
		}
		else if (!tex_set)
		{
			o->texture = parse_texture(tok[i]);
			if (o->texture == (t_texture)-1)
				fatal(sc, "Unknown texture name\n");
			tex_set = 1;
		}
		else
			fatal(sc, "Too many light option tokens\n");
		i++;
	}
	if (!rough_set) o->rough = 0.1f;
}

static t_object	*new_object(t_scene *sc, t_object_type type)
{
	t_object *o;
	if (objects_malloc_manager(sc))
		fatal(sc, "Allocation failure\n");
	o = &sc->objects[sc->object_count++];
	ft_bzero(o, sizeof(*o));
	o->type = type;
	o->texture = TEXTURE_NONE;
	o->rough = 0.1f;
	o->rot = quat_from_axis_angle(vec3(0, -1, 0), 0.0f);
	return (o);
}

static void	parse_ambient(char **t, int n, t_scene *sc)
{
	static int done = 0;
	double r;
	if (done) fatal(sc, "Duplicate ambient\n");
	if (n != 3) fatal(sc, "Ambient: A ratio R,G,B\n");
	r = ft_atof(t[1]);
	if (r < 0 || r > 1) fatal(sc, "Ambient ratio in [0,1]\n");
	if (!parse3(t[2], &sc->ambient, 0, 255))
		fatal(sc, "Ambient color\n");
	sc->ambient = vec3_scale(vec3_scale(sc->ambient, 1.0f/255.0f), r);
	done = 1;
}

static void	parse_camera_line(char **t, int n, t_scene *sc)
{
	static int done = 0;
	if (done) fatal(sc, "Duplicate camera\n");
	if (n != 4 && n != 6) fatal(sc, "Camera: C pos dir fov [focus aperture]\n");
	if (!parse3(t[1], &sc->pos, 0, 0)) fatal(sc, "Camera position\n");
	if (!parse3(t[2], &sc->dir, -1, 1)) fatal(sc, "Camera direction\n");
	sc->fov = ft_atof(t[3]);
	if (sc->fov < 0 || sc->fov > 180) fatal(sc, "FOV in [0,180]\n");
	if (n == 6)
	{
		sc->focus_depth = ft_atof(t[4]);
		sc->aperture_size = ft_atof(t[5]);
		if (sc->focus_depth < 0) fatal(sc, "Focus depth >=0\n");
		if (sc->aperture_size < 0) fatal(sc, "Aperture >=0\n");
	}
	if (fabsf(vec3_length(sc->dir) - 1.0f) >= 0.001f)
		fatal(sc, "Camera direction must be unit\n");
	done = 1;
}

static void	parse_sphere_line(char **t, int n, t_scene *sc)
{
	t_object *o;
	if (n < 4) fatal(sc, "Sphere: sp pos diameter R,G,B [...]\n");
	o = new_object(sc, OBJECT_SPHERE);
	if (!parse3(t[1], &o->pos, 0, 0)) fatal(sc, "Sphere position\n");
	o->radius = ft_atof(t[2]) / 2.0f;
	if (o->radius <= 0) fatal(sc, "Sphere diameter > 0\n");
	if (!parse3(t[3], &o->color, 0, 255)) fatal(sc, "Sphere color\n");
	o->color = vec3_scale(o->color, 1.0f/255.0f);
	parse_object_opts(o, t, 4, n, sc);
}

static void	parse_plane_line(char **t, int n, t_scene *sc)
{
	t_vec3 nrm; t_object *o;
	if (n < 4) fatal(sc, "Plane: pl pos normal R,G,B [...]\n");
	o = new_object(sc, OBJECT_PLANE);
	if (!parse3(t[1], &o->pos, 0, 0)) fatal(sc, "Plane position\n");
	if (!parse3(t[2], &nrm, -1, 1)) fatal(sc, "Plane normal\n");
	if (fabsf(vec3_length(nrm) - 1.0f) >= 0.001f)
		fatal(sc, "Plane normal unit\n");
	if (!parse3(t[3], &o->color, 0, 255)) fatal(sc, "Plane color\n");
	o->color = vec3_scale(o->color, 1.0f/255.0f);
	o->rot = quat_from_direction(nrm);
	parse_object_opts(o, t, 4, n, sc);
}

static void	parse_cylinder_line(char **t, int n, t_scene *sc)
{
	t_vec3 ax; t_object *o;
	if (n < 6) fatal(sc, "Cylinder: cy pos normal diameter height R,G,B [...]\n");
	o = new_object(sc, OBJECT_CYLINDER);
	if (!parse3(t[1], &o->pos, 0, 0)) fatal(sc, "Cylinder position\n");
	if (!parse3(t[2], &ax, -1, 1)) fatal(sc, "Cylinder axis\n");
	if (fabsf(vec3_length(ax) - 1.0f) >= 0.001f)
		fatal(sc, "Cylinder axis unit\n");
	o->radius = ft_atof(t[3]) / 2.0f;
	o->height = ft_atof(t[4]);
	if (o->radius <= 0) fatal(sc, "Cylinder diameter > 0\n");
	if (o->height <= 0) fatal(sc, "Cylinder height > 0\n");
	if (!parse3(t[5], &o->color, 0, 255)) fatal(sc, "Cylinder color\n");
	o->color = vec3_scale(o->color, 1.0f/255.0f);
	o->rot = quat_from_direction(ax);
	parse_object_opts(o, t, 6, n, sc);
}

static void	parse_paraboloid_line(char **t, int n, t_scene *sc)
{
	t_vec3 ax; t_object *o;
	if (n < 6) fatal(sc, "Paraboloid: pa pos axis diameter height R,G,B [...]\n");
	o = new_object(sc, OBJECT_PARA);
	if (!parse3(t[1], &o->pos, 0, 0)) fatal(sc, "Paraboloid position\n");
	if (!parse3(t[2], &ax, -1, 1)) fatal(sc, "Paraboloid axis\n");
	if (fabsf(vec3_length(ax) - 1.0f) >= 0.001f)
		fatal(sc, "Paraboloid axis unit\n");
	o->radius = ft_atof(t[3]) / 2.0f;
	o->height = ft_atof(t[4]);
	if (o->radius <= 0) fatal(sc, "Paraboloid diameter > 0\n");
	if (o->height <= 0) fatal(sc, "Paraboloid height > 0\n");
	if (!parse3(t[5], &o->color, 0, 255)) fatal(sc, "Paraboloid color\n");
	o->color = vec3_scale(o->color, 1.0f/255.0f);
	o->rot = quat_from_direction(ax);
	parse_object_opts(o, t, 6, n, sc);
}

static void	parse_light_line(char **t, int n, t_scene *sc)
{
	t_object *o; t_vec3 raw;
	if (n < 4) fatal(sc, "Light: L pos brightness R,G,B [...]\n");
	o = new_object(sc, OBJECT_LIGHT);
	if (!parse3(t[1], &o->pos, 0, 0)) fatal(sc, "Light position\n");
	double br = ft_atof(t[2]);
	if (br < 0) fatal(sc, "Light brightness >= 0\n");
	if (!parse3(t[3], &raw, 0, 255)) fatal(sc, "Light color\n");
	o->color = vec3_scale(vec3_scale(raw, 1.0f/255.0f), br);
	o->radius = 0.0f;
	parse_light_opts(o, t, 4, n, sc);
}

static void	dispatch_tokens(char **t, int n, t_scene *sc)
{
	if (ft_strcmp(t[0], "A") == 0) parse_ambient(t, n, sc);
	else if (ft_strcmp(t[0], "C") == 0) parse_camera_line(t, n, sc);
	else if (ft_strcmp(t[0], "L") == 0) parse_light_line(t, n, sc);
	else if (ft_strcmp(t[0], "sp") == 0) parse_sphere_line(t, n, sc);
	else if (ft_strcmp(t[0], "pl") == 0) parse_plane_line(t, n, sc);
	else if (ft_strcmp(t[0], "cy") == 0) parse_cylinder_line(t, n, sc);
	else if (ft_strcmp(t[0], "pa") == 0) parse_paraboloid_line(t, n, sc);
	else fatal(sc, "Unknown identifier\n");
}

static void	parse_line(t_scene *sc, char *line)
{
	char *nl; char **tok; int n;
	nl = ft_strrchr(line, '\n');
	if (nl) *nl = '\0';
	if (ft_str_is_whitespace(line)) return ;
	tok = ft_split(line, ' ');
	if (!tok) fatal(sc, "Split failure\n");
	n = array_len(tok);
	if (n > 0) dispatch_tokens(tok, n, sc);
	free_array(tok);
}

void	load_scene(t_scene *sc, const char *file)
{
	int fd; char *ln;
	fd = open(file, O_RDONLY);
	if (fd < 0) fatal(sc, "Cannot open file\n");
	while ((ln = get_next_line(fd)))
	{
		parse_line(sc, ln);
		free(ln);
	}
	get_next_line(-1);
	close(fd);
	if (sc->object_count == 0) fatal(sc, "No objects in scene\n");
}