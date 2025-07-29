#include "minirt.h"

float	object_distance(t_object *object, t_vec3 ro, t_vec3 rd)
{
	static const t_distance_function	functions[] = {
		plane_distance,
		sphere_distance,
		cylinder_distance,
		NULL,
		sphere_distance,
	};

	ro = quat_rotate_vec3(quat_inverse(object->rot), vec3_sub(ro, object->pos));
	rd = quat_rotate_vec3(quat_inverse(object->rot), rd);
	return (functions[object->type](object, ro, rd));
}

t_vec3	object_normal(t_object *obj, t_vec3 point)
{
	static const t_normal_function	functions[] = {
		plane_normal,
		sphere_normal,
		cylinder_normal,
	};

	point = quat_rotate_vec3(quat_inverse(obj->rot), vec3_sub(point, obj->pos));
	return (quat_rotate_vec3(obj->rot, functions[obj->type](obj, point)));
}

t_vec3	object_texcoord(t_object *obj, t_vec3 point)
{
	static const t_normal_function	functions[] = {
		plane_texcoord,
		sphere_texcoord,
		cylinder_texcoord,
	};

	point = quat_rotate_vec3(quat_inverse(obj->rot), vec3_sub(point, obj->pos));
	return (functions[obj->type](obj, point));
}
