#include "minirt.h"

int	array_len(char **array)
{
	int	i;

	if (!array)
		return (0);
	i = 0;
	while (array[i])
		i++;
	return (i);
}

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

void	cleanup_scene(t_scene *sc)
{
	if (!sc)
		return ;
	if (sc->objects)
	{
		free(sc->objects);
		sc->objects = NULL;
	}
	sc->object_count = 0;
	sc->camera_fov = 0;
	sc->camera_pos = vec3(0, 0, 0);
	sc->camera_dir = vec3(0, 0, 0);
	sc->ambient = vec3(0, 0, 0);
}

void	free_array(char **array)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i] != NULL)
		free(array[i++]);
	free(array);
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
bool	parse_vec3(char *str, t_vec3 *out, float min, float max)
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

t_object_type	parse_type(char *s)
{
	if (ft_strcmp(s, "L") == 0)
		return (OBJECT_LIGHT);
	if (ft_strcmp(s, "sp") == 0)
		return (OBJECT_SPHERE);
	if (ft_strcmp(s, "pl") == 0)
		return (OBJECT_PLANE);
	if (ft_strcmp(s, "cy") == 0)
		return (OBJECT_CYLINDER);
	if (ft_strcmp(s, "cn") == 0)
		return (OBJECT_CONE);
	return (-1);
}

int	objects_malloc_manager(t_scene *sc)
{
	static int	is_malloced = 0;
	static int	capacity = 64;
	t_object	*new_objects;

	if (!is_malloced)
	{
		sc->objects = malloc(capacity * sizeof(t_object));
		if (!sc->objects)
			return (1);
		is_malloced = 1;
	}
	else if (sc->object_count >= (size_t)capacity)
	{
		capacity *= 2;
		new_objects = malloc(capacity * sizeof(t_object));
		if (!new_objects)
			return (1);
		memcpy(new_objects, sc->objects, sc->object_count * sizeof(t_object));
		free(sc->objects);
		sc->objects = new_objects;
	}
	return (0);
}

bool	parse_shape(char **line, t_scene *sc)
{
	(void)line;
	(void)sc;
	return (false);
}

bool	parse_ambient_light(char **line, t_scene *sc)
{
	static bool	ambient_exists = false;
	double		ratio;

	if (ambient_exists || array_len(line) != 3)
		return (true);
	ratio = ft_atof(line[1]);
	if (!parse_vec3(line[2], &sc->ambient, 0, 255) || !mrt_assert(ratio >= 0.0f
			&& ratio <= 1.0f,
			"Ambient light ratio must be between 0.0 and 1.0\n"))
		return (true);
	sc->ambient = vec3_scale(vec3_scale(sc->ambient, 1.0 / 255.0), ratio);
	ambient_exists = true;
	return (false);
}

bool	parse_point_light(char **line, t_scene *sc)
{
	t_object	*obj;
	double		brightness;

	if (array_len(line) != 4 || objects_malloc_manager(sc))
		return (true);
	brightness = ft_atof(line[2]);
	obj = &sc->objects[sc->object_count];
	obj->type = OBJECT_LIGHT;
	if (!parse_vec3(line[1], &obj->pos, 0, 0) || !parse_vec3(line[3],
			&obj->color, 0, 255) || !mrt_assert(brightness >= 0.0f
			&& brightness <= 1.0f,
			"Light brightness must be between 0.0 and 1.0\n"))
		return (true);
	obj->color = vec3_scale(vec3_scale(obj->color, 1.0 / 255.0), brightness);
	sc->object_count++;
	printf("Point light: (%f, %f, %f) (%f, %f, %f)\n", obj->pos.x, obj->pos.y,
		obj->pos.z, obj->color.x, obj->color.y, obj->color.z);
	return (false);
}

bool	parse_light(char **line, t_scene *sc)
{
	if (ft_strcmp(line[0], "A") == 0)
		return (parse_ambient_light(line, sc));
	else if (ft_strcmp(line[0], "L") == 0)
		return (parse_point_light(line, sc));
	return (true);
}

// Then in read_map_into_scene, when errors occur:
/*
 * the command is going through array of strings that was passed to it
 * it will take the positions, split them, check if the length is 3,
	error if it is not
 * convert it to floats, check if it is within the limits
 * error if it is not
 * do the same with fov but without spliting
 *
 */

bool	parse_camera(char **line, t_scene *sc)
{
	static int	exist = 0;

	if (exist == 1 || array_len(line) != 4)
		return (true);
	sc->camera_fov = ft_atof(line[3]);
	if (!parse_vec3(line[1], &sc->camera_pos, 0, 0) || !parse_vec3(line[2],
			&sc->camera_dir, -1, 1))
		return (true);
	exist = 1;
	return (!mrt_assert(fabsf(vec3_length(sc->camera_dir) - 1.0f) < 0.001f,
			"Camera direction components must be between -1 and 1")
		|| !mrt_assert(sc->camera_fov >= 0.0f && sc->camera_fov <= 180.0f,
			"Camera FOV must be between 0 and 180 degrees"));
}

int	process_line(t_scene *sc, char *buff)
{
	char	**line;
	bool	error;

	line = ft_split(buff, ' ');
	if (line == NULL || !line[0])
	{
		free_array(line);
		return (1);
	}
	if (ft_strcmp(line[0], "C") == 0)
	{
		error = parse_camera(line, sc);
	}
	else if (ft_strcmp(line[0], "L") == 0 || ft_strcmp(line[0], "A") == 0)
		error = parse_light(line, sc);
	else if (ft_strcmp(line[0], "sp") == 0 || ft_strcmp(line[0], "pl") == 0
		|| ft_strcmp(line[0], "cy") == 0 || ft_strcmp(line[0], "cn") == 0)
		error = parse_shape(line, sc);
	else
		error = !mrt_assert(false, "Not a valid object type");
	free_array(line);
	return (error);
}

bool	read_map_into_scene(int fd, t_scene *sc)
{
	char	*buff;

	while (1)
	{
		buff = get_next_line(fd);
		if (!buff)
			break ;
		if (!ft_str_is_whitespace(buff))
		{
			if (process_line(sc, buff))
			{
				free(buff);
				cleanup_scene(sc);
				return (false);
			}
		}
		free(buff);
	}
	return (sc->object_count > 0);
}

static bool	ends_with_rt(const char *path)
{
	const char	*dot = ft_strrchr(path, '.');

	if (!dot)
		return (false);
	if (dot != NULL && *(dot + 1) != '\0')
		return (ft_strcmp(dot, ".rt") == 0);
	return (false);
}

bool	validate_input_and_parse_map(int ac, char **av, t_scene *scene)
{
	int	fd;

	if (!mrt_assert(ac == 2 && ends_with_rt(av[1]),
			"usage: ./miniRT <path_to_the_file.rt>\n"))
		return (false);
	fd = open(av[1], O_RDONLY);
	if (fd < 0)
	{
		ft_fprintf(2, "Error\nCould not open file %s\n", av[1]);
		return (false);
	}
	read_map_into_scene(fd, scene);
	get_next_line(-1);
	close(fd);
	return (scene->object_count > 0);
}
