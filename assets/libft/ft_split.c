/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:04:34 by maoliiny          #+#    #+#             */
/*   Updated: 2025/08/16 17:20:00 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	split(const char *string, char delimiter, char **array)
{
	size_t	array_length;
	size_t	start;
	size_t	index;

	index = 0;
	array_length = 0;
	while (string[index] != '\0')
	{
		while (string[index] == delimiter)
			index++;
		start = index;
		while (string[index] != delimiter && string[index] != '\0')
			index++;
		if (start != index)
		{
			if (array != NULL)
			{
				array[array_length] = ft_substr(string, start, index - start);
				if (array[array_length] == NULL)
					return (-1);
			}
			array_length++;
		}
	}
	return (array_length);
}

char	**ft_split(const char *string, char delimiter)
{
	size_t	array_length;
	char	**array;

	if (string == NULL)
		return (NULL);
	array_length = split(string, delimiter, NULL);
	array = ft_calloc((array_length + 1), sizeof(char *));
	if (array != NULL)
	{
		array[array_length] = NULL;
		if (split(string, delimiter, array) == (size_t) - 1)
		{
			while (array_length-- != 0)
				free(array[array_length]);
			free(array);
			return (NULL);
		}
	}
	return (array);
}
