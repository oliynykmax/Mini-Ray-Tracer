#include "minirt.h"

void	parse_amb_light(t_parse *m)
{
	static bool	ambient_exists = false;
	double		ratio;

	fatal_if(m, array_len(m->line) != 3, "Invalid ambient light format\n");
	fatal_if(m, ambient_exists, "Ambient light already exists\n");
	ratio = ft_atof(m, m->line[1]);
	parse3(m, m->line[2], &m->sc->ambient, (float []){0, 255});
	fatal_if(m, ratio < 0.0f || ratio > 1.0f,
		"Ambient light ratio must be between 0.0 and 1.0\n");
	m->sc->ambient = scale3(m->sc->ambient, ratio / 255.0);
	ambient_exists = true;
}

void	parse_point_light(t_parse *m)
{
	double	brightness;

	m->arrlen = array_len(m->line);
	fatal_if(m, (m->arrlen != 4 && m->arrlen != 5),
		"Invalid point light format\n");
	brightness = ft_atof(m, m->line[2]);
	m->obj->type = OBJECT_LIGHT;
	if (m->arrlen == 5)
	{
		m->obj->radius = ft_atof(m, m->line[4]);
		fatal_if(m, fabs(m->obj->radius) == HUGE_VAL,
			"Light radius cant be infinite\n");
	}
	fatal_if(m, m->obj->radius < 0.0f, "Light radius must be positive\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[3], &m->obj->color, (float []){0, 255});
	fatal_if(m, brightness < 0.0f, "Light brightness must be positive\n");
	m->obj->color = scale3(m->obj->color, brightness / 255.0);
	m->obj->rot = quat_from_axis_angle(vec3(0.0f, -1.0f, 0.0f), 0.0f);
}

// Then in read_map_into_scene, when errors occur:
/*
 * the command is going through array of strings that was passed to it
 * it will take the positions, split them, check if the length is 3,
	error if it is not
 * convert it to floats, check if it is within the limits
 * error if it is not
 * do the same with fov but without spliting
 *
 */
void	parse_camera(t_parse *m)
{
	static bool	camera_exist = false;

	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 4 || m->arrlen > 6, "Invalid camera format\n");
	fatal_if(m, camera_exist, "Camera already exist\n");
	m->sc->focus_depth = parse_float(m->arrlen > 4, m, 5.0f, 4);
	m->sc->aperture_size = parse_float(m->arrlen > 5, m, 0.0f, 5);
	fatal_if(m, fabs(m->sc->focus_depth) == HUGE_VAL
		|| fabs(m->sc->aperture_size) == HUGE_VAL,
		"Camera focus depth or aperture cannot be infinite\n");
	m->sc->fov = ft_atof(m, m->line[3]);
	parse3(m, m->line[1], &m->sc->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->sc->dir, (float []){-1, 1});
	camera_exist = true;
	// fatal_if(m, fabsf(len3(m->sc->dir) - 1.0f) >= 0.001f,
	// 	"Camera direction components must be between -1 and 1\n");
	fatal_if(m, m->sc->fov < 0.0f || m->sc->fov > 180.0f,
		"Camera FOV must be between 0 and 180 degrees\n");
	fatal_if(m, m->sc->focus_depth < 0.0f,
		"Camera focus depth must be non-negative\n");
	fatal_if(m, m->sc->aperture_size < 0.0f,
		"Camera aperture size must be non-negative\n");
}
