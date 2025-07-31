#include "minirt.h"

bool	parse_amb_light(char **line, t_scene *sc)
{
	static bool	ambient_exists = false;
	double		ratio;

	if (ambient_exists || array_len(line) != 3)
		return (true);
	ratio = ft_atof(line[1]);
	if (!parse_vec3(line[2], &sc->ambient, 0, 255) || !mrt_assert(ratio >= 0.0f
			&& ratio <= 1.0f,
			"Ambient light ratio must be between 0.0 and 1.0\n"))
		return (true);
	sc->ambient = vec3_scale(vec3_scale(sc->ambient, 1.0 / 255.0), ratio);
	ambient_exists = true;
	return (false);
}

bool	parse_point_light(char **line, t_scene *sc)
{
	t_object	*obj;
	double		brightness;

	if (array_len(line) != 4 || objects_malloc_manager(sc))
		return (true);
	brightness = ft_atof(line[2]);
	obj = &sc->objects[sc->object_count];
	obj->type = OBJECT_LIGHT;
	if (!parse_vec3(line[1], &obj->pos, 0, 0) || !parse_vec3(line[3],
			&obj->color, 0, 255) || !mrt_assert(brightness >= 0.0f
			&& brightness <= 1.0f,
			"Light brightness must be between 0.0 and 1.0\n"))
		return (true);
	obj->color = vec3_scale(vec3_scale(obj->color, 1.0 / 255.0), brightness);
	obj->rot = quat_from_axis_angle(vec3(0.0f, -1.0f, 0.0f), 0.0f);
	obj->radius = 1.0f;
	sc->object_count++;
	return (false);
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
bool	parse_camera(char **line, t_scene *sc)
{
	static int	exist = 0;

	if (exist == 1 || (array_len(line) != 4 && array_len(line) != 6))
		return (true);
	if (array_len(line) == 6)
	{
		sc->focus_depth = ft_atof(line[4]);
		sc->aperture_size = ft_atof(line[5]);
	}
	sc->fov = ft_atof(line[3]);
	if (!parse_vec3(line[1], &sc->pos, 0, 0) || !parse_vec3(line[2],
			&sc->dir, -1, 1))
		return (true);
	exist = 1;
	return (!mrt_assert(fabsf(vec3_length(sc->dir) - 1.0f) < 0.001f,
			"Camera direction components must be between -1 and 1\n")
		|| !mrt_assert(sc->fov >= 0.0f && sc->fov <= 180.0f,
			"Camera FOV must be between 0 and 180 degrees\n")
		|| !mrt_assert(sc->focus_depth >= 0.0f,
			"Camera focus depth must be non-negative\n")
		|| !mrt_assert(sc->aperture_size >= 0.0f,
			"Camera aperture size must be non-negative\n"));
}
