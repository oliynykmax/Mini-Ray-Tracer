/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 19:58:13 by maoliiny          #+#    #+#             */
/*   Updated: 2025/04/15 20:12:30 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	size_t	i;
	char	*ret_string;

	if (!s || !f)
		return (NULL);
	i = 0;
	ret_string = malloc(sizeof(char) * (ft_strlen(s) + 1));
	if (!ret_string)
		return (NULL);
	while (s[i])
	{
		ret_string[i] = f(i, s[i]);
		i++;
	}
	ret_string[i] = '\0';
	return (ret_string);
}
