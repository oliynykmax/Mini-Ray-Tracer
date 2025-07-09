/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 09:47:14 by maoliiny          #+#    #+#             */
/*   Updated: 2025/04/16 11:26:19 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_ntoa(char *s, long num, size_t len)
{
	int	i;

	i = len - 1;
	if (num < 0)
	{
		num = -num;
		s[0] = '-';
	}
	while (num > 0)
	{
		s[i--] = num % 10 + '0';
		num = num / 10;
	}
	s[len] = '\0';
}

char	*ft_itoa(int n)
{
	char	*result;
	size_t	len;
	long	cpy;

	len = n < 0;
	cpy = n;
	if (n == 0)
		return (ft_strdup("0"));
	while (cpy && ++len)
		cpy = cpy / 10;
	result = malloc(len + 1);
	if (!result)
		return (NULL);
	ft_ntoa(result, (long)n, len);
	return (result);
}
