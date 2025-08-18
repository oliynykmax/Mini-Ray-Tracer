/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 15:09:11 by maoliiny          #+#    #+#             */
/*   Updated: 2025/08/18 15:49:12 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_fnl(char *s, char c)
{
	size_t	i;

	if (!s)
		return (-1);
	i = 0;
	while (s[i])
	{
		if (s[i] == c)
			return ((int)i);
		i++;
	}
	return (-1);
}

int	ft_strcat(char **res, int fd)
{
	char	*buff;
	char	*temp_join;
	int		read_bytes;

	buff = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buff)
		return (-1);
	read_bytes = read(fd, buff, BUFFER_SIZE);
	if (read_bytes <= 0)
	{
		free(buff);
		return (read_bytes);
	}
	buff[read_bytes] = '\0';
	temp_join = ft_strjoin(*res, buff);
	free(buff);
	if (!temp_join)
		return (-1);
	if (*res)
		free(*res);
	*res = temp_join;
	return (read_bytes);
}

void	*ft_memcpy_gnl(void *dest, const void *src, size_t n)
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
