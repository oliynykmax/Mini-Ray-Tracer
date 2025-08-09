#include "minirt.h"

t_texture	parse_texture(bool exists, t_parse *map, const char *token)
{
	if (!exists)
		return (TEXTURE_NONE);
	if (ft_strcmp(token, "checker") == 0)
		return (TEXTURE_CHECKED);
	if (ft_strcmp(token, "zigzag") == 0)
		return (TEXTURE_ZIGZAG);
	if (ft_strcmp(token, "polkadot") == 0)
		return (TEXTURE_POLKADOT);
	if (ft_strcmp(token, "bump") == 0)
		return (TEXTURE_BUMP);
	if (ft_strcmp(token, "_") == 0)
		return (TEXTURE_NONE);
	mrt_assert(map,
		false,
		"Unknown texture '%s' (expected checker | zigzag | polkadot | bump)\n",
		token);
	return (TEXTURE_NONE);
}

float	parse_float(bool exists, char *str, float std)
{
	if (!exists || str == NULL || ft_strcmp(str, "_") == 0)
		return (std);
	return (ft_atof(str));
}
