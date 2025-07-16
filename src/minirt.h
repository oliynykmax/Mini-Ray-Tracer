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

typedef struct s_render		t_render;
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
	float			angle; // Angle (cone)
};

struct s_scene
{
	t_object	*objects; // Array of objects in the scene
	size_t		object_count; // Length of the objects array
	t_vec3		camera_pos; // Camera position
	t_vec3		camera_dir; // Camera direction (normalized)
	float		camera_fov; // Camera field of view (in degrees)
	t_vec3		ambient; // Ambient color (multiplied by ratio)
};

struct s_render
{
	t_scene		*scene;			// The scene to render
	mlx_t		*mlx;			// MLX state
	mlx_image_t	*image;			// MLX image
	t_vec3		camera_x;		// Camera "right" vector
	t_vec3		camera_y;		// Camera "down" vector
	t_vec3		camera_z;		// Camera "out" vector
	t_vec3		viewport[4];	// The four corners of the viewport
};

// loop.c
void		render_scene(t_scene *scene);

// main.c
void		parse_scene(t_scene *scene, char *filename);

// title.c
void		show_stats_in_window_title(t_render *r);

// trace.c
uint32_t	trace_pixel(t_render *r, float x, float y);

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

// vec3_utility.c
t_vec3		vec3(float x, float y, float z);
t_vec3		vec3_lerp(t_vec3 a, t_vec3 b, float t);
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
