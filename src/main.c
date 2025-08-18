#include "minirt.h"

static void	free_scene_objects(t_scene *scene)
{
	size_t	i;

	i = 0;
	while (i < scene->object_count)
	{
		free_object_textures(&scene->objects[i]);
		++i;
	}
	free(scene->objects);
	if (scene->amb_texture)
	{
		mlx_delete_texture(scene->amb_texture);
		scene->amb_texture = NULL;
	}
}

static void	init_structs(t_scene *scene, t_render *render, t_parse *map)
{
	*scene = (t_scene){0};
	*render = (t_render){0};
	*map = (t_parse){0};
	map->sc = scene;
	map->fd = -1;
	render->scene = scene;
}

int	main(int ac, char **av)
{
	t_scene		scene;
	t_render	render;
	t_parse		map;

	init_structs(&scene, &render, &map);
	validate_input_and_parse_map(ac, av, &map);
	render_scene(&render);
	free_scene_objects(&scene);
	return (0);
}
