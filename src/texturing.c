#include "minirt.h"

static float	zigzag(float u, float v)
{
	const float	a = fabsf(fract(u * 3.0f + 0) * 2.0f - 1.0f);
	const float	b = fabsf(fract(v * 6.0f + a) * 2.0f - 1.0f);

	return (b > 0.5f);
}

static float	polkadot(float u, float v)
{
	const float	r = 0.25f;

	u = fract(u * 10.0f) - 0.5f;
	v = fract(v * 10.0f) - 0.5f;
	return (u * u + v * v > r * r);
}

static float	checkboard(float u, float v)
{
	return (zigzag(u, v));
	return (polkadot(u, v));
	u = floorf(u * 10.0f);
	v = floorf(v * 10.0f);
	return (fract((u + v) * 0.5f));
}

t_vec3	apply_texture(t_object *object, t_vec3 point)
{
	const t_vec3	texcoord = object_texcoord(object, point);
	const float		value = 0.1f + 0.9f * checkboard(texcoord.x, texcoord.y);

	return (vec3(value, value, value));
}
