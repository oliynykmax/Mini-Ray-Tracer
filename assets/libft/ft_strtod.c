/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtod.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maoliiny <maoliiny@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 17:41:05 by maoliiny          #+#    #+#             */
/*   Updated: 2025/05/15 17:42:53 by maoliiny         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

static double	handle_decimal_part(const char *str, int *i)
{
	double	decimal;
	double	div;

	decimal = 0.0;
	div = 10.0;
	if (str[*i] == '.')
	{
		(*i)++;
		while (str[*i] >= '0' && str[*i] <= '9')
		{
			decimal += (str[*i] - '0') / div;
			div *= 10.0;
			(*i)++;
		}
	}
	return (decimal);
}

double	ft_strtod(const char *str, char **endptr)
{
	double	result;
	int		sign;
	int		i;

	result = 0.0;
	sign = 1;
	i = 0;
	while (str[i] == ' ' || str[i] == '\t')
		i++;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			sign = -1;
	while (str[i] >= '0' && str[i] <= '9')
		result = result * 10.0 + (str[i++] - '0');
	result += handle_decimal_part(str, &i);
	if (endptr)
		*endptr = (char *)(str + i);
	return (sign * result);
}
