#include "minirt.h"

double	ft_atof(t_parse *m, const char *str)
{
	double	result;
	double	divisor;
	int		sign;

	result = 0.0;
	divisor = 10.0;
	sign = 1;
	while (*str == ' ' || *str == '\t' || *str == '\n')
		str++;
	if (*str == '+' || *str == '-')
		sign = -1 * (*str++ == '-');
	while (*str >= '0' && *str <= '9')
		result = result * 10.0 + (*str++ - '0');
	if (*str == '\0')
		return (result * sign);
	if (*str == '.')
		str++;
	fatal_if(m, *str < '0' || *str > '9', "Bad number: %s\n", str);
	while (*str >= '0' && *str <= '9')
	{
		result += (*str++ - '0') / divisor;
		divisor *= 10.0;
	}
	fatal_if(m, *str != '\0', "Not a number: %s", str);
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
	float	epsilon;

	epsilon = 0.001f;
	return ((v.x >= lower - epsilon) && (v.x <= upper + epsilon)
		&& (v.y >= lower - epsilon) && (v.y <= upper + epsilon) && (v.z >= lower
			- epsilon) && (v.z <= upper + epsilon));
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

	split = ft_split(str, ',');
	fatal_if(m, split == NULL, "Memory allocation failure\n");
	fatal_if(m, array_len(split) != 3,
		"Vector format must be 'x,y,z' with 3 values\n");
	*out = vec3(ft_atof(m, split[0]), ft_atof(m, split[1]), ft_atof(m,
				split[2]));
	free_array(split);
	if (limits[0] != limits[1])
		fatal_if(m, !in_range3(*out, limits[0], limits[1]),
			"Vector values must be between %d and %d\n", (int)limits[0],
			(int)limits[1]);
}
