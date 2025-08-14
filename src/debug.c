/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: YOUR_NAME <your_email@example.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 12:00:00 by YOUR_NAME         #+#    #+#             */
/*   Updated: 2025/08/14 12:00:00 by YOUR_NAME        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

static void	print_vec3(const char *label, t_vec3 v)
{
	printf("\t\t%s: (x: %.2f, y: %.2f, z: %.2f)\n", label, v.x, v.y, v.z);
}

static void	print_quat(const char *label, t_quat q)
{
	printf("\t\t%s: (x: %.2f, y: %.2f, z: %.2f, w: %.2f)\n", label, q.x, q.y, q.z, q.w);
}

static const char	*get_object_type_name(t_object_type type)
{
	switch (type)
	{
		case OBJECT_PLANE:
			return ("PLANE");
		case OBJECT_SPHERE:
			return ("SPHERE");
		case OBJECT_CYLINDER:
			return ("CYLINDER");
		case OBJECT_PARA:
			return ("PARABOLOID");
		case OBJECT_BOX:
			return ("BOX");
		case OBJECT_LIGHT:
			return ("LIGHT");
		default:
			return ("UNKNOWN");
	}
}

void	debug_print_scene(t_scene *scene)
{
	size_t	i;

	printf("--- DEBUG SCENE ---\n");
	printf("Camera:\n");
	print_vec3("Position", scene->pos);
	print_vec3("Direction", scene->dir);
	printf("\t\tFOV: %.2f\n", scene->fov);
	printf("\t\tFocus Depth: %.2f\n", scene->focus_depth);
	printf("\t\tAperture Size: %.2f\n", scene->aperture_size);
	print_vec3("Ambient", scene->ambient);
	printf("\nObjects (%zu):\n", scene->object_count);
	i = 0;
	while (i < scene->object_count)
	{
		t_object	*obj;

		obj = &scene->objects[i];
		printf("\tObject %zu: %s\n", i + 1, get_object_type_name(obj->type));
		print_vec3("Position", obj->pos);
		print_quat("Rotation", obj->rot);
		print_vec3("Color", obj->color);
		if (obj->type == OBJECT_SPHERE || obj->type == OBJECT_CYLINDER
			|| obj->type == OBJECT_PARA || obj->type == OBJECT_LIGHT)
			printf("\t\tRadius: %.2f\n", obj->radius);
		if (obj->type == OBJECT_CYLINDER || obj->type == OBJECT_PARA)
			printf("\t\tHeight: %.2f\n", obj->height);
		if (obj->type == OBJECT_BOX)
			print_vec3("Size", obj->size);
		printf("\t\tRoughness: %.2f\n", obj->rough);
		printf("\t\tMetallic: %.2f\n", obj->metallic);
		i++;
	}
	printf("--- END DEBUG SCENE ---\n");
}
