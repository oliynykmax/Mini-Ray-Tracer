#include "minirt.h"

t_texture	parse_texture(bool exists, t_parse *map, int i)
{
	if (!exists || map->line[i] == NULL || ft_strcmp(map->line[i], "_") == 0)
		return (TEXTURE_NONE);
	if (ft_strcmp(map->line[i], "checker") == 0)
		return (TEXTURE_CHECKED);
	if (ft_strcmp(map->line[i], "zigzag") == 0)
		return (TEXTURE_ZIGZAG);
	if (ft_strcmp(map->line[i], "polkadot") == 0)
		return (TEXTURE_POLKADOT);
	if (ft_strcmp(map->line[i], "bump") == 0)
		return (TEXTURE_BUMP);
	fatal_if(map,
		true,
		"Unknown texture '%s' (expected checker | zigzag | polkadot | bump)\n",
		map->line[i]);
	return (TEXTURE_NONE);
}

float	parse_float(bool exists, t_parse *map, float std, int i)
{
	if (!exists || map->line[i] == NULL || ft_strcmp(map->line[i], "_") == 0)
		return (std);
	return (ft_atof(map->line[i]));
}

void	parse_optionals(t_parse *m, int t_index)
{
	const int	rough_index = t_index + 1;
	const int	metal_index = t_index + 2;

	m->obj->texture = parse_texture(m->arrlen > t_index, m, t_index);
	m->obj->rough = parse_float(m->arrlen > rough_index, m, DEFAULT_ROUGH,
			rough_index);
	fatal_if(m, m->obj->rough < 0.0f || m->obj->rough > 1.0f,
		"Rough must be in [0,1]\n");
	m->obj->metallic = parse_float(m->arrlen > metal_index, m,
			DEFAULT_METALLIC, metal_index);
	fatal_if(m, m->obj->metallic < 0.0f || m->obj->metallic > 1.0f,
		"Metallic must be in [0,1]\n");
}
