#include "minirt.h"

void	debug_print_scene(t_scene *sc)
{
	size_t		i;
	t_object	*obj;

	printf("Scene Debug Info:\n");
	printf("Object count: %zu\n", sc->object_count);
	printf("Ambient light: (%f, %f, %f)\n", sc->ambient.x, sc->ambient.y,
		sc->ambient.z);
	printf("Camera position: (%f, %f, %f)\n", sc->pos.x,
		sc->pos.y, sc->pos.z);
	printf("Camera direction: (%f, %f, %f)\n", sc->dir.x,
		sc->dir.y, sc->dir.z);
	printf("Camera FOV: %f\n", sc->fov);
	printf("\nObjects:\n");
	for (i = 0; i < sc->object_count; i++)
	{
		obj = &sc->objects[i];
		printf("Object %zu: ", i + 1);
		switch (obj->type)
		{
		case OBJECT_LIGHT:
			printf("LIGHT at (%f, %f, %f), color: (%f, %f, %f)\n", obj->pos.x,
				obj->pos.y, obj->pos.z, obj->color.x, obj->color.y,
				obj->color.z);
			break ;
		case OBJECT_SPHERE:
			printf("SPHERE at (%f, %f, %f), radius: %f, color: (%f, %f, %f)\n",
				obj->pos.x, obj->pos.y, obj->pos.z, obj->radius, obj->color.x,
				obj->color.y, obj->color.z);
			break ;
		case OBJECT_PLANE:
			printf("PLANE at (%f, %f, %f), normal: (%f, %f, %f), color: (%f, %f, %f)\n", obj->pos.x, obj->pos.y, obj->pos.z, obj->normal.x,
				obj->normal.y, obj->normal.z, obj->color.x, obj->color.y,
				obj->color.z);
			break ;
		case OBJECT_CYLINDER:
			printf("CYLINDER at (%f, %f, %f), dir: (%f, %f, %f), radius: %f, height: %f, color: (%f, %f, %f)\n", obj->pos.x, obj->pos.y,
				obj->pos.z, obj->normal.x, obj->normal.y, obj->normal.z,
				obj->radius, obj->height, obj->color.x, obj->color.y,
				obj->color.z);
			break ;
		case OBJECT_CONE:
			printf("CONE at (%f, %f, %f), dir: (%f, %f, %f), radius: %f, height: %f, angle: %f, color: (%f, %f, %f)\n", obj->pos.x, obj->pos.y,
				obj->pos.z, obj->normal.x, obj->normal.y, obj->normal.z,
				obj->radius, obj->height, obj->angle, obj->color.x,
				obj->color.y, obj->color.z);
			break ;
		default:
			printf("UNKNOWN TYPE\n");
		}
	}
	printf("\n");
}
