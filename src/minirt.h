#ifndef MINIRT_H
# define MINIRT_H

# include <errno.h>
# include <fcntl.h>
# include <math.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

# include "../assets/MLX42/include/MLX42/MLX42.h"
# include "../assets/libft/libft.h"

// The number of rendering threads to use.
# define THREAD_COUNT 4

// Mouse sensitivity (higher values = more sensitive).
# define MOUSE_SENSITIVITY 0.006

// Key bindings.
# define KEY_FORWARD	MLX_KEY_W			// Walk forward
# define KEY_LEFT		MLX_KEY_A			// Strafe left
# define KEY_BACK		MLX_KEY_S			// Walk backward
# define KEY_RIGHT		MLX_KEY_D			// Strafe right
# define KEY_UP			MLX_KEY_SPACE		// Float up
# define KEY_DOWN		MLX_KEY_LEFT_SHIFT	// Sink down

# define TAU 6.283185307179586 // 2π

// Typedefs for enum/structure/union types.
typedef enum e_object_type	t_object_type;
typedef enum e_texture		t_texture;
typedef struct s_keys		t_keys;
typedef struct s_object		t_object;
typedef struct s_pbr		t_pbr;
typedef struct s_ray		t_ray;
typedef struct s_render		t_render;
typedef struct s_scene		t_scene;
typedef struct s_thread		t_thread;
typedef union u_quat		t_quat;
typedef union u_vec3		t_vec3;

// Typedefs for function types.
typedef float				(*t_distance_function)(t_object*, t_vec3, t_vec3);
typedef t_vec3				(*t_normal_function)(t_object*, t_vec3);
typedef t_vec3				(*t_texcoord_function)(t_object*, t_vec3);
typedef float				(*t_texture_function)(float, float);

// 3D coordinate vector type (also used for colors).
union	u_vec3
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
};

// Quaternion type (used for rotating/orienting objects).
union	u_quat
{
	struct
	{
		float	x;		// Imaginary
		float	y;		// Imaginary
		float	z;		// Imaginary
		float	w;		// Real
	};
	t_vec3		xyz;	// Imaginary parts as vector
};

// Enumeration for different scene object types.
enum	e_object_type
{
	OBJECT_PLANE,
	OBJECT_SPHERE,
	OBJECT_CYLINDER,
	OBJECT_PARA,
	OBJECT_LIGHT,
};

// Enumeration type for different procedural textures.
enum	e_texture
{
	TEXTURE_NONE,
	TEXTURE_CHECKED,
	TEXTURE_ZIGZAG,
	TEXTURE_POLKADOT,
};

// Data describing one geometric object or light in the scene.
struct s_object
{
	t_object_type	type;	// Object type (one of OBJECT_xxx)
	t_vec3			pos;	// Object position in world coordinates
	t_quat			rot;	// Object rotation relative to world coordinates
	t_vec3			color;	// Surface color
	float			radius;	// Radius (sphere/cylinder/para/light)
	float			height;	// Height (cylinder/para)
};

struct s_ray
{
	t_scene		*scene;	// Scene data (read only!)
	uint16_t	rng;	// Index used for random number generation
	t_vec3		ro;		// Ray origin
	t_vec3		rd;		// Ray direction
	int			bounce;	// Remaining ray bounces
};

// Data describing the objects in the scene.
struct s_scene
{
	t_object	*objects;		// Array of objects in the scene
	size_t		object_count;	// Length of the objects array
	t_vec3		pos;			// Camera position
	t_vec3		dir;			// Camera direction (normalized)
	float		fov;			// Camera field of view (in degrees)
	float		focus_depth;	// Camera focus depth
	float		aperture_size;	// Camera aperture size
	t_vec3		ambient;		// Ambient color (multiplied by ratio)
};

// Data for the current keyboard state.
struct s_keys
{
	int	forward;	// State of the key for walking forward
	int	back;		// State of the key for walking backward
	int	left;		// State of the key for strafing left
	int	right;		// State of the key for strafing right
	int	up;			// State of the key for floating up
	int	down;		// State of the key for sinking down
};

// Common renderer state.
struct s_render
{
	t_scene			*scene;					// The scene to render
	mlx_t			*mlx;					// MLX state
	mlx_image_t		*image;					// MLX image
	float			camera_yaw;				// Camera yaw angle (in radians)
	float			camera_pitch;			// Camera pitch angle (in radians)
	t_vec3			camera_x;				// Camera "right" vector
	t_vec3			camera_y;				// Camera "down" vector
	t_vec3			camera_z;				// Camera "forward" vector
	t_vec3			viewport[4];			// The four corners of the viewport
	t_vec3			*frame;					// Floating point frame buffer
	size_t			frame_size;				// Allocated size of the frame
	int				frame_samples;			// Number of accumulated samples
	t_vec3			jitter;					// Subpixel jitter
	pthread_t		threads[THREAD_COUNT];	// Array of rendering threads
	int				threads_started;		// How many threads were initialized
	bool			threads_stop;			// Set to stop the render threads
	size_t			jobs_available;			// Available render jobs
	size_t			jobs_finished;			// Finished render jobs
	pthread_cond_t	available_cond;			// Tells when jobs become available
	pthread_cond_t	finished_cond;			// Tells when all jobs are finished
	pthread_mutex_t	mutex;					// Protects common render state
};

// Data for one render thread.
struct s_thread
{
	size_t	id;		// Index from 0 to THREAD_COUNT - 1
	size_t	job;	// Job index (always increasing)
	size_t	y_min;	// y-coordinate of top scanline of region
	size_t	y_max;	// y-coordinate of bottom scanline of region
};

// Structure holding PBR lighting parameters during shading.

struct s_pbr
{
	t_vec3	point;		// Surface point that's being shaded
	t_vec3	normal;		// Surface normal at shading point
	t_vec3	f0;			// Surface reflection at zero incidence (for fresnel)
	t_vec3	albedo;		// Surface albedo at shaded point
	float	metallic;	// PBR metallic parameter
	float	rough;		// PBR roughness parameter
	t_vec3	view_dir;	// View direction (points toward camera)
	t_vec3	light;		// Light vector (from light to shading point)
	t_vec3	light_dir;	// Light direction (normalized light vector)
	t_vec3	halfway;	// Halfway vector (between normal and light direction)
	float	ndotv;		// (surface normal) · (view vector)
	float	ndotl;		// (surface normal) · (light vector)
	float	ndoth;		// (surface normal) · (halfway vector)
	float	hdotv;		// (halfway vector) · (view vector)
	t_vec3	diffuse;	// Diffuse contribution
	t_vec3	specular;	// Specular contribution
};

// camera.c
void		camera_update(t_render *r);
// para.c
float		para_distance(t_object *o, t_vec3 ro, t_vec3 rd);
t_vec3		para_normal(t_object *o, t_vec3 p);
t_vec3		para_texcoord(t_object *o, t_vec3 p);
// cylinder.c
float		cylinder_distance(t_object *o, t_vec3 ro, t_vec3 rd);
t_vec3		cylinder_normal(t_object *o, t_vec3 p);
t_vec3		cylinder_texcoord(t_object *o, t_vec3 p);

// lighting.c
t_vec3		apply_lighting(t_ray *r, t_object *object, t_vec3 p);

// loop.c
void		render_scene(t_render *r, t_scene *scene);

// main.c
void		parse_scene(t_scene *scene, char *filename);

// math.c
float		clamp(float value, float lower, float upper);
float		saturate(float value);
float		radians(float degrees);
float		fract(float x);
float		solve_quadratic(float a, float b, float c);

// object.c
float		object_distance(t_object *object, t_vec3 ro, t_vec3 rd);
t_vec3		object_normal(t_object *obj, t_vec3 point);
t_vec3		object_texcoord(t_object *obj, t_vec3 point);

// plane.c
float		plane_distance(t_object *o, t_vec3 ro, t_vec3 rd);
t_vec3		plane_normal(t_object *o, t_vec3 p);
t_vec3		plane_texcoord(t_object *o, t_vec3 p);

// quaternion.c
t_quat		quat_from_axis_angle(t_vec3 axis, float angle);
t_quat		quat_from_direction(t_vec3 direction);
t_quat		quat_multiply(t_quat a, t_quat b);
t_quat		quat_inverse(t_quat q);
t_vec3		quat_rotate_vec3(t_quat q, t_vec3 v);

// random.c
t_vec3		random_point_in_square(uint16_t rng);
t_vec3		random_point_in_disk(uint16_t rng, float radius);
t_vec3		random_point_on_sphere(uint16_t rng, float radius);

// sphere.c
float		sphere_distance(t_object *s, t_vec3 ro, t_vec3 rd);
t_vec3		sphere_normal(t_object *s, t_vec3 p);
t_vec3		sphere_texcoord(t_object *s, t_vec3 d);

// texturing.c
float		get_texture(t_texture texture, float u, float v);

// threads.c
bool		threads_init(t_render *r);
void		threads_quit(t_render *r);

// title.c
void		show_stats_in_window_title(t_render *r);

// trace.c
float		scene_distance(t_scene *s, t_vec3 ro, t_vec3 rd, t_object **object);
t_vec3		get_viewport_ray(t_render *r, float x, float y, bool jitter);
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
t_vec3		vec3_tonemap(t_vec3 color);
t_vec3		vec3_to_srgb(t_vec3 color);
uint32_t	vec3_to_color(t_vec3 color);

/* parsing of the map and validating the input input.c */
bool		validate_input_and_parse_map(int ac, char **av, t_scene *scene);
bool		parse_sphere(char **line, t_scene *sc);
bool		parse_plane(char **line, t_scene *sc);
bool		parse_para(char **line, t_scene *sc);
bool		parse_cylinder(char **line, t_scene *sc);
bool		parse_amb_light(char **line, t_scene *sc);
bool		parse_point_light(char **line, t_scene *sc);
bool		parse_object(char **line, t_scene *sc);
bool		parse_camera(char **line, t_scene *sc);
int			objects_malloc_manager(t_scene *sc);
/* asser	ting function, worth thinking if we want to builtin
 * the e	xiting and cleaning into it*/
bool		mrt_assert(bool condition, char *format, ...);
bool		mrt_warning(bool condition, char *format, ...);
void		cleanup_scene(t_scene *sc);
/* array	 utils */
int			array_len(char **array);
void		free_array(char **array);
/* vector parsing utils */
double		ft_atof(const char *str);
bool		vec3_in_range(t_vec3 v, float lower, float upper);
bool		parse_vec3(char *str, t_vec3 *out, float min, float max);
bool		validate_input_and_parse_map(int ac, char **av, t_scene *scene);

#endif
