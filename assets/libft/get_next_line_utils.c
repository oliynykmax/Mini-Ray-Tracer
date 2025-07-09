/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/27 15:09:11 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/05 14:30:06 by maoliiny         ###   LAUSANNE.ch       */
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

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	len1;
	size_t	len2;
	size_t	total_len;
	char	*result;

	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	total_len = len1 + len2 + 1;
	result = malloc(total_len * sizeof(char));
	if (!result)
		return (NULL);
	ft_memcpy(result, s1, len1);
	ft_memcpy(result + len1, s2, len2);
	result[total_len - 1] = '\0';
	return (result);
}

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
