#include "minirt.h"

bool	mrt_assert(bool condition, char *msg)
{
	const int	errno_value = errno;

	if (condition == true)
		return (true);
	ft_fprintf(2, "Error\n");
	if (msg != NULL || errno_value != 0)
	{
		ft_putstr_fd(strerror(errno_value), STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(msg, STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
	}
	return (false);
}
