#include "minirt.h"

void	parse_sphere(t_parse *m)
{
	m->arrlen = array_len(m->line);
	mrt_assert(m, m->arrlen >= 4 && m->arrlen <= 7, "Invalid sphere format\n");
	objects_malloc_manager(m);
	m->obj = &m->sc->objects[m->sc->object_count];
	m->obj->type = OBJECT_SPHERE;
	m->obj->texture = parse_texture(m->arrlen > 4, m, 4);
	m->obj->rough = parse_float(m->arrlen > 5, m, DEFAULT_ROUGH, 5);
	mrt_assert(m, m->obj->rough >= 0.0f && m->obj->rough <= 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->metallic = parse_float(m->arrlen > 6, m, DEFAULT_METALLIC, 6);
	mrt_assert(m, m->obj->metallic >= 0.0f && m->obj->metallic <= 1.0f,
		"Metallic must be in [0,1]\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[3], &m->obj->color, (float []){0, 255});
	m->obj->radius = ft_atof(m->line[2]) * 0.5;
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	m->obj->rot = quat_from_axis_angle(vec3(0.0f, -1.0f, 0.0f), 0.0f);
	mrt_assert(m, m->obj->radius > 0.0f, "Sphere radius must be positive\n");
}

void	parse_plane(t_parse *m)
{
	m->arrlen = array_len(m->line);
	mrt_assert(m, m->arrlen >= 4 && m->arrlen <= 7, "Invalid plane format\n");
	objects_malloc_manager(m);
	m->obj = &m->sc->objects[m->sc->object_count];
	m->obj->type = OBJECT_PLANE;
	m->obj->texture = parse_texture(m->arrlen > 4, m, 4);
	m->obj->rough = parse_float(m->arrlen > 5, m, DEFAULT_ROUGH, 5);
	mrt_assert(m, m->obj->rough >= 0.0f && m->obj->rough <= 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->metallic = parse_float(m->arrlen > 6, m, DEFAULT_METALLIC, 6);
	mrt_assert(m, m->obj->metallic >= 0.0f && m->obj->metallic <= 1.0f,
		"Metallic must be in [0,1]\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	parse3(m, m->line[3], &m->obj->color, (float []){0, 255});
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	mrt_assert(m, fabsf(len3(m->normal) - 1.0f) < 0.001f,
		"Plane normal must be a unit vector\n");
	m->obj->rot = quat_from_direction(m->normal);
}

void	parse_para(t_parse *m)
{
	m->arrlen = array_len(m->line);
	mrt_assert(m, m->arrlen >= 6 && m->arrlen <= 9, "Invalid para format\n");
	objects_malloc_manager(m);
	m->obj = &m->sc->objects[m->sc->object_count];
	m->obj->type = OBJECT_PARA;
	m->obj->texture = parse_texture(m->arrlen > 6, m, 6);
	m->obj->rough = parse_float(m->arrlen > 7, m, DEFAULT_ROUGH, 7);
	mrt_assert(m, m->obj->rough >= 0.0f && m->obj->rough <= 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->metallic = parse_float(m->arrlen > 8, m, DEFAULT_METALLIC, 8);
	mrt_assert(m, m->obj->metallic >= 0.0f && m->obj->metallic <= 1.0f,
		"Metallic must be in [0,1]\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	parse3(m, m->line[5], &m->obj->color, (float []){0, 255});
	m->obj->radius = ft_atof(m->line[3]) * 0.5;
	m->obj->height = ft_atof(m->line[4]);
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	mrt_assert(m, fabsf(len3(m->normal) - 1.0f) < 0.001f,
		"Paraboloid axis must be a unit vector\n");
	mrt_assert(m, m->obj->radius > 0.0f && m->obj->height > 0.0f,
		"Para radius and height must be positive\n");
	m->obj->rot = quat_from_direction(m->normal);
}

void	parse_cylinder(t_parse *m)
{
	m->arrlen = array_len(m->line);
	mrt_assert(m, m->arrlen >= 6 && m->arrlen <= 9, "Invalid Cy format\n");
	objects_malloc_manager(m);
	m->obj = &m->sc->objects[m->sc->object_count];
	m->obj->type = OBJECT_CYLINDER;
	m->obj->texture = parse_texture(m->arrlen > 6, m, 6);
	m->obj->rough = parse_float(m->arrlen > 7, m, DEFAULT_ROUGH, 7);
	mrt_assert(m, m->obj->rough >= 0.0f && m->obj->rough <= 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->metallic = parse_float(m->arrlen > 8, m, DEFAULT_METALLIC, 8);
	mrt_assert(m, m->obj->metallic >= 0.0f && m->obj->metallic <= 1.0f,
		"Metallic must be in [0,1]\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	parse3(m, m->line[5], &m->obj->color, (float []){0, 255});
	m->obj->radius = ft_atof(m->line[3]) * 0.5;
	m->obj->height = ft_atof(m->line[4]);
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	mrt_assert(m, fabsf(len3(m->normal) - 1.0f) < 0.001f,
		"Cylinder axis must be a unit vector\n");
	mrt_assert(m, m->obj->radius > 0.0f && m->obj->height > 0.0f,
		"Cylinder radius and height must be positive\n");
	m->obj->rot = quat_from_direction(m->normal);
}

void	parse_type(t_parse *m)
{
	if (ft_strcmp(m->line[0], "A") == 0)
		parse_amb_light(m);
	else if (ft_strcmp(m->line[0], "L") == 0)
		parse_point_light(m);
	else if (ft_strcmp(m->line[0], "C") == 0)
		parse_camera(m);
	else if (ft_strcmp(m->line[0], "sp") == 0)
		parse_sphere(m);
	else if (ft_strcmp(m->line[0], "pl") == 0)
		parse_plane(m);
	else if (ft_strcmp(m->line[0], "cy") == 0)
		parse_cylinder(m);
	else if (ft_strcmp(m->line[0], "pa") == 0)
		parse_para(m);
	else
		mrt_assert(m, false, "Not a valid type: %s\n", m->line[0]);
	m->sc->object_count++;
}
