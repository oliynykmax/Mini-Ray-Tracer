#include "minirt.h"

// For now this just sets up a test scene. This will be replaced by a proper .rt
// file parser at some stage.

void	parse_scene(t_scene *scene, char *filename)
{
	const t_object	objects[] = {{
		.type = OBJECT_SPHERE,
		.pos = vec3(+1.0f, 0.0f, 0.0f),
		.color = vec3(1.0f, 0.0f, 0.0),
		.radius = 1.0f,
	}, {
		.type = OBJECT_SPHERE,
		.pos = vec3(-1.0f, 0.0f, 0.0f),
		.color = vec3(0.0f, 1.0f, 1.0),
		.radius = 1.0f,
	}};

	(void) filename;
	scene->objects = (t_object *) objects;
	scene->object_count = sizeof(objects) / sizeof(*objects);
	scene->camera_pos = vec3(0.0f, 0.0f, +1.0f);
	scene->camera_dir = vec3(0.0f, 0.0f, -1.0f);
	scene->camera_fov = 90.0f;
	scene->ambient = vec3(0.0f, 0.1f, 0.2f);
}

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
	cleanup_scene(&scene);
	parse_scene(&scene, av[1]);
	render_scene(&scene);
	return (0);
}
