#include "minirt.h"

// Dithering function for masking color banding artifacts. The specific method
// is called Interleaved Gradient Noise; there are some good articles on it on
// the web.

static t_vec3	dither(float x, float y)
{
	x *= 0.06711056f;
	y *= 0.00583715f;
	x = fract(52.9829189f * fract(x + y)) / 255.0f;
	return (vec3(x, x, x));
}

// MLX loop hook. Runs every frame to render the next image.

static void	loop_hook(void *param)
{
	t_render *const	r = (t_render*) param;
	uint32_t		x;
	uint32_t		y;
	t_vec3			color;
	size_t			index;

	camera_update(r);
	show_stats_in_window_title(r);
	y = -1;
	while (++y < r->image->height)
	{
		x = -1;
		while (++x < r->image->width)
		{
			index = x + y * r->image->width;
			color = trace_pixel(r, x + r->jitter_x, y + r->jitter_y);
			color = vec3_add(color, dither(x, y));
			r->frame[index] = vec3_add(r->frame[index], color);
			color = vec3_scale(r->frame[index], 1.0f / r->frame_samples);
			mlx_put_pixel(r->image, x, y, vec3_to_color(color));
		}
	}
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
	const size_t	min_size = width * height * sizeof(t_vec3);

	mlx_resize_image(r->image, width, height);
	if (min_size > r->frame_size)
	{
		r->frame_size = min_size;
		free(r->frame);
		r->frame = malloc(r->frame_size);
		if (r->frame == NULL)
			mlx_close_window(r->mlx);
		else
			ft_bzero(r->frame, r->frame_size);
	}
}

// Renderer entry point. Sets up the MLX state and installs all event hooks.

void	render_scene(t_render *r, t_scene *scene)
{
	r->scene = scene;
	r->camera_yaw = atan2(scene->dir.z, scene->dir.x);
	r->camera_pitch = acos(scene->dir.y);
	r->mlx = mlx_init(480, 360, "miniRT", true);
	if (r->mlx != NULL)
	{
		r->image = mlx_new_image(r->mlx, r->mlx->width, r->mlx->height);
		if (r->image && mlx_image_to_window(r->mlx, r->image, 0, 0) != -1)
		{
			r->frame_size = r->mlx->width * r->mlx->height * sizeof(t_vec3);
			r->frame = malloc(r->frame_size);
			if (r->frame != NULL)
			{
				ft_bzero(r->frame, r->frame_size);
				mlx_key_hook(r->mlx, key_hook, r);
				mlx_resize_hook(r->mlx, resize_hook, r);
				if (mlx_loop_hook(r->mlx, loop_hook, r))
					mlx_loop(r->mlx);
			}
			free(r->frame);
		}
		mlx_delete_image(r->mlx, r->image);
	}
	mlx_terminate(r->mlx);
}
