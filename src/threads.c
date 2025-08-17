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

static void	render_sub_tile(t_render *r, uint32_t x0, uint32_t y0, uint32_t frame)
{
	uint32_t frame2 = frame > 4 ? 4 : frame;
	uint32_t d = TILE_SIZE >> frame2;
	t_vec3	color = trace_pixel(r, x0, y0, frame);
	color = tonemap3(color);
	size_t index = x0 + y0 * r->image->width;
	if (frame == 4)
		r->frame[index] = vec3(0.0f, 0.0f, 0.0f);
	if (frame >= 4) {
		r->frame[index] = add3(r->frame[index], color);
		color = scale3(r->frame[index], 1.0f / (frame - 3));
	}
	color = to_srgb3(color);
	color = add3(color, dither(x0, y0));
	uint32_t color_u32 = to_color3(color);
	d += !d;
	uint32_t x_max = x0 + d < r->image->width  ? x0 + d : r->image->width;
	uint32_t y_max = y0 + d < r->image->height ? y0 + d : r->image->height;
	for (uint32_t y = y0; y < y_max; y++)
	for (uint32_t x = x0; x < x_max; x++) {
		mlx_put_pixel(r->image, x, y, color_u32);
	}
}

static void	threads_render(t_render *r, uint32_t x0, uint32_t y0, uint32_t frame)
{
	uint32_t frame2 = frame > 4 ? 4 : frame;
	uint32_t d = (TILE_SIZE >> frame2) + !(TILE_SIZE >> frame2);
	for (uint32_t y = y0; y < y0 + TILE_SIZE; y += d)
	for (uint32_t x = x0; x < x0 + TILE_SIZE; x += d) {
		render_sub_tile(r, x, y, frame);
	}
}

static void	*threads_main(void *arg)
{
	t_render *const	r = (t_render*) arg;
	uint64_t tiles_x = (r->image->width  + TILE_SIZE - 1) / TILE_SIZE;
	uint64_t tiles_y = (r->image->height + TILE_SIZE - 1) / TILE_SIZE;
	uint64_t tiles_per_frame = tiles_x * tiles_y;
	r->fancy = true;

	while (!r->threads_stop)
	{
		uint64_t job = r->job_counter++;
		uint64_t frame_tile = (job - r->last_reset) % tiles_per_frame;
		uint32_t x0 = TILE_SIZE * (frame_tile % tiles_x);
		uint32_t y0 = TILE_SIZE * (frame_tile / tiles_x);
		uint32_t frame = (job - r->last_reset) / tiles_per_frame;
		threads_render(r, x0, y0, frame);
	}
	return (NULL);
}

// Create synchronization objects and start the render threads. Returns true on
// success, or false if an object couldn't be created. The threads_started
// counter keeps track of how many threads were actually started.

bool	threads_init(t_render *r)
{
	if (pthread_mutex_init(&r->mutex, NULL) != 0)
		return (false);
	if (pthread_cond_init(&r->available_cond, NULL) != 0
		|| pthread_cond_init(&r->finished_cond, NULL) != 0)
		return (false);
	while (r->threads_started < THREAD_COUNT)
	{
		if (pthread_create(&r->threads[r->threads_started], 0, threads_main, r))
			break ;
		r->threads_started++;
	}
	return (r->threads_started == THREAD_COUNT);
}

// Signal the render threads to stop, wait for all of them to finish, then
// destroy all synchronization objects.

void	threads_quit(t_render *r)
{
	pthread_mutex_lock(&r->mutex);
	r->threads_stop = true;
	pthread_cond_broadcast(&r->available_cond);
	pthread_mutex_unlock(&r->mutex);
	while (r->threads_started-- > 0)
		pthread_join(r->threads[r->threads_started], NULL);
	pthread_mutex_destroy(&r->mutex);
	pthread_cond_destroy(&r->available_cond);
	pthread_cond_destroy(&r->finished_cond);
}
