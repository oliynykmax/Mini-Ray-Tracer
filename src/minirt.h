#ifndef MINIRT_H
# define MINIRT_H

# include <math.h>

# include "../MLX42/include/MLX42/MLX42.h"

typedef enum e_object_type	t_object_type;
typedef struct s_object		t_object;
typedef struct s_render		t_render;
typedef struct s_scene		t_scene;
typedef union u_vec3		t_vec3;

enum e_object_type
{
	OBJECT_PLANE,
	OBJECT_SPHERE,
	OBJECT_CYLINDER,
	OBJECT_CONE,
	OBJECT_LIGHT,
};

union u_vec3
{
	struct // XYZ coordinate vector
	{
		float	x;
		float	y;
		float	z;
	};
	struct // RGB color vector
	{
		float	r;
		float	g;
		float	b;
	};
	float	a[3]; // Array access to components.
};

struct s_object
{
	t_object_type	type;	// Object type (one of OBJECT_xxx)
	t_vec3			pos;	// Position of the object
	t_vec3			color;	// Color of the object
	float			radius;	// Radius (sphere/cylinder/cone)
	float			height;	// Height (cylinder/cone)
	t_vec3			normal;	// Normal/axis (plane/cylinder/cone)
};

struct s_scene
{
	t_object	*objects;		// Array of objects in the scene
	size_t		object_count;	// Length of the objects array
	t_vec3		camera_pos;		// Camera position
	t_vec3		camera_dir;		// Camera direction (normalized)
	float		camera_fov;		// Camera field of view (in degrees)
	t_vec3		ambient;		// Ambient color (multiplied by ratio)
};

struct s_render
{
	t_scene		*scene;	// The scene to render
	mlx_t		*mlx;	// MLX state
	mlx_image_t	*image;	// MLX image
};

// loop.c
void	render_scene(t_scene *scene);

// main.c
void	parse_scene(t_scene *scene, char *filename);

// vec3_arithmetic.c
t_vec3	vec3(float x, float y, float z);
t_vec3	vec3_add(t_vec3 a, t_vec3 b);
t_vec3	vec3_sub(t_vec3 a, t_vec3 b);
t_vec3	vec3_mul(t_vec3 a, t_vec3 b);
t_vec3	vec3_div(t_vec3 a, t_vec3 b);

// vec3_geometric.c
t_vec3	vec3_scale(t_vec3 v, float s);
float	vec3_dot(t_vec3 a, t_vec3 b);
float	vec3_length(t_vec3 v);
t_vec3	vec3_normalize(t_vec3 v);
t_vec3	vec3_cross(t_vec3 a, t_vec3 b);

#endif
