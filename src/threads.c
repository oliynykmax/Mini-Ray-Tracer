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

// Render one thread's region of the frame. The region extends from y_min to
// y_max. During this function, it's important that the thread only reads from
// common render and scene data, to avoid data races. The only memory the thread
// should write to is the color/pixel data of its own region of the frame.

static void	threads_render(t_render *r, uint32_t y_min, uint32_t y_max)
{
	uint32_t		x;
	uint32_t		y;
	t_vec3			color;
	size_t			index;
	const size_t	pitch = sizeof(t_vec3) * r->image->width * (y_max - y_min);

	if (r->frame_samples == 1)
		memset(r->frame + y_min * r->image->width, 0, pitch);
	y = y_min - 1;
	while (++y < y_max)
	{
		x = -1;
		while (++x < r->image->width)
		{
			index = x + y * r->image->width;
			color = trace_pixel(r, x, y);
			r->frame[index] = vec3_add(r->frame[index], color);
			color = vec3_scale(r->frame[index], 1.0f / r->frame_samples);
			color = vec3_to_srgb(color);
			color = vec3_add(color, dither(x, y));
			mlx_put_pixel(r->image, x, y, vec3_to_color(color));
		}
	}
}

// Render thread entry point. The frame is divided between threads into regions
// of equal size. When the MLX loop hook is called, the main thread increments
// the jobs_available counter to indicate that there is rendering work to be
// done. The render threads wait for work to become available, using a condition
// variable to avoid busy waiting. After rendering its region of the frame, each
// render thread increments the jobs_finished counter to indicate completion.
// When all jobs for one frame have finished, the main thread is signaled, after
// which it returns from the MLX loop hook, and the cycle repeats.

static void	*threads_main(void *arg)
{
	static _Atomic size_t	thread_id_counter;
	t_render *const			r = (t_render*) arg;
	t_thread				thread;

	thread.id = thread_id_counter++;
	thread.job = thread.id;
	while (true)
	{
		pthread_mutex_lock(&r->mutex);
		while (thread.job >= r->jobs_available && !r->threads_stop)
			pthread_cond_wait(&r->available_cond, &r->mutex);
		if (r->threads_stop)
			break ;
		pthread_mutex_unlock(&r->mutex);
		thread.y_min = r->image->height / THREAD_COUNT * (thread.id + 0);
		thread.y_max = r->image->height / THREAD_COUNT * (thread.id + 1);
		threads_render(r, thread.y_min, thread.y_max);
		pthread_mutex_lock(&r->mutex);
		if (++r->jobs_finished == r->jobs_available)
			pthread_cond_signal(&r->finished_cond);
		pthread_mutex_unlock(&r->mutex);
		thread.job += THREAD_COUNT;
	}
	pthread_mutex_unlock(&r->mutex);
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
