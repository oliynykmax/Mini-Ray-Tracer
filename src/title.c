#include "minirt.h"

// Add a string to the title buffer. Returns a pointer to the position in the
// buffer where the next character could be written.

static char	*add_string(char *buffer, char *string)
{
	while (*string != '\0')
		*buffer++ = *string++;
	return (buffer);
}

// Add an integer value to the title buffer. Returns a pointer to the position
// in the buffer where the next character could be written. Does not handle
// overflow if the value equals INT_MIN.

static char	*add_int(char *buffer, int value)
{
	if (value < 0)
	{
		*buffer++ = '-';
		return (add_int(buffer, -value));
	}
	if (value >= 10)
		buffer = add_int(buffer, value / 10);
	*buffer++ = '0' + value % 10;
	return (buffer);
}

// Build a string of useful stats and set it as the window title.

static void	update_window_title(t_render *r)
{
	char	title[256];
	char	*buffer;

	ft_bzero(title, sizeof(title));
	buffer = title;
	buffer = add_string(buffer, "miniRT — objects: ");
	buffer = add_int(buffer, r->scene->object_count);
	buffer = add_string(buffer, " | delta: ");
	buffer = add_int(buffer, r->mlx->delta_time * 1000.0);
	buffer = add_string(buffer, " ms");
	buffer = add_string(buffer, " | fps: ");
	buffer = add_int(buffer, 1.0 / r->mlx->delta_time);
	buffer = add_string(buffer, " | resolution: ");
	buffer = add_int(buffer, r->image->width);
	buffer = add_string(buffer, " x ");
	buffer = add_int(buffer, r->image->height);
	mlx_set_window_title(r->mlx, title);
}

// Update the window title with useful stats, also rate-limiting title updates
// so that it's easier to read.

void	show_stats_in_window_title(t_render *r)
{
	static double	update_timer;
	const double	update_rate = 0.1;

	update_timer += r->mlx->delta_time;
	if (update_timer < update_rate)
		return ;
	update_timer = fmod(update_timer, update_rate);
	update_window_title(r);
}
