#include "minirt.h"

bool	ends_with_rt(const char *path)
{
	const char	*dot = ft_strrchr(path, '.');

	if (!dot)
		return (false);
	if (dot != NULL && *(dot + 1) != '\0')
		return (ft_strcmp(dot, ".rt") == 0);
	return (false);
}

t_scene	*validate_input_and_parse_map(int ac, char **av)
{
	int		fd;
	t_scene	*scene;

	scene = NULL;
	if (ac != 2 || !ends_with_rt(av[1]))
		ft_fprintf(2, "Invalid input,"
			"usage:\n./miniRT <path_to_the_file.rt>\n");
	else
	{
		fd = open(av[1], O_RDONLY);
		close(fd);
	}
	return (scene);
}
