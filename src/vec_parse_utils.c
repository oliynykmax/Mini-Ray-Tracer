#include "minirt.h"

double	ft_atof(const char *str)
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
		if (*str++ == '-')
			sign = -1;
	while (*str >= '0' && *str <= '9')
		result = result * 10.0 + (*str++ - '0');
	if (*str == '.')
	{
		str++;
		while (*str >= '0' && *str <= '9')
		{
			result += (*str - '0') / divisor;
			divisor *= 10.0;
			str++;
		}
	}
	return (result * sign);
}

/*
 * Checks if all components of a vector are within given boundaries
 * @param v The vector to check
 * @param lower Lower boundary (inclusive)
 * @param upper Upper boundary (inclusive)
 * @return true if all components are within boundaries, false otherwise
 */
bool	vec3_in_range(t_vec3 v, float lower, float upper)
{
	float	epsilon;

	epsilon = 0.001f;
	return ((v.x >= lower - epsilon) && (v.x <= upper + epsilon)
		&& (v.y >= lower - epsilon) && (v.y <= upper + epsilon) && (v.z >= lower
			- epsilon) && (v.z <= upper + epsilon));
}

/*
 * Parses a string "x,y,z" into a vec3 and checks if values are within range
 * @param str The string to parse in format "x,y,z"
 * @param out Pointer to store the parsed vector
 * @param min Minimum allowed value (ignored if min == max)
 * @param max Maximum allowed value (ignored if min == max)
 * @return true on success, false on failure
 */
bool	parse3(char *str, t_vec3 *out, float min, float max)
{
	char	**split;

	split = ft_split(str, ',');
	if (!mrt_assert(split != NULL, "Memory allocation failure\n"))
	{
		free_array(split);
		return (false);
	}
	if (!mrt_assert(array_len(split) == 3,
			"Vector format must be 'x,y,z' with 3 values\n"))
	{
		free_array(split);
		return (false);
	}
	*out = vec3(ft_atof(split[0]), ft_atof(split[1]), ft_atof(split[2]));
	free_array(split);
	if (min != max && !mrt_assert(vec3_in_range(*out, min, max),
			"Vector values must be between %d and %d\n", (int)min, (int)max))
		return (false);
	return (true);
}
