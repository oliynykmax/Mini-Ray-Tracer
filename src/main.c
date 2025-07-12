#include "minirt.h"

int	main(int ac, char **av)
{
	t_scene	scene;

	ft_bzero(&scene, sizeof(t_scene));
	if (!validate_input_and_parse_map(ac, av, &scene))
		return (1);
	return (0);
}
