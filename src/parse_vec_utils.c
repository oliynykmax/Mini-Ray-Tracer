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
	double			result;
	int				sign;
	const char		*start = str;

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

/*
 * Checks if all components of a vector are within given boundaries
 * @param v The vector to check
 * @param lower Lower boundary (inclusive)
 * @param upper Upper boundary (inclusive)
 * @return true if all components are within boundaries, false otherwise
 */
bool	in_range3(t_vec3 v, float lower, float upper)
{
	return ((v.x >= lower - 0.001f) && (v.x <= upper + 0.001f)
		&& (v.y >= lower - 0.001f) && (v.y <= upper + 0.001f) && (v.z >= lower
			- 0.001f) && (v.z <= upper + 0.001f));
}

/*
** Splits a string by a comma, strictly for 3 components.
** Calls fatal_if on error.
*/
static char	**ft_split_vec(t_parse *m, const char *str)
{
	char		**result;
	const char	*p1 = ft_strchr(str, ',');
	const char	*p2;

	p2 = NULL;
	if (p1)
		p2 = ft_strchr(p1 + 1, ',');
	fatal_if(m, p1 == NULL || p2 == NULL || ft_strchr(p2 + 1, ','),
		"Vector format must be 'x,y,z' with exactly 2 commas\n");
	result = malloc(sizeof(char *) * 4);
	fatal_if(m, result == NULL, "Memory allocation failure\n");
	m->temp_split = result;
	result[0] = ft_substr(str, 0, p1 - str);
	result[1] = ft_substr(str, p1 - str + 1, p2 - (p1 + 1));
	result[2] = ft_substr(p2 + 1, 0, ft_strlen(p2 + 1));
	result[3] = NULL;
	fatal_if(m, (p1 - str) == 0 || (p2 - (p1 + 1)) == 0
		|| ft_strlen(p2 + 1) == 0,
		"Vector components must not be empty\n");
	fatal_if(m, result[0] == NULL || result[1] == NULL
		|| result[2] == NULL, "Memory allocation failure in substr\n");
	return (result);
}

/*
* Parses a string "x,y,z" into a vec3 and (optionally) validates component range.
 * Exits on failure using fatal_if (no return value).
 * @param m       Parsing context (for unified cleanup on error)
 * @param str     Input string in the form "x,y,z"
 * @param out     Destination vector
 * @param limits  Two-element array {min, max}; if limits[0] == limits[1],
	range check is skipped
 */
void	parse3(t_parse *m, const char *str, t_vec3 *out, float limits[2])
{
	char	**split;

	split = ft_split_vec(m, str);
	*out = vec3(ft_atof(m, split[0]), ft_atof(m, split[1]), ft_atof(m,
				split[2]));
	m->temp_split = NULL;
	free_array(split);
	if (limits[0] != limits[1])
		fatal_if(m, !in_range3(*out, limits[0], limits[1]),
			"Vector values must be between %d and %d\n", (int)limits[0],
			(int)limits[1]);
}
