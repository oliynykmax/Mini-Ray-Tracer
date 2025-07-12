#include "minirt.h"

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

void	process_map(t_scene *sc, char **buff)
{
	size_t	i;

	i = 0;
	(void)buff;
	while (i < sc->object_count)
	{
		printf("%s", buff[i]);
		// parse shape
		// parse light
		// parse camera
		i++;
	}
}

void	read_map_into_scene(int fd, t_scene *sc)
{
	char	*buff[OBJECT_MAX + 1];
	int		i;

	i = 0;
	while (i < OBJECT_MAX)
	{
		buff[i] = get_next_line(fd);
		if (buff[i] == NULL)
			break ;
		if (ft_str_is_whitespace(buff[i]))
			continue ;
		i++;
	}
	buff[i] = NULL;
	while (get_next_line(fd) != NULL)
		;
	sc->object_count = i;
	if (sc->object_count == 0)
		return ;
	process_map(sc, buff);
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
		ft_fprintf(2, "Error: Could not open file %s\n", av[1]);
		return (false);
	}
	read_map_into_scene(fd, scene);
	close(fd);
	return (true);
}
