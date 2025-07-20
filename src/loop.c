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

	pthread_mutex_lock(&r->mutex);
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

	pthread_mutex_lock(&r->mutex);
	if (data.action == MLX_PRESS && data.key == MLX_KEY_ESCAPE)
		mlx_close_window(r->mlx);
	pthread_mutex_unlock(&r->mutex);
}

// MLX window resize hook. Resizes the image to match the new window dimensions.

static void	resize_hook(int32_t width, int32_t height, void *param)
{
	t_render *const	r = (t_render*) param;
	const size_t	min_size = width * height * sizeof(t_vec3);

	pthread_mutex_lock(&r->mutex);
	mlx_resize_image(r->image, width, height);
	if (min_size > r->frame_size)
	{
		r->frame_size = min_size;
		free(r->frame);
		r->frame = malloc(r->frame_size);
		if (r->frame == NULL)
			mlx_close_window(r->mlx);
	}
	if (r->frame != NULL)
		ft_bzero(r->frame, r->image->width * r->image->height * sizeof(t_vec3));
	r->frame_samples = 0;
	pthread_mutex_unlock(&r->mutex);
}

static void	thread_render(t_render *r, int thread_id)
{
	const uint32_t	y_min = r->image->height / THREAD_COUNT * (thread_id + 0);
	const uint32_t	y_max = r->image->height / THREAD_COUNT * (thread_id + 1);
	uint32_t		x;
	uint32_t		y;
	t_vec3			color;
	size_t			index;

	y = y_min - 1;
	while (++y < y_max)
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

static void	*threads_main(void *arg)
{
	t_render *const			r = (t_render*) arg;
	static _Atomic size_t	job_counter;
	size_t					job_id;

	job_id = job_counter++;
	while (true)
	{
		pthread_mutex_lock(&r->mutex);
		while (job_id >= r->jobs_available && !r->threads_stop)
			pthread_cond_wait(&r->available_cond, &r->mutex);
		if (r->threads_stop)
		{
			pthread_mutex_unlock(&r->mutex);
			break ;
		}
		pthread_mutex_unlock(&r->mutex);
		thread_render(r, job_id % THREAD_COUNT);
		pthread_mutex_lock(&r->mutex);
		if (++r->jobs_finished == r->jobs_available)
			pthread_cond_signal(&r->finished_cond);
		pthread_mutex_unlock(&r->mutex);
		job_id += THREAD_COUNT;
	}
	return (NULL);
}

static bool	threads_init(t_render *r)
{
	int	i;

	if (pthread_mutex_init(&r->mutex, NULL) != 0)
		return (false);
	if (pthread_cond_init(&r->available_cond, NULL) != 0
	|| pthread_cond_init(&r->finished_cond, NULL) != 0)
		return (false);
	i = -1;
	while (++i < THREAD_COUNT)
	{
		if (pthread_create(&r->threads[i], NULL, threads_main, r))
			break ;
		r->threads_started++;
	}
	return (true);
}

static void	threads_quit(t_render *r)
{
	int	i;

	pthread_mutex_lock(&r->mutex);
	r->threads_stop = true;
	pthread_cond_broadcast(&r->available_cond);
	pthread_mutex_unlock(&r->mutex);
	i = -1;
	while (++i < r->threads_started)
		pthread_join(r->threads[i], NULL);
	pthread_mutex_destroy(&r->mutex);
	pthread_cond_destroy(&r->available_cond);
	pthread_cond_destroy(&r->finished_cond);
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
				threads_init(r);
				ft_bzero(r->frame, r->frame_size);
				mlx_key_hook(r->mlx, key_hook, r);
				mlx_resize_hook(r->mlx, resize_hook, r);
				if (mlx_loop_hook(r->mlx, loop_hook, r))
					mlx_loop(r->mlx);
				threads_quit(r);
			}
			free(r->frame);
		}
		mlx_delete_image(r->mlx, r->image);
	}
	mlx_terminate(r->mlx);
}
