#include "minirt.h"

void	parse_sphere(t_parse *m)
{
	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 4 || m->arrlen > 7, "Invalid sphere format\n");
	m->obj->type = OBJECT_SPHERE;
	m->obj->texture = parse_texture(m->arrlen > 4, m, 4);
	m->obj->rough = parse_float(m->arrlen > 5, m, DEFAULT_ROUGH, 5);
	fatal_if(m, m->obj->rough < 0.0f || m->obj->rough > 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->rough = fmaxf(MIN_ROUGH, m->obj->rough);
	m->obj->metallic = parse_float(m->arrlen > 6, m, DEFAULT_METALLIC, 6);
	fatal_if(m, m->obj->metallic < 0.0f || m->obj->metallic > 1.0f,
		"Metallic must be in [0,1]\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[3], &m->obj->color, (float []){0, 255});
	m->obj->radius = ft_atof(m->line[2]) * 0.5;
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	m->obj->rot = quat_from_axis_angle(vec3(0.0f, -1.0f, 0.0f), 0.0f);
	fatal_if(m, m->obj->radius <= 0.0f, "Sphere radius must be positive\n");
}

void	parse_plane(t_parse *m)
{
	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 4 || m->arrlen > 7, "Invalid plane format\n");
	m->obj->type = OBJECT_PLANE;
	m->obj->texture = parse_texture(m->arrlen > 4, m, 4);
	m->obj->rough = parse_float(m->arrlen > 5, m, DEFAULT_ROUGH, 5);
	fatal_if(m, m->obj->rough < 0.0f || m->obj->rough > 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->rough = fmaxf(MIN_ROUGH, m->obj->rough);
	m->obj->metallic = parse_float(m->arrlen > 6, m, DEFAULT_METALLIC, 6);
	fatal_if(m, m->obj->metallic < 0.0f || m->obj->metallic > 1.0f,
		"Metallic must be in [0,1]\n");
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
	fatal_if(m, m->arrlen < 6 || m->arrlen > 9, "Invalid para format\n");
	m->obj->type = OBJECT_PARA;
	m->obj->texture = parse_texture(m->arrlen > 6, m, 6);
	m->obj->rough = parse_float(m->arrlen > 7, m, DEFAULT_ROUGH, 7);
	fatal_if(m, m->obj->rough < 0.0f || m->obj->rough > 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->rough = fmaxf(MIN_ROUGH, m->obj->rough);
	m->obj->metallic = parse_float(m->arrlen > 8, m, DEFAULT_METALLIC, 8);
	fatal_if(m, m->obj->metallic < 0.0f || m->obj->metallic > 1.0f,
		"Metallic must be in [0,1]\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	parse3(m, m->line[5], &m->obj->color, (float []){0, 255});
	m->obj->radius = ft_atof(m->line[3]) * 0.5;
	m->obj->height = ft_atof(m->line[4]);
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	fatal_if(m, fabsf(len3(m->normal) - 1.0f) >= 0.001f,
		"Paraboloid axis must be a unit vector\n");
	fatal_if(m, m->obj->radius <= 0.0f || m->obj->height <= 0.0f,
		"Para radius and height must be positive\n");
	m->obj->rot = quat_from_direction(m->normal);
}

void	parse_cylinder(t_parse *m)
{
	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 6 || m->arrlen > 9, "Invalid Cy format\n");
	m->obj->type = OBJECT_CYLINDER;
	m->obj->texture = parse_texture(m->arrlen > 6, m, 6);
	m->obj->rough = parse_float(m->arrlen > 7, m, DEFAULT_ROUGH, 7);
	fatal_if(m, m->obj->rough < 0.0f || m->obj->rough > 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->rough = fmaxf(MIN_ROUGH, m->obj->rough);
	m->obj->metallic = parse_float(m->arrlen > 8, m, DEFAULT_METALLIC, 8);
	fatal_if(m, m->obj->metallic < 0.0f || m->obj->metallic > 1.0f,
		"Metallic must be in [0,1]\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	parse3(m, m->line[5], &m->obj->color, (float []){0, 255});
	m->obj->radius = ft_atof(m->line[3]) * 0.5;
	m->obj->height = ft_atof(m->line[4]);
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	fatal_if(m, fabsf(len3(m->normal) - 1.0f) >= 0.001f,
		"Cylinder axis must be a unit vector\n");
	fatal_if(m, m->obj->radius <= 0.0f && m->obj->height <= 0.0f,
		"Cylinder radius and height must be positive\n");
	m->obj->rot = quat_from_direction(m->normal);
}

void	parse_box(t_parse *m)
{
	m->arrlen = array_len(m->line);
	fatal_if(m, m->arrlen < 5 || m->arrlen > 8, "Invalid box format\n");
	m->obj->type = OBJECT_BOX;
	m->obj->texture = parse_texture(m->arrlen > 5, m, 5);
	m->obj->rough = parse_float(m->arrlen > 6, m, DEFAULT_ROUGH, 6);
	fatal_if(m, m->obj->rough < 0.0f || m->obj->rough > 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->rough = fmaxf(MIN_ROUGH, m->obj->rough);
	m->obj->metallic = parse_float(m->arrlen > 7, m, DEFAULT_METALLIC, 7);
	fatal_if(m, m->obj->metallic < 0.0f || m->obj->metallic > 1.0f,
		"Metallic must be in [0,1]\n");
	parse3(m, m->line[1], &m->obj->pos, (float []){0, 0});
	parse3(m, m->line[2], &m->normal, (float []){-1, 1});
	parse3(m, m->line[3], &m->obj->size, (float []){0, 0});
	parse3(m, m->line[4], &m->obj->color, (float []){0, 255});
	m->obj->color = scale3(m->obj->color, 1.0 / 255.0);
	fatal_if(m, fabsf(len3(m->normal) - 1.0f) >= 0.001f,
		"Box axis must be a unit vector\n");
	fatal_if(m, m->obj->size.x <= 0.0f || m->obj->size.y <= 0.0f
		|| m->obj->size.z <= 0.0f, "Box dimensions must be positive\n");
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
		fatal_if(m, false, "Not a valid type: %s\n", id);
	if (alloc[i])
		m->obj = object_new(m);
	else
		m->obj = NULL;
	f[i](m);
}
