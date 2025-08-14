#include "minirt.h"

void	init_structs(t_scene *scene, t_render *render, t_parse *map)
{
	ft_bzero(scene, sizeof(t_scene));
	ft_bzero(render, sizeof(t_render));
	ft_bzero(map, sizeof(t_parse));
	map->sc = scene;
	map->fd = -1;
	render->scene = scene;
	scene->objects = NULL;
}

int	main(int ac, char **av)
{
	t_scene		scene;
	t_render	render;
	t_parse		map;

	init_structs(&scene, &render, &map);
	validate_input_and_parse_map(ac, av, &map);
	render_scene(&render);
	free(scene.objects);
	return (0);
}
