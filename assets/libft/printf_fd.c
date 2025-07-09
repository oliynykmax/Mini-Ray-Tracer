/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 15:28:03 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/05 14:16:32 by maoliiny         ###   LAUSANNE.ch       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	printf_fd(int fd, const char *s, va_list *args)
{
	int		counter;
	int		temp;

	counter = 0;
	if (!s)
		return (-1);
	while (*s)
	{
		if (*s != '%')
		{
			if (write(fd, s++, 1) == -1)
				return (-1);
			counter += 1;
			continue ;
		}
		temp = choose_type_fd(args, &s, fd);
		if (temp == -1)
			return (temp);
		counter += temp;
		s++;
	}
	return (counter);
}

int	choose_type_fd(va_list *args, const char **s, int fd)
{
	(*s)++;
	if (**s == 's')
		return (printf_fd_s(va_arg(*args, char *), fd));
	else if (**s == 'c')
		return (printf_fd_c(va_arg(*args, int), fd));
	else if (**s == '%')
		return (write(fd, "%", 1));
	else if (**s == 'd' || **s == 'i')
		return (printf_fd_diux(va_arg(*args, int), "0123456789", fd));
	else if (**s == 'u')
		return (printf_fd_diux(va_arg(*args, unsigned), "0123456789", fd));
	else if (**s == 'x')
		return (printf_fd_diux(va_arg(*args, unsigned),
				"0123456789abcdef", fd));
	else if (**s == 'X')
		return (printf_fd_diux(va_arg(*args, unsigned),
				"0123456789ABCDEF", fd));
	else if (**s == 'p')
		return (printf_fd_p(va_arg(*args, void *), fd));
	if (write(fd, "%", 1) == -1 || write(fd, *s, 1) == -1)
		return (-1);
	return (2);
}

int	printf_fd_c(char c, int fd)
{
	return (write(fd, &c, 1));
}

int	printf_fd_s(char *s, int fd)
{
	if (!s)
		return (write(fd, "(null)", 6));
	return (write(fd, s, ft_strlen(s)));
}

int	printf_fd_p(void *ptr, int fd)
{
	if (!ptr)
		return (write(fd, "(nil)", 5));
	if (write(fd, "0x", 2) == -1)
		return (-1);
	return (2 + printf_fd_pnum((unsigned long long)ptr,
			"0123456789abcdef", fd));
}
