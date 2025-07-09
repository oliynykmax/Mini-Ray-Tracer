/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 09:43:12 by maoliiny          #+#    #+#             */
/*   Updated: 2025/04/19 10:52:24 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static	void	write_digit(long number, int fd)
{
	if (number < 0)
	{
		ft_putchar_fd('-', fd);
		number = -number;
	}
	if (number >= 10)
		write_digit(number / 10, fd);
	ft_putchar_fd((number % 10) + '0', fd);
}

void	ft_putnbr_fd(int n, int fd)
{
	write_digit(n, fd);
}
