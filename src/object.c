#include "minirt.h"

float	object_distance(t_object *object, t_vec3 ro, t_vec3 rd)
{
	static const t_distance_function	functions[] = {
		plane_distance,
		sphere_distance,
		cylinder_distance,
		para_distance,
		sphere_distance,
	};

	ro = quat_rotate_vec3(quat_inverse(object->rot), sub3(ro, object->pos));
	rd = quat_rotate_vec3(quat_inverse(object->rot), rd);
	return (functions[object->type](object, ro, rd));
}

void	object_params(t_object *o, t_shading *s)
{
	const t_vec3					p = s->point;
	static const t_params_function	functions[] = {
		plane_params,
		sphere_params,
		cylinder_params,
		para_params,
	};

	s->point = quat_rotate_vec3(quat_inverse(o->rot), sub3(p, o->pos));
	functions[o->type](o, s);
	s->normal = quat_rotate_vec3(o->rot, s->normal);
	s->tangent = quat_rotate_vec3(o->rot, s->tangent);
	s->bitangent = cross3(s->normal, s->tangent);
	s->point = p;
}
