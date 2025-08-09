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

static int	process_line(t_scene *sc, char *buff)
{
	char	**line;
	bool	error;
	char	*nl;

	nl = ft_strrchr(buff, '\n');
	if (nl != NULL)
		*nl = '\0';
	line = ft_split(buff, ' ');
	if (!line)
		return (!mrt_assert(false, "Allocation failure during split\n"));
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

static void	read_map_into_scene(int fd, t_scene *sc)
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
				return ;
			}
		}
		free(buff);
	}
}

static bool	ends_with_rt(const char *p)
{
	const char *d = ft_strrchr(p, '.');
	return (d && !ft_strcmp(d, ".rt"));
}


bool	validate_input_and_parse_map(int ac, char **av, t_scene *sc)
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
	read_map_into_scene(fd, sc);
	get_next_line(-1);
	close(fd);
	return (mrt_assert(sc->object_count > 0, "not enough objects provided"));
}
