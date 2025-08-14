#include "minirt.h"

void	parse_sphere(t_parse *m)
{
	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 4 || m->arrlen > 8, "Invalid sphere format\n");
	m->obj->type = OBJECT_SPHERE;
	parse_optionals(m, 4);
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[3], &m->obj->color, (float []){0, 255});
	m->obj->radius = ft_atof(m, m->line[2]) * 0.5;
	fatal_if(m, fabs(m->obj->radius) == HUGE_VAL,
		"Sphere radius can't be inf\n");
	fatal_if(m, m->obj->radius <= 0.0f, "Sphere radius must be positive\n");
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	m->obj->rot = quat_from_axis_angle(vec3(0.0f, -1.0f, 0.0f), 0.0f);
}

void	parse_plane(t_parse *m)
{
	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 4 || m->arrlen > 8, "Invalid plane format\n");
	m->obj->type = OBJECT_PLANE;
	parse_optionals(m, 4);
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	parse3(m, m->line[3], &m->obj->color, (float []){0, 255});
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	fatal_if(m, fabsf(len3(m->normal) - 1.0f) >= 0.001f,
		"Plane normal must be a unit vector\n");
	m->obj->rot = quat_from_direction(m->normal);
}

void	parse_para(t_parse *m)
{
	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 6 || m->arrlen > 10, "Invalid para format\n");
	m->obj->type = OBJECT_PARA;
	parse_optionals(m, 6);
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	fatal_if(m, fabsf(len3(m->normal) - 1.0f) >= 0.001f,
		"Paraboloid axis must be a unit vector\n");
	parse3(m, m->line[5], &m->obj->color, (float []){0, 255});
	m->obj->radius = ft_atof(m, m->line[3]) * 0.5;
	m->obj->height = ft_atof(m, m->line[4]);
	fatal_if(m, fabs(m->obj->radius) == HUGE_VAL
		|| fabs(m->obj->height) == HUGE_VAL,
		"Object radius and height cannot be infinite\n");
	fatal_if(m, m->obj->radius <= 0.0f || m->obj->height <= 0.0f,
		"Para radius and height must be positive\n");
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	m->obj->rot = quat_from_direction(m->normal);
}

void	parse_cylinder(t_parse *m)
{
	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 6 || m->arrlen > 10, "Invalid Cy format\n");
	m->obj->type = OBJECT_CYLINDER;
	parse_optionals(m, 6);
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	fatal_if(m, fabsf(len3(m->normal) - 1.0f) >= 0.001f,
		"Cylinder axis must be a unit vector\n");
	parse3(m, m->line[5], &m->obj->color, (float []){0, 255});
	m->obj->radius = ft_atof(m, m->line[3]) * 0.5;
	m->obj->height = ft_atof(m, m->line[4]);
	fatal_if(m, fabs(m->obj->radius) == HUGE_VAL
		|| fabs(m->obj->height) == HUGE_VAL,
		"Object radius and height cannot be infinite\n");
	fatal_if(m, m->obj->radius <= 0.0f || m->obj->height <= 0.0f,
		"Cylinder radius and height must be positive\n");
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	m->obj->rot = quat_from_direction(m->normal);
}

void	parse_box(t_parse *m)
{
	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 5 || m->arrlen > 9, "Invalid box format\n");
	m->obj->type = OBJECT_BOX;
	parse_optionals(m, 5);
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	fatal_if(m, fabsf(len3(m->normal) - 1.0f) >= 0.001f,
		"Box axis must be a unit vector\n");
	parse3(m, m->line[3], &m->obj->size, (float []){0, 0});
	fatal_if(m, m->obj->size.x <= 0.0f || m->obj->size.y <= 0.0f
		|| m->obj->size.z <= 0.0f, "Box dimensions must be positive\n");
	parse3(m, m->line[4], &m->obj->color, (float []){0, 255});
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	m->obj->rot = quat_from_direction(m->normal);
}

void	parse_type(t_parse *m)
{
	const char			*id = m->line[0];
	static const char	*ids[] = {"sp", "pl", "cy", "pa", "bx", "L", "A", "C",
		NULL};
	static const char	alloc[] = {1, 1, 1, 1, 1, 1, 0, 0};
	int					i;
	static void (*const	f[])(t_parse *) = {
		parse_sphere, parse_plane, parse_cylinder,
		parse_para, parse_box, parse_point_light,
		parse_amb_light, parse_camera};

	i = 0;
	while (ids[i] && ft_strcmp(id, ids[i]))
		i++;
	if (!ids[i])
		fatal_if(m, true, "Not a valid type: %s\n", id);
	if (alloc[i])
		m->obj = object_new(m);
	else
		m->obj = NULL;
	f[i](m);
}
