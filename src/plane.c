#include "minirt.h"

float	plane_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	(void) o;
	return (-ro.y / rd.y);
}

void	plane_params(t_object *o, t_shading *s)
{
	(void) o;
	s->normal = vec3(0.0f, 1.0f, 0.0f);
	s->texcoord = vec3(-s->point.x, s->point.z, 0.0f);
}
