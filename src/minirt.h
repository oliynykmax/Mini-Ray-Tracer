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
# include <errno.h>

# define OBJECT_MAX 32

typedef enum e_object_type	t_object_type;
typedef struct s_object		t_object;
typedef struct s_ray		t_ray;
typedef struct s_render		t_render;
typedef struct s_scene		t_scene;
typedef union u_vec3		t_vec3;

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
	float			angle; // Angle (cone)
};

struct s_scene
{
	t_object	*objects;		// Array of objects in the scene
	size_t		object_count;	// Length of the objects array
	t_vec3		pos;			// Camera position
	t_vec3		dir;			// Camera direction (normalized)
	float		fov;			// Camera field of view (in degrees)
	t_vec3		ambient;		// Ambient color (multiplied by ratio)
};

struct s_render
{
	t_scene		*scene;			// The scene to render
	mlx_t		*mlx;			// MLX state
	mlx_image_t	*image;			// MLX image
	float		camera_yaw;		// Camera yaw angle (in radians)
	float		camera_pitch;	// Camera pitch angle (in radians)
	t_vec3		camera_x;		// Camera "right" vector
	t_vec3		camera_y;		// Camera "down" vector
	t_vec3		camera_z;		// Camera "forward" vector
	t_vec3		viewport[4];	// The four corners of the viewport
	int			key_forward;
	int			key_back;
	int			key_left;
	int			key_right;
	int			key_up;
	int			key_down;
};

struct s_ray
{
	t_vec3		ro;		// Ray origin
	t_vec3		rd;		// Ray direction (normalized)
	float		depth;	// Distance to closest point
	t_vec3		point;	// Closest point of intersection
	t_vec3		color;	// Color at that point
	t_vec3		normal;	// Surface normal at that point
};

// camera.c
void		camera_update(t_render *r);

// loop.c
void		render_scene(t_scene *scene);

// main.c
void		parse_scene(t_scene *scene, char *filename);

// math.c
float		clamp(float value, float lower, float upper);
float		saturate(float value);
float		radians(float degrees);

// title.c
void		show_stats_in_window_title(t_render *r);

// trace.c
t_vec3		trace_pixel(t_render *r, float x, float y);

// vec3_arithmetic.c
t_vec3		vec3_add(t_vec3 a, t_vec3 b);
t_vec3		vec3_sub(t_vec3 a, t_vec3 b);
t_vec3		vec3_mul(t_vec3 a, t_vec3 b);
t_vec3		vec3_div(t_vec3 a, t_vec3 b);
t_vec3		vec3_scale(t_vec3 v, float s);

// vec3_geometric.c
float		vec3_dot(t_vec3 a, t_vec3 b);
float		vec3_length(t_vec3 v);
t_vec3		vec3_normalize(t_vec3 v);
t_vec3		vec3_cross(t_vec3 a, t_vec3 b);
t_vec3		vec3_reflect(t_vec3 i, t_vec3 n);

// vec3_utility.c
t_vec3		vec3(float x, float y, float z);
t_vec3		vec3_lerp(t_vec3 a, t_vec3 b, float t);
t_vec3		vec3_to_srgb(t_vec3 color);
uint32_t	vec3_to_color(t_vec3 color);

/* parsing of the map and validating the input input.c */
bool		validate_input_and_parse_map(int ac, char **av, t_scene *scene);
bool		parse_sphere(char **line, t_scene *sc);
bool		parse_plane(char **line, t_scene *sc);
bool		parse_cone(char **line, t_scene *sc);
bool		parse_cylinder(char **line, t_scene *sc);
bool		parse_amb_light(char **line, t_scene *sc);
bool		parse_point_light(char **line, t_scene *sc);
bool		parse_object(char **line, t_scene *sc);
bool		parse_camera(char **line, t_scene *sc);
int			objects_malloc_manager(t_scene *sc);
/* debug printing functions */
void		debug_print_scene(t_scene *sc);
/* asser	ting function, worth thinking if we want to builtin
 * the e	xiting and cleaning into it*/
bool		mrt_assert(bool condition, char *format, ...);
void		cleanup_scene(t_scene *sc);
/* array	 utils */
int			array_len(char **array);
void		free_array(char **array);
/* vecto	r parsing utils */
double		ft_atof(const char *str);
bool		vec3_in_range(t_vec3 v, float lower, float upper);
bool		parse_vec3(char *str, t_vec3 *out, float min, float max);
bool		validate_input_and_parse_map(int ac, char **av, t_scene *scene);

#endif
