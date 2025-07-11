#ifndef MINIRT_H
# define MINIRT_H

# include "../assets/MLX42/include/MLX42/MLX42.h"
# include "../assets/libft/libft.h"
# include <fcntl.h>
# include <math.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

typedef union u_vec3		t_vec3;
typedef struct s_scene		t_scene;
typedef struct s_object		t_object;
typedef enum e_object_type	t_object_type;

enum						e_object_type
{
	OBJECT_PLANE,
	OBJECT_SPHERE,
	OBJECT_CYLINDER,
	OBJECT_CONE,
	OBJECT_LIGHT,
};

union						u_vec3
{
	struct // XYZ coordinate vector
	{
		float				x;
		float				y;
		float				z;
	};
	struct // RGB color vector
	{
		float				r;
		float				g;
		float				b;
	};
	float	a[3]; // Array access to components.
};

struct						s_object
{
	t_object_type	type; // Object type (one of OBJECT_xxx)
	t_vec3			pos;
	t_vec3			color; // Color of the object
	float			radius; // Radius (sphere/cylinder/cone)
	float			height; // Height (cylinder/cone)
	t_vec3			normal; // Normal/axis (plane/cylinder/cone)
};

struct						s_scene
{
	t_object	*objects; // Array of objects in the scene
	size_t		object_count; // Length of the objects array
	t_vec3		camera_pos; // Camera position
	t_vec3		camera_dir; // Camera direction (normalized)
	float		camera_fov; // Camera field of view (in degrees)
	t_vec3		ambient; // Ambient color (multiplied by ratio)
};

/* parsing of the map and validating the input input.c */
t_scene	*validate_input_and_parse_map(int ac, char **av);

#endif
