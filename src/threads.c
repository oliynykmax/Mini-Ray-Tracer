#include "minirt.h"

static void	render_sub_tile(t_render *r, int x0, int y0, uint32_t frame)
{
	t_vec3		color;
	size_t		index;
	uint32_t	d;
	int			x;
	int			y;

	d = TILE_SIZE >> min(31, frame);
	index = x0 + y0 * r->tiles_x * TILE_SIZE;
	color = tonemap3(trace_pixel(r, x0, y0, frame));
	if (d <= 1)
	{
		r->frame[index] = scale3(r->frame[index], d == 0);
		r->frame[index] = add3(r->frame[index], color);
		color = scale3(r->frame[index], 1.0f / (frame - 3));
	}
	color = add3(to_srgb3(color), dither(x0, y0));
	y = y0;
	while (y < min(y0 + d + !d, r->image->height))
	{
		x = x0;
		while (x < min(x0 + d + !d, r->image->width))
			mlx_put_pixel(r->image, x++, y, to_color3(color));
		y++;
	}
}

static void	render_tile(t_render *r, uint32_t x0, uint32_t y0, uint32_t frame)
{
	const uint32_t	d = max(1, TILE_SIZE >> min(31, frame));
	uint32_t		x;
	uint32_t		y;

	y = y0;
	while (y < y0 + TILE_SIZE)
	{
		x = x0;
		while (x < x0 + TILE_SIZE)
		{
			if (!r->threads_pause)
				render_sub_tile(r, x, y, frame);
			x += d;
		}
		y += d;
	}
}

static void	*threads_main(void *arg)
{
	t_render	*r;
	uint64_t	job;
	uint64_t	tile;
	uint32_t	x0;
	uint32_t	y0;

	r = (t_render *) arg;
	while (!r->threads_stop)
	{
		while (r->threads_pause)
			usleep(100);
		job = r->jobs_started++;
		tile = (job - r->last_reset) % r->tiles_per_frame;
		x0 = TILE_SIZE * (tile % r->tiles_x);
		y0 = TILE_SIZE * (tile / r->tiles_x);
		render_tile(r, x0, y0, (job - r->last_reset) / r->tiles_per_frame);
		r->jobs_finished++;
	}
	return (NULL);
}

// Create synchronization objects and start the render threads. Returns true on
// success, or false if an object couldn't be created. The threads_started
// counter keeps track of how many threads were actually started.

bool	threads_init(t_render *r)
{
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
	r->threads_stop = true;
	while (r->threads_started-- > 0)
		pthread_join(r->threads[r->threads_started], NULL);
}
