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
	mrt_assert(map,
		false,
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
