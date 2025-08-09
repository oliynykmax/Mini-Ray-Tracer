#include "minirt.h"

void	objects_malloc_manager(t_parse *m)
{
	static int	is_malloced = 0;
	static int	capacity = 64;
	t_object	*new_objects;

	if (!is_malloced)
	{
		m->sc->objects = malloc(capacity * sizeof(t_object));
		mrt_assert(m, m->sc->objects != NULL, "Allocation failure\n");
		is_malloced = 1;
		ft_bzero(m->sc->objects, capacity * sizeof(t_object));
		return ;
	}
	if (m->sc->object_count < (size_t)capacity)
		return ;
	capacity *= 2;
	new_objects = malloc(capacity * sizeof(t_object));
	mrt_assert(m, new_objects != NULL, "Allocation failure\n");
	memcpy(new_objects, m->sc->objects, m->sc->object_count * sizeof(t_object));
	free(m->sc->objects);
	m->sc->objects = new_objects;
	ft_bzero(m->sc->objects + m->sc->object_count, (capacity
			- (int)m->sc->object_count) * sizeof(t_object));
}

static void	process_line(t_parse *map)
{
	char	*nl;

	nl = ft_strrchr(map->buff, '\n');
	if (nl != NULL)
		*nl = '\0';
	map->line = ft_split(map->buff, ' ');
	mrt_assert(map, map->line, "Allocation failure during split\n");
	if (ft_strcmp(map->line[0], "C") == 0)
		parse_camera(map);
	else if (ft_strcmp(map->line[0], "L") == 0 || ft_strcmp(map->line[0],
			"A") == 0 || ft_strcmp(map->line[0], "sp") == 0
		|| ft_strcmp(map->line[0], "pl") == 0 || ft_strcmp(map->line[0],
			"cy") == 0 || ft_strcmp(map->line[0], "pa") == 0)
		parse_object(map);
	else
		mrt_assert(map, false, "Not a valid object type");
	free_array(map->line);
}

static void	read_map_into_scene(t_parse *map)
{
	while (1)
	{
		map->buff = get_next_line(map->fd);
		if (!map->buff)
			break ;
		if (!ft_str_is_whitespace(map->buff))
			process_line(map);
		free(map->buff);
	}
}

static bool	ends_with_rt(const char *p)
{
	const char	*d = ft_strrchr(p, '.');

	return (d && !ft_strcmp(d, ".rt"));
}

void	validate_input_and_parse_map(int ac, char **av, t_scene *sc)
{
	t_parse	map;

	map.sc = sc;
	mrt_assert(&map, ac == 2 && ends_with_rt(av[1]),
		"usage: ./miniRT <path_to_the_file.rt>\n");
	map.fd = open(av[1], O_RDONLY);
	mrt_assert(&map, map.fd < 0, "Error\nCould not open file %s\n", av[1]);
	read_map_into_scene(&map);
	get_next_line(-1);
	close(map.fd);
	mrt_assert(&map, sc->object_count > 0, "not enough objects provided");
}
