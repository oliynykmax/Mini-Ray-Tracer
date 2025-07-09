/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:04:34 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/18 22:20:42 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_strchar(char c, char str)
{
	if (str == c)
		return (1);
	return (0);
}

static char	*ft_strdup_loc(const char *src, int n)
{
	char	*dest;
	int		i;

	i = 0;
	dest = (char *)malloc((n + 1));
	while (i < n && src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

static int	ft_sp(const char *str, char charset, char **tab)
{
	int	start;
	int	counter;
	int	i;

	counter = 0;
	i = 0;
	while (str[i])
	{
		while (str[i] && ft_strchar(str[i], charset))
			i++;
		start = i;
		while (!ft_strchar(str[i], charset) && str[i])
			i++;
		if (tab && start < i)
			tab[counter] = ft_strdup_loc(&str[start], i - start);
		if (start < i)
			counter++;
	}
	return (counter);
}

char	**ft_split(char const *s, char c)
{
	char	**tab;
	int		i;

	i = ft_sp(s, c, NULL);
	tab = malloc(sizeof(char *) * (i + 1));
	if (!tab)
		return (NULL);
	ft_sp(s, c, tab);
	tab[i] = 0;
	return (tab);
}
