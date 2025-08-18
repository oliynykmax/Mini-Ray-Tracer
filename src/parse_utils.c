#include "minirt.h"

mlx_texture_t	*parse_texture(bool exists, t_parse *map, int i)
{
	size_t			len;
	mlx_texture_t	*tex;

	if (!exists || ft_strcmp(map->line[i], "_") == 0)
		return (NULL);
	len = ft_strlen(map->line[i]);
	if (len >= 4 && ft_strcmp(map->line[i] + len - 4, ".png") == 0)
	{
		tex = load_png_texture(map, map->line[i]);
		return (tex);
	}
	fatal_if(map, true,
		"Unknown texture '%s' (expected <file>.png or _)\n",
		map->line[i]);
	return (NULL);
}

float	parse_float(bool exists, t_parse *map, float std, int i)
{
	if (!exists || map->line[i] == NULL || ft_strcmp(map->line[i], "_") == 0)
		return (std);
	return (ft_atof(map, map->line[i]));
}

void	parse_optionals(t_parse *m, int i)
{
	m->obj->rough = parse_float(m->arrlen > i, m, DEFAULT_ROUGH, i);
	fatal_if(m, m->obj->rough < 0.0f || m->obj->rough > 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->metallic = parse_float(m->arrlen > i + 1, m,
			DEFAULT_METALLIC, i + 1);
	fatal_if(m, m->obj->metallic < 0.0f || m->obj->metallic > 1.0f,
		"Metallic must be in [0,1]\n");
	m->obj->texture_img = parse_texture(m->arrlen > i + 2, m, i + 2);
	m->obj->bump_img = parse_texture(m->arrlen > i + 3, m, i + 3);
}

/*
 * Dispatcher moved here from separate file (parse_type.c) to consolidate
 * parsing helpers in a single compilation unit.
 */
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
