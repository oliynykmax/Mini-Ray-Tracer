#include "minirt.h"

static void	cleanup_parsing(t_parse *map)
{
	get_next_line(-1);
	if (map->fd >= 0)
		close(map->fd);
	if (map->buff)
		free(map->buff);
	if (map->line)
		free_array(map->line);
	cleanup_scene(map->sc);
}

void	mrt_assert(t_parse *map, bool condition, char *format, ...)
{
	const int	errno_value = errno;
	va_list		args;

	if (condition == true)
		return ;
	ft_fprintf(2, "Error\n");
	if (format != NULL || errno_value != 0)
	{
		if (errno_value != 0)
		{
			ft_putstr_fd(strerror(errno_value), STDERR_FILENO);
			ft_putstr_fd(": ", STDERR_FILENO);
			write(2, "\n", 1);
		}
		va_start(args, format);
		printf_fd(2, format, &args);
		va_end(args);
	}
	cleanup_parsing(map);
	exit(EXIT_FAILURE);
}

bool	mrt_warning(bool condition, char *format, ...)
{
	va_list	args;

	if (condition == true)
		return (true);
	ft_fprintf(2, "Warning\n");
	if (format != NULL)
	{
		va_start(args, format);
		printf_fd(2, format, &args);
		va_end(args);
	}
	return (false);
}
