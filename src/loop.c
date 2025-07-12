#include "minirt.h"

// Determine the color of a single pixel in the image.

static uint32_t	trace_pixel(t_render *r, int x, int y)
{
	const int	cr = x * 255 / r->image->width;
	const int	cg = y * 255 / r->image->height;

	return ((cr << 24) | (cg << 16) | 255);
}

// MLX loop hook. Runs every frame to render the next image.

static void	loop_hook(void *param)
{
	t_render *const	r = (t_render*) param;
	const uint32_t	w = r->image->width;
	const uint32_t	h = r->image->height;
	uint32_t		i;

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
