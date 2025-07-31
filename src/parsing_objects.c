#include "minirt.h"

bool	parse_sphere(char **line, t_scene *sc)
{
	t_object	*obj;

	if (array_len(line) != 4 || objects_malloc_manager(sc))
		return (true);
	obj = &sc->objects[sc->object_count];
	obj->type = OBJECT_SPHERE;
	if (!parse_vec3(line[1], &obj->pos, 0, 0) || !parse_vec3(line[3],
			&obj->color, 0, 255))
		return (true);
	obj->radius = ft_atof(line[2]) / 2.0;
	if (!mrt_assert(obj->radius > 0.0f, "Sphere radius must be positive\n"))
		return (true);
	obj->color = vec3_scale(obj->color, 1.0 / 255.0);
	obj->rot = quat_from_axis_angle(vec3(0.0f, -1.0f, 0.0f), 0.0f);
	sc->object_count++;
	return (false);
}

bool	parse_plane(char **line, t_scene *sc)
{
	t_object	*obj;
	t_vec3		normal;

	if (array_len(line) != 4 || objects_malloc_manager(sc))
		return (true);
	obj = &sc->objects[sc->object_count];
	obj->type = OBJECT_PLANE;
	if (!parse_vec3(line[1], &obj->pos, 0, 0) || !parse_vec3(line[2],
			&normal, -1, 1) || !parse_vec3(line[3], &obj->color, 0, 255))
		return (true);
	if (!mrt_assert(fabsf(vec3_length(normal) - 1.0f) < 0.001f,
			"Plane normal must be a unit vector\n"))
		return (true);
	obj->color = vec3_scale(obj->color, 1.0 / 255.0);
	obj->rot = quat_from_direction(normal);
	sc->object_count++;
	return (false);
}

bool	parse_para(char **line, t_scene *sc)
{
	t_object	*obj;
	t_vec3		normal;

	if (array_len(line) != 6 || objects_malloc_manager(sc))
		return (true);
	obj = &sc->objects[sc->object_count];
	obj->type = OBJECT_PARA;
	if (!parse_vec3(line[1], &obj->pos, 0, 0) || !parse_vec3(line[2],
			&normal, -1, 1) || !parse_vec3(line[5], &obj->color, 0, 255))
		return (true);
	if (!mrt_assert(fabsf(vec3_length(normal) - 1.0f) < 0.001f,
			"para axis must be a unit vector\n"))
		return (true);
	obj->radius = ft_atof(line[3]) / 2.0;
	obj->height = ft_atof(line[4]);
	if (!mrt_assert(obj->radius > 0.0f, "para radius must be positive\n")
		|| !mrt_assert(obj->height > 0.0f, "para height must be positive\n"))
		return (true);
	obj->color = vec3_scale(obj->color, 1.0 / 255.0);
	obj->rot = quat_from_direction(normal);
	sc->object_count++;
	return (false);
}

bool	parse_cylinder(char **line, t_scene *sc)
{
	t_object	*obj;
	t_vec3		axis;

	if (array_len(line) != 6 || objects_malloc_manager(sc))
		return (true);
	obj = &sc->objects[sc->object_count];
	obj->type = OBJECT_CYLINDER;
	if (!parse_vec3(line[1], &obj->pos, 0, 0) || !parse_vec3(line[2],
			&axis, -1, 1) || !parse_vec3(line[5], &obj->color, 0, 255))
		return (true);
	if (!mrt_assert(fabsf(vec3_length(axis) - 1.0f) < 0.001f,
			"Cylinder axis must be a unit vector\n"))
		return (true);
	obj->radius = ft_atof(line[3]) / 2.0;
	obj->height = ft_atof(line[4]);
	if (!mrt_assert(obj->radius > 0.0f, "Cylinder diameter must be positive\n")
		|| !mrt_assert(obj->height > 0.0f,
			"Cylinder height must be positive\n"))
		return (true);
	obj->color = vec3_scale(obj->color, 1.0 / 255.0);
	obj->rot = quat_from_direction(axis);
	sc->object_count++;
	return (false);
}

bool	parse_object(char **line, t_scene *sc)
{
	if (ft_strcmp(line[0], "A") == 0)
		return (parse_amb_light(line, sc));
	else if (ft_strcmp(line[0], "L") == 0)
		return (parse_point_light(line, sc));
	else if (ft_strcmp(line[0], "sp") == 0)
		return (parse_sphere(line, sc));
	else if (ft_strcmp(line[0], "pl") == 0)
		return (parse_plane(line, sc));
	else if (ft_strcmp(line[0], "cy") == 0)
		return (parse_cylinder(line, sc));
	else if (ft_strcmp(line[0], "pa") == 0)
		return (parse_para(line, sc));
	return (true);
}
