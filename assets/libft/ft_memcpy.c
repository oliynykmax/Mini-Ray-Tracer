/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 18:31:38 by maoliiny          #+#    #+#             */
/*   Updated: 2025/04/15 11:16:45 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char		*dstp;
	const unsigned char	*srcp;
	size_t				i;

	dstp = (unsigned char *)dest;
	srcp = (const unsigned char *)src;
	i = 0;
	if (n == 0 || dest == src)
		return (dest);
	if (dest == NULL && src == NULL)
		return (dest);
	while (i < n)
	{
		dstp[i] = srcp[i];
		i++;
	}
	return (dest);
}
