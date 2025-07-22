#include "minirt.h"

int	main(int ac, char **av)
{
	t_scene		scene;
	t_render	render;

	ft_bzero(&scene, sizeof(t_scene));
	ft_bzero(&render, sizeof(render));
	if (!validate_input_and_parse_map(ac, av, &scene))
	{
		cleanup_scene(&scene);
		return (1);
	}
	render_scene(&render, &scene);
	cleanup_scene(&scene);
	free(scene.objects);
	return (0);
}
