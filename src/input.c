#include "minirt.h"

/* Inline fatal_if implementation (previously mrt_assert) */
static void	cleanup_parsing(t_parse *ctx)
{
	size_t	i = 0;

	get_next_line(-1);
	if (ctx->fd >= 0)
		close(ctx->fd);
	if (ctx->buff)
		free(ctx->buff);
	if (ctx->line)
		free_array(ctx->line);
	if (ctx->temp_split)
		free_array(ctx->temp_split);
	if (ctx->sc && ctx->sc->objects)
	{
		while (i < ctx->sc->object_count)
		{
			free_object_textures(&ctx->sc->objects[i]);
			++i;
		}
		free(ctx->sc->objects);
		ctx->sc->objects = NULL;
		ctx->sc->object_count = 0;
	}
}

void	fatal_if(t_parse *ctx, bool condition, char *format, ...)
{
	const int	errno_value = errno;
	va_list		args;

	if (!condition)
		return ;
	ft_fprintf(2, "Error\n");
	if (format != NULL || errno_value != 0)
	{
		if (errno_value != 0)
		{
			ft_putstr_fd(strerror(errno_value), STDERR_FILENO);
			ft_putstr_fd(": ", STDERR_FILENO);
			write(STDERR_FILENO, "\n", 1);
		}
		if (format)
		{
			va_start(args, format);
			printf_fd(2, format, &args);
			va_end(args);
		}
	}
	cleanup_parsing(ctx);
	exit(EXIT_FAILURE);
}

/* Dynamic array push-back for scene objects.
 * - Initial static capacity = 64
 * - Doubles on growth.
 * - Exits via fatal_if on allocation failure.
 * - Returns zeroed slot.
 */
t_object	*object_new(t_parse *map)
{
	static size_t	capacity = 64;
	t_object		*slot;
	t_object		*new_storage;

	if (map->sc->objects == NULL)
	{
		map->sc->objects = malloc(capacity * sizeof(t_object));
		fatal_if(map, map->sc->objects == NULL, "Allocation failure\n");
		ft_memset(map->sc->objects, 0, capacity * sizeof(t_object));
	}
	else if (map->sc->object_count == capacity)
	{
		capacity *= 2;
		new_storage = malloc(capacity * sizeof(t_object));
		fatal_if(map, new_storage == NULL, "Allocation failure\n");
		ft_memcpy(new_storage, map->sc->objects,
			map->sc->object_count * sizeof(t_object));
		free(map->sc->objects);
		ft_memset(new_storage + map->sc->object_count, 0,
			(capacity - map->sc->object_count) * sizeof(t_object));
		map->sc->objects = new_storage;
	}
	slot = &map->sc->objects[map->sc->object_count++];
	ft_memset(slot, 0, sizeof(*slot));
	return (slot);
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
			fatal_if(map, map->line == NULL, "Allocation fail during split\n");
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
	fatal_if(m, ac != 2, "usage: ./miniRT <path_to_the_file.rt>\n");
	fatal_if(m, ft_strcmp(ft_strrchr(av[1], '.'), ".rt"), "Not .rt file\n");
	m->fd = open(av[1], O_RDONLY);
	fatal_if(m, m->fd < 0, "Error\nCould not open file %s\n", av[1]);
	read_map_into_scene(m);
	get_next_line(-1);
	close(m->fd);
	fatal_if(m, m->camera_exists != true, "Camera doesn't exist\n");
	fatal_if(m, m->ambient_exists != true, "Ambient doesn't exist\n");
	fatal_if(m, m->num_of_lights == 0, "Not enough lights provided\n");
	fatal_if(m, m->num_of_lights == m->sc->object_count,
		"Not enough objects\n");
	fatal_if(m, m->sc->object_count <= 0, "Not enough objects provided\n");
}
