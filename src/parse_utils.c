#include "minirt.h"

t_texture	parse_texture(bool exists, t_parse *map, int i)
{
	if (!exists || map->line[i] == NULL || ft_strcmp(map->line[i], "_") == 0)
		return (TEXTURE_NONE);
	if (ft_strcmp(map->line[i], "checked") == 0)
		return (TEXTURE_CHECKED);
	if (ft_strcmp(map->line[i], "zigzag") == 0)
		return (TEXTURE_ZIGZAG);
	if (ft_strcmp(map->line[i], "polkadot") == 0)
		return (TEXTURE_POLKADOT);
	if (ft_strcmp(map->line[i], "marble") == 0)
		return (TEXTURE_MARBLE);
	fatal_if(map,
		true,
		"Unknown texture '%s' (expected checked | zigzag | polkadot | marble)\n",
		map->line[i]);
	return (TEXTURE_NONE);
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
	m->obj->texture = parse_texture(m->arrlen > i + 2, m, i + 2);
	m->obj->bump = parse_texture(m->arrlen > i + 3, m, i + 3);
}
