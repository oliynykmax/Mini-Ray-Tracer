#include "minirt.h"

void	parse_amb_light(t_parse *m)
{
	static bool	ambient_exists = false;
	double		ratio;

	mrt_assert(m, array_len(m->line) == 3, "Invalid ambient light format\n");
	mrt_assert(m, !ambient_exists, "Ambient light already exists\n");
	ratio = ft_atof(m->line[1]);
	parse3(m, m->line[2], &m->sc->ambient, (float []){0, 255});
	mrt_assert(m, ratio >= 0.0f && ratio <= 1.0f,
		"Ambient light ratio must be between 0.0 and 1.0\n");
	m->sc->ambient = vec3_scale(vec3_scale(m->sc->ambient, 1.0 / 255.0), ratio);
	ambient_exists = true;
}

void	parse_point_light(t_parse *m)
{
	double	brightness;

	m->arrlen = array_len(m->line);
	mrt_assert(m, (m->arrlen == 4 || m->arrlen == 5),
		"Invalid point light format\n");
	objects_malloc_manager(m);
	brightness = ft_atof(m->line[2]);
	m->obj = &m->sc->objects[m->sc->object_count];
	m->obj->type = OBJECT_LIGHT;
	if (m->arrlen == 5)
		m->obj->radius = ft_atof(m->line[4]);
	mrt_assert(m, m->obj->radius >= 0.0f, "Light radius must be positive\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[3], &m->obj->color, (float []){0, 255});
	mrt_assert(m, brightness >= 0.0f, "Light brightness must be positive\n");
	mrt_warning(brightness <= 1.0f, "Light brightness is over 1.0\n");
	m->obj->color = vec3_scale(vec3_scale(m->obj->color, 1.0 / 255.0),
			brightness);
	m->obj->rot = quat_from_axis_angle(vec3(0.0f, -1.0f, 0.0f), 0.0f);
	m->sc->object_count++;
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
	mrt_assert(m, m->arrlen >= 4 && m->arrlen <= 6, "Invalid camera format\n");
	mrt_assert(m, !camera_exist, "Camera already exist\n");
	m->sc->focus_depth = parse_float(m->arrlen > 4, m->line[4], 0.0f);
	m->sc->aperture_size = parse_float(m->arrlen > 5, m->line[5], 0.0f);
	m->sc->fov = ft_atof(m->line[3]);
	parse3(m, m->line[1], &m->sc->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->sc->dir, (float []){-1, 1});
	camera_exist = true;
	mrt_assert(m, fabsf(vec3_length(m->sc->dir) - 1.0f) < 0.001f,
		"Camera direction components must be between -1 and 1\n");
	mrt_assert(m, m->sc->fov >= 0.0f && m->sc->fov <= 180.0f,
		"Camera FOV must be between 0 and 180 degrees\n");
	mrt_assert(m, m->sc->focus_depth >= 0.0f,
		"Camera focus depth must be non-negative\n");
	mrt_assert(m, m->sc->aperture_size >= 0.0f,
		"Camera aperture size must be non-negative\n");
}
