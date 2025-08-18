#include "minirt.h"

static double	parse_number_part(t_parse *m, const char **str,
		double start_val, double divisor_step)
{
	double	result;

	result = start_val;
	while (**str >= '0' && **str <= '9')
	{
		if (divisor_step == 1.0)
			result = result * 10.0 + (**str - '0');
		else
		{
			result += (**str - '0') / divisor_step;
			divisor_step *= 10.0;
		}
		if (fabs(result) > 1e8f)
			fatal_if(m, 1, "Overflow in number\n");
		(*str)++;
	}
	return (result);
}

double	ft_atof(t_parse *m, const char *str)
{
	double		result;
	int			sign;
	const char	*start = str;

	result = 0.0;
	sign = 1;
	if (*str == '+' || *str == '-')
		sign = -2 * (*str++ == '-') + 1;
	fatal_if(m, *str == '\0', "Not a number: %s\n", start);
	result = parse_number_part(m, &str, 0.0, 1.0);
	if (*str == '\0')
		return (result * sign);
	if (*str == '.')
	{
		str++;
		fatal_if(m, *str < '0' || *str > '9', "Bad number: %s\n", start);
		result = parse_number_part(m, &str, result, 10.0);
	}
	fatal_if(m, *str != '\0', "Not a number: %s\n", start);
	fatal_if(m, result != 0.0 && fabs(result) < 1.0e-6,
		"Underflow in number\n");
	return (result * sign);
}

bool	in_range3(t_vec3 v, float lower, float upper)
{
	return ((v.x >= lower - 0.001f) && (v.x <= upper + 0.001f) && (v.y >= lower
			- 0.001f) && (v.y <= upper + 0.001f) && (v.z >= lower - 0.001f)
		&& (v.z <= upper + 0.001f));
}

/* parse3 using ft_atof with fixed internal buffer (no malloc, no mutation) */
void	parse3(t_parse *m, const char *s, t_vec3 *out, float limits[2])
{
	char	buf[32];
	int		ip[2];
	float	vals[3];

	ft_memset(ip, 0, 8);
	while (ip[0] < (int) sizeof(buf) - 1)
	{
		if (*s && *s != ',')
		{
			buf[ip[0]++] = *s++;
			continue ;
		}
		buf[ip[0]] = '\0';
		fatal_if(m, ip[0] == 0, "Empty vector component\n");
		vals[ip[1]++] = ft_atof(m, buf);
		ip[0] = 0;
		if (*s++ == '\0')
			break ;
		fatal_if(m, ip[1] > 3, "Vector format must be x,y,z\n");
	}
	fatal_if(m, ip[1] != 3, "Vector format must be x,y,z\n");
	*out = vec3(vals[0], vals[1], vals[2]);
	fatal_if(m, !in_range3(*out, limits[0], limits[1]) * (limits[0] != limits[1]
			), "Vector values must be between %d and %d\n",
		(int)limits[0], (int)limits[1]);
}
