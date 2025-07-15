#include "minirt.h"

bool	mrt_assert(bool condition, char *format, ...)
{
	const int	errno_value = errno;
	va_list		args;

	if (condition == true)
		return (true);
	ft_fprintf(2, "Error\n");
	if (format != NULL || errno_value != 0)
	{
		if (errno_value != 0)
		{
			ft_putstr_fd(strerror(errno_value), STDERR_FILENO);
			ft_putstr_fd(": ", STDERR_FILENO);
		}
		va_start(args, format);
		printf_fd(STDERR_FILENO, format, &args);
		va_end(args);
	}
	return (false);
}
