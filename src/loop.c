#include "minirt.h"

static void	init_camera(t_render *r)
{
	const float		aspect = (float) r->image->width / (float) r->image->height;
	const t_vec3	x0 = vec3_scale(r->camera_x, -0.5f * aspect);
	const t_vec3	x1 = vec3_scale(r->camera_x, +0.5f * aspect);
	const t_vec3	y0 = vec3_scale(r->camera_y, -0.5f);
	const t_vec3	y1 = vec3_scale(r->camera_y, +0.5f);

	r->camera_z = vec3_normalize(r->scene->camera_pos);
	r->camera_x = vec3_cross(vec3(0.0f, 1.0f, 0.0f), r->camera_z);
	r->camera_y = vec3_cross(r->camera_x, r->camera_z);
	r->viewport[0] = vec3_sub(vec3_add(x0, y0), r->camera_z);
	r->viewport[1] = vec3_sub(vec3_add(x1, y0), r->camera_z);
	r->viewport[2] = vec3_sub(vec3_add(x0, y1), r->camera_z);
	r->viewport[3] = vec3_sub(vec3_add(x1, y1), r->camera_z);
}

// MLX loop hook. Runs every frame to render the next image.

static void	loop_hook(void *param)
{
	t_render *const	r = (t_render*) param;
	const uint32_t	w = r->image->width;
	const uint32_t	h = r->image->height;
	uint32_t		i;

	init_camera(r);
	show_stats_in_window_title(r);
	i = -1;
	while (++i < w * h)
		mlx_put_pixel(r->image, i % w, i / w, trace_pixel(r, i % w, i / w));
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

	r.scene = scene;
	r.mlx = mlx_init(1280, 720, "miniRT", true);
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
