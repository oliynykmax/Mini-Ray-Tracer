#include "minirt.h"

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
		error = parse_camera(line, sc);
	else if (ft_strcmp(line[0], "L") == 0 || ft_strcmp(line[0], "A") == 0
		|| ft_strcmp(line[0], "sp") == 0 || ft_strcmp(line[0], "pl") == 0
		|| ft_strcmp(line[0], "cy") == 0 || ft_strcmp(line[0], "pa") == 0)
		error = parse_object(line, sc);
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
