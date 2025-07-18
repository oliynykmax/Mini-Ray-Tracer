#include "minirt.h"

// Dithering function for masking color banding artifacts. The specific method
// is called Interleaved Gradient Noise; there are some good articles on it on
// the web.

static float	dither(float x, float y)
{
	return (fract(52.9829189f * fract(0.06711056f * x + 0.00583715f * y)));
}

// MLX loop hook. Runs every frame to render the next image.

static void	loop_hook(void *param)
{
	t_render *const	r = (t_render*) param;
	uint32_t		x;
	uint32_t		y;
	t_vec3			color;
	float			noise;

	camera_update(r);
	show_stats_in_window_title(r);
	y = -1;
	while (++y < r->image->height)
	{
		x = -1;
		while (++x < r->image->width)
		{
			noise = dither(x, y) / 255.0f * 10.0f;
			color = trace_pixel(r, x, y);
			color = vec3_add(color, vec3(noise, noise, noise));
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

	mlx_resize_image(r->image, width, height);
}

// Renderer entry point. Sets up the MLX state and installs all event hooks.

void	render_scene(t_scene *scene)
{
	t_render	r;

	ft_bzero(&r, sizeof(r));
	r.scene = scene;
	r.camera_yaw = atan2(scene->dir.z, scene->dir.x);
	r.camera_pitch = acos(scene->dir.y);
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
