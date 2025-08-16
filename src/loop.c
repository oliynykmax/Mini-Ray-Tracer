#include "minirt.h"

// MLX loop hook. Runs every frame to render the next image.

static void	loop_hook(void *param)
{
	t_render *const	r = (t_render*) param;

	pthread_mutex_lock(&r->mutex);
	transform_selection(r);
	camera_update(r);
	show_stats_in_window_title(r);
	r->jobs_available += THREAD_COUNT;
	pthread_cond_broadcast(&r->available_cond);
	while (r->jobs_finished < r->jobs_available)
		pthread_cond_wait(&r->finished_cond, &r->mutex);
	pthread_mutex_unlock(&r->mutex);
}

// MLX key hook. Used to quit when the escape key is pressed.

static void	key_hook(mlx_key_data_t data, void *param)
{
	t_render *const	r = (t_render*) param;

	if (data.action != MLX_PRESS)
		return ;
	if (data.key == MLX_KEY_ESCAPE)
		mlx_close_window(r->mlx);
	if (data.key == MLX_KEY_F)
	{
		r->fancy = !r->fancy;
		r->frame_samples = 0;
	}
}

// MLX mouse hook. Used for picking objects with the right mouse button.

static void	mouse_hook(mouse_key_t b, action_t a, modifier_key_t m, void *param)
{
	t_render *const	r = (t_render*) param;
	t_vec3			rd;
	int32_t			x;
	int32_t			y;

	(void) m;
	if (b != MLX_MOUSE_BUTTON_LEFT)
		return ;
	r->selection = NULL;
	if (a == MLX_PRESS)
	{
		mlx_get_mouse_pos(r->mlx, &x, &y);
		rd = get_viewport_ray(r, x, y, false);
		scene_distance(r->scene, r->scene->pos, rd, &r->selection);
		if (mlx_is_key_down(r->mlx, MLX_KEY_Z))
			r->mode = MODE_TRANSLATE;
		if (mlx_is_key_down(r->mlx, MLX_KEY_X))
			r->mode = MODE_ROTATE;
		if (mlx_is_key_down(r->mlx, MLX_KEY_C))
			r->mode = MODE_SCALE;
	}
	if (r->selection == NULL)
		r->mode = MODE_NORMAL;
}

// MLX window resize hook. Resizes the image to match the new window dimensions.

static void	resize_hook(int32_t width, int32_t height, void *param)
{
	t_render *const	r = (t_render*) param;
	const size_t	min_size = width * height * sizeof(t_vec3);

	mlx_resize_image(r->image, width, height);
	if (min_size > r->frame_size)
	{
		r->frame_size = min_size;
		free(r->frame);
		r->frame = malloc(r->frame_size);
		if (r->frame == NULL)
			mlx_close_window(r->mlx);
	}
	r->frame_samples = 0;
}

// Renderer entry point. Sets up the MLX state and installs all event hooks.

void	render_scene(t_render *r)
{
	r->camera_yaw = atan2(r->scene->dir.z, r->scene->dir.x);
	r->camera_pitch = acos(r->scene->dir.y);
	r->mlx = mlx_init(1 * 480, 1 * 360, "miniRT", true);
	if (r->mlx != NULL)
	{
		r->image = mlx_new_image(r->mlx, r->mlx->width, r->mlx->height);
		if (r->image && mlx_image_to_window(r->mlx, r->image, 0, 0) != -1)
		{
			r->frame_size = r->mlx->width * r->mlx->height * sizeof(t_vec3);
			r->frame = malloc(r->frame_size);
			if (r->frame != NULL)
			{
				mlx_key_hook(r->mlx, key_hook, r);
				mlx_mouse_hook(r->mlx, mouse_hook, r);
				mlx_resize_hook(r->mlx, resize_hook, r);
				if (mlx_loop_hook(r->mlx, loop_hook, r) && threads_init(r))
					mlx_loop(r->mlx);
				threads_quit(r);
			}
			free(r->frame);
		}
		mlx_delete_image(r->mlx, r->image);
	}
	mlx_terminate(r->mlx);
}
