#include "minirt.h"

float	plane_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	(void) o;
	return (-ro.y / rd.y);
}

t_vec3	plane_normal(t_object *o, t_vec3 p)
{
	(void) p;
	return (o->normal);
}

t_vec3	plane_texcoord(t_object *o, t_vec3 p)
{
	(void) o;
	return (vec3(p.x, p.z, 0.0f));
}
