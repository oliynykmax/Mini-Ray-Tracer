/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 10:01:27 by maoliiny          #+#    #+#             */
/*   Updated: 2025/04/15 11:14:24 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char		search;
	const unsigned char	*string;
	size_t				i;

	search = (unsigned char)c;
	string = (const unsigned char *)s;
	i = 0;
	while (i < n)
	{
		if (search == string[i])
			return ((void *)&string[i]);
		i++;
	}
	return (NULL);
}
