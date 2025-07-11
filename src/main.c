#include "minirt.h"

int	main(int ac, char **av)
{
	const t_scene	*scene = validate_input_and_parse_map(ac, av);

	if (!scene)
		exit(EXIT_FAILURE);
}
