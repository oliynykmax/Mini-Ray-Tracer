#include "libft.h"

int	ft_str_is_whitespace(const char *str)
{
	if (!str)
		return (0);
	while (*str)
	{
		if (*str != ' ' && *str != '\t' && *str != '\n')
			return (0);
		str++;
	}
	return (1);
}
