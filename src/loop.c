#include "minirt.h"

static void	camera_move_angular(t_render *r)
{
	const float		scale = 0.006f * sinf(r->scene->camera_fov * 0.5f);
	static int32_t	prev[2];
	int32_t			curr[2];

	mlx_get_mouse_pos(r->mlx, &curr[0], &curr[1]);
	if (mlx_is_mouse_down(r->mlx, MLX_MOUSE_BUTTON_LEFT))
	{
		r->camera_yaw -= scale * (curr[0] - prev[0]);
		r->camera_pitch -= scale * (curr[1] - prev[1]);
		r->camera_pitch = clamp(r->camera_pitch, radians(1), radians(179));
		r->scene->camera_dir.x = sin(r->camera_pitch) * cos(r->camera_yaw);
		r->scene->camera_dir.y = cos(r->camera_pitch);
		r->scene->camera_dir.z = sin(r->camera_pitch) * sin(r->camera_yaw);
	}
	prev[0] = curr[0];
	prev[1] = curr[1];
}

static void	camera_move_horizontal(t_render *r)
{
	t_vec3	vec;
	t_vec3	move_x;
	t_vec3	move_z;

	move_x = vec3_normalize(vec3(r->camera_x.x, 0.0f, r->camera_x.z));
	move_z = vec3_normalize(vec3(r->camera_z.x, 0.0f, r->camera_z.z));
	r->key_forward = mlx_is_key_down(r->mlx, MLX_KEY_W);
	r->key_left = mlx_is_key_down(r->mlx, MLX_KEY_A);
	r->key_back = mlx_is_key_down(r->mlx, MLX_KEY_S);
	r->key_right = mlx_is_key_down(r->mlx, MLX_KEY_D);
	r->key_up = mlx_is_key_down(r->mlx, MLX_KEY_SPACE);
	r->key_down = mlx_is_key_down(r->mlx, MLX_KEY_LEFT_SHIFT);
	vec = vec3(0.0f, r->key_down - r->key_up, 0.0f);
	vec = vec3_add(vec, vec3_scale(move_x, r->key_right - r->key_left));
	vec = vec3_add(vec, vec3_scale(move_z, r->key_forward - r->key_back));
	vec = vec3_scale(vec, 3.0f * r->mlx->delta_time);
	r->scene->camera_pos = vec3_add(r->scene->camera_pos, vec);
}

static void	camera_update(t_render *r)
{
	const float	aspect = (float) r->image->width / (float) r->image->height;
	t_vec3		x0;
	t_vec3		x1;
	t_vec3		y0;
	t_vec3		y1;

	r->camera_z = r->scene->camera_dir;
	r->camera_x = vec3_cross(r->camera_z, vec3(0.0f, -1.0f, 0.0f));
	r->camera_y = vec3_cross(r->camera_z, r->camera_x);
	x0 = vec3_scale(r->camera_x, -0.5f * aspect);
	x1 = vec3_scale(r->camera_x, +0.5f * aspect);
	y0 = vec3_scale(r->camera_y, -0.5f);
	y1 = vec3_scale(r->camera_y, +0.5f);
	r->viewport[0] = vec3_add(vec3_add(x0, y0), r->camera_z);
	r->viewport[1] = vec3_add(vec3_add(x1, y0), r->camera_z);
	r->viewport[2] = vec3_add(vec3_add(x0, y1), r->camera_z);
	r->viewport[3] = vec3_add(vec3_add(x1, y1), r->camera_z);
	camera_move_angular(r);
	camera_move_horizontal(r);
}

static int	dither(int x, int y)
{
	static const unsigned char	blue_noise[] = {
		#include "../assets/blue-noise.inc"
	};

	return (blue_noise[(y % 64) * 64 + (x % 64)]);
}

// MLX loop hook. Runs every frame to render the next image.

static void	loop_hook(void *param)
{
	static uint8_t	dither_index;
	t_render *const	r = (t_render*) param;
	const uint32_t	w = r->image->width;
	const uint32_t	h = r->image->height;
	uint32_t		i;

	camera_update(r);
	show_stats_in_window_title(r);
	i = -1;
	while (++i < w * h)
		if (dither(i % w, i / w) == dither_index || true)
			mlx_put_pixel(r->image, i % w, i / w, trace_pixel(r, i % w, i / w));
	dither_index++;
}

// MLX key hook. Used to quit when the escape key is pressed.

static void	key_hook(mlx_key_data_t data, void *param)
{
	t_render *const	r = (t_render*) param;

	if (data.action == MLX_PRESS && data.key == MLX_KEY_ESCAPE)
		mlx_close_window(r->mlx);
}

// MLX window resize hook. Resizes the image to match the new window dimensions.

static void	resize_hook(int32_t width, int32_t height, void *param)
{
	t_render *const	r = (t_render*) param;

	mlx_resize_image(r->image, width, height);
}

// Renderer entry point. Sets up the MLX state and installs all event hooks.

void	render_scene(t_scene *scene)
{
	t_render	r;

	ft_bzero(&r, sizeof(r));
	r.scene = scene;
	r.camera_yaw = atan2(scene->camera_dir.z, scene->camera_dir.x);
	r.camera_pitch = acos(scene->camera_dir.y);
	r.mlx = mlx_init(480, 360, "miniRT", true);
	if (r.mlx != NULL)
	{
		r.image = mlx_new_image(r.mlx, r.mlx->width, r.mlx->height);
		if (r.image != NULL && mlx_image_to_window(r.mlx, r.image, 0, 0) != -1)
		{
			mlx_key_hook(r.mlx, key_hook, &r);
			mlx_resize_hook(r.mlx, resize_hook, &r);
			if (mlx_loop_hook(r.mlx, loop_hook, &r))
				mlx_loop(r.mlx);
		}
		mlx_delete_image(r.mlx, r.image);
	}
	mlx_terminate(r.mlx);
}
