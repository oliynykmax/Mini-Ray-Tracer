/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 13:47:08 by maoliiny          #+#    #+#             */
/*   Updated: 2025/06/14 13:55:30 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

long	ft_atoi(const char *nptr)
{
	long	num;
	long	minus;
	int		dig;

	num = 0;
	minus = 1;
	while ((*nptr >= 9 && *nptr <= 13) || *nptr == 32)
		nptr++;
	if (*nptr == '+' || *nptr == '-')
		if (*nptr++ == '-')
			minus = -1;
	while (ft_isdigit(*nptr))
	{
		dig = *nptr - '0';
		if (minus == 1 && (num > LONG_MAX / 10 || (num == LONG_MAX / 10
					&& dig > LONG_MAX % 10)))
			return (LONG_MAX);
		if (minus == -1 && (num > -(LONG_MIN / 10) || (num == -(LONG_MIN / 10)
					&& dig > -(LONG_MIN % 10))))
			return (LONG_MIN);
		num = num * 10 + dig;
		nptr++;
	}
	return (num * minus);
}

int	ft_atoi_safe(const char *nptr, long *result)
{
	long	num;
	long	minus;
	int		dig;

	num = 0;
	minus = 1;
	while ((*nptr >= 9 && *nptr <= 13) || *nptr == 32)
		nptr++;
	if (*nptr == '+' || *nptr == '-')
		if (*nptr++ == '-')
			minus = -1;
	while (ft_isdigit(*nptr))
	{
		dig = *nptr - '0';
		if (minus == 1 && (num > LONG_MAX / 10 || (num == LONG_MAX / 10
					&& dig > LONG_MAX % 10)))
			return (0);
		if (minus == -1 && (num > -(LONG_MIN / 10) || (num == -(LONG_MIN / 10)
					&& dig > -(LONG_MIN % 10))))
			return (0);
		num = num * 10 + dig;
		nptr++;
	}
	*result = num * minus;
	return (1);
}
