#include "minirt.h"

void	free_array(char **array, bool is_heap)
{
	int	i;

	i = 0;
	if (!array)
		return ;
	while (array[i] != NULL)
		free(array[i++]);
	if (is_heap)
		free(array);
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

int array_len(char **array)
{
	int i = 0;
	while (array[i])
		i++;
	return (i);
}

void			parse_shape(char **line, t_scene *sc);
void			parse_light(char **line, t_scene *sc);
void	parse_camera(char **line, t_scene *sc, bool *error)
{
	static int	exist = 0;
	char		**pos;
	char		**dir;

	if (exist == 1)
	{
		*error = true;
		return ;
	}
	pos = ft_split(line[1], ',');
	dir = ft_split(line[2], ',');
	sc->camera_fov = ft_atof(line[3]);
	*error = !(mrt_assert_array(pos, 3, 0, 0) || mrt_assert_array(dir, 3, -1, 1)); sc->
	free_array(position, 1);
	free_array(direction, 1);
	exist = 1;
}

int	process_map(t_scene *sc, char **buff)
{
	const char	**original_buff = (const char **)buff;
	char		**line;
	static bool	*error = false;

	while (*buff)
	{
		line = ft_split(*buff++, ' ');
		if (line == NULL)
		{
			free_array((char **)original_buff, 0);
			return (1);
		}
		if (ft_strcmp(line[0], "sp") == 0 || ft_strcmp(line[0], "pl") == 0
			|| ft_strcmp(line[0], "cy") == 0 || ft_strcmp(line[0], "cn") == 0)
			parse_shape(line, sc, error);
		else if (ft_strcmp(line[0], "L") == 0 || ft_strcmp(line[0], "A") == 0)
			parse_light(line, sc, error);
		else if (ft_strcmp(line[0], "C") == 0)
			parse_camera(line, sc, error);
		free_array(line, 1);
	}
	free_array((char **)original_buff, 0);
	return (0);
}

int	read_map_into_scene(int fd, t_scene *sc)
{
	char	*buff[OBJECT_MAX + 1];
	int		i;

	i = 0;
	while (i < OBJECT_MAX)
	{
		buff[i] = get_next_line(fd);
		if (buff[i] == NULL)
			break ;
		else if (ft_str_is_whitespace(buff[i]))
			continue ;
		i++;
	}
	buff[i] = NULL;
	while (get_next_line(fd) != NULL)
		;
	sc->object_count = i;
	if (sc->object_count == 0)
		return (1);
	if (process_map(sc, buff))
		return (1);
	return (0);
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
	close(fd);
	return (true);
}
