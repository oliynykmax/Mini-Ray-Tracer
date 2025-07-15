#include "minirt.h"

void	cleanup_scene(t_scene *sc)
{
	if (!sc)
		return ;
	if (sc->objects)
	{
		free(sc->objects);
		sc->objects = NULL;
	}
	sc->object_count = 0;
	sc->camera_fov = 0;
	sc->camera_pos = vec3(0, 0, 0);
	sc->camera_dir = vec3(0, 0, 0);
	sc->ambient = vec3(0, 0, 0);
}
