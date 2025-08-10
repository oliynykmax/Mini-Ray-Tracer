#include "minirt.h"

void	objects_malloc_manager(t_parse *m)
{
	static int	is_malloced = 0;
	static int	capacity = 64;
	t_object	*new_objects;

	m->sc->object_count++;
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

static void	read_map_into_scene(t_parse *map)
{
	char	*nl;

	while (1)
	{
		map->buff = get_next_line(map->fd);
		if (!map->buff)
			break ;
		if (!ft_str_is_whitespace(map->buff))
		{
			nl = ft_strrchr(map->buff, '\n');
			if (nl != NULL)
				*nl = '\0';
			map->line = ft_split(map->buff, ' ');
			mrt_assert(map, map->line, "Allocation failure during split\n");
			if (map->line[0][0] != '#')
				parse_type(map);
			free_array(map->line);
			map->line = NULL;
		}
		free(map->buff);
	}
}

void	validate_input_and_parse_map(int ac, char **av, t_parse *m)
{
	mrt_assert(m, ac == 2, "usage: ./miniRT <path_to_the_file.rt>\n");
	mrt_assert(m, !ft_strcmp(ft_strrchr(av[1], '.'), ".rt"), "Not .rt file\n");
	m->fd = open(av[1], O_RDONLY);
	mrt_assert(m, m->fd >= 0, "Error\nCould not open file %s\n", av[1]);
	read_map_into_scene(m);
	get_next_line(-1);
	close(m->fd);
	mrt_assert(m, m->sc->object_count > 0, "Not enough objects provided\n");
}
