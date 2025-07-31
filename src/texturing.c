#include "minirt.h"

static float	checkboard(float u, float v)
{
	u = floorf(u * 10.0f);
	v = floorf(v * 10.0f);
	return (0.1f + 0.9f * fract((u + v) * 0.5f));
}

t_vec3	apply_texture(t_object *object, t_vec3 point)
{
	const t_vec3	texcoord = object_texcoord(object, point);
	const float		value = checkboard(texcoord.x, texcoord.y);

	return (vec3(value, value, value));
}
