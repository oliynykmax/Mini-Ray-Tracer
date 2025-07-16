#include "minirt.h"

// For now this just sets up a test scene. This will be replaced by a proper .rt
// file parser at some stage.

void	parse_scene(t_scene *scene, char *filename)
{
	(void) filename;
	scene->objects = malloc(sizeof(t_object[2]));
	scene->object_count = 2;
	scene->objects[0] = (t_object){
		.type = OBJECT_SPHERE,
		.pos = vec3(+1.0f, 0.0f, 0.0f),
		.color = vec3(1.0f, 0.0f, 0.0),
		.radius = 1.0f,
	};
	scene->objects[1] = (t_object){
		.type = OBJECT_SPHERE,
		.pos = vec3(-1.0f, 0.0f, 0.0f),
		.color = vec3(0.0f, 1.0f, 0.0),
		.radius = 1.0f,
	};
	scene->camera_pos = vec3(3.0f, 0.0f, +3.0f);
	scene->camera_dir = vec3_normalize(vec3(-1.0f, 0.0f, -1.0f));
	scene->camera_fov = 90.0f;
	scene->ambient = vec3(0.0f, 0.3f, 0.5f);
}

// parse_scene should call exit() if anything goes wrong during parsing.
// Eventually main should also free any resources in the scene, but right now
// there aren't any. FIXME: The argc should be fixed when we have files to test.

int	main(int ac, char **av)
{
	t_scene			scene;

	ft_bzero(&scene, sizeof(t_scene));
	if (!validate_input_and_parse_map(ac, av, &scene))
		return (1);
	parse_scene(&scene, av[1]);
	render_scene(&scene);
	free(scene.objects);
	return (0);
}
