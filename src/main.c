#include "minirt.h"

// parse_scene should call exit() if anything goes wrong during parsing.
// Eventually main should also free any resources in the scene, but right now
// there aren't any. FIXME: The argc should be fixed when we have files to test.

int	main(int ac, char **av)
{
	t_scene	scene;

	ft_bzero(&scene, sizeof(t_scene));
	if (!validate_input_and_parse_map(ac, av, &scene))
	{
		cleanup_scene(&scene);
		return (1);
	}
	render_scene(&scene);
	cleanup_scene(&scene);
	free(scene.objects);
	return (0);
}
