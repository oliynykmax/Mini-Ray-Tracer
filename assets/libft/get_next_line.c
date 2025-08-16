/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 14:31:16 by maoliiny          #+#    #+#             */
/*   Updated: 2025/08/15 14:02:54 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strncpy(const char *src, int n)
{
	char	*dest;
	int		i;

	i = 0;
	dest = malloc(sizeof(char) * (n + 1));
	if (!dest)
		return (NULL);
	while (i < n && src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*extract_line(char **res)
{
	char	*line;
	char	*tail;
	int		idx;

	idx = ft_fnl(*res, '\n');
	if (idx != -1)
	{
		line = ft_strncpy(*res, idx + 1);
		tail = ft_strncpy(*res + idx + 1, ft_strlen(*res) - idx - 1);
	}
	else
	{
		idx = ft_strlen(*res);
		line = ft_strncpy(*res, idx);
		tail = ft_strncpy("", 0);
	}
	free(*res);
	*res = tail;
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*res;
	int			bytes;

	if (fd == -1)
	{
		free(res);
		res = NULL;
	}
	if (fd < 0 || BUFFER_SIZE <= 0 || fd > FD_NUM)
		return (NULL);
	if (!res)
		res = ft_strncpy("", 0);
	bytes = 1;
	while (ft_fnl(res, '\n') == -1 && bytes > 0)
		bytes = ft_strcat(&res, fd);
	if ((bytes <= 0) && *res == '\0')
	{
		free(res);
		res = NULL;
		return (NULL);
	}
	return (extract_line(&res));
}
