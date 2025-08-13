#include "minirt.h"

static float	texture_none(float u, float v)
{
	(void) u, (void) v;
	return (1.0f);
}

static float	texture_checked(float u, float v)
{
	u = floorf(u * 10.0f);
	v = floorf(v * 10.0f);
	return (fract((u + v) * 0.5f));
}

static float	texture_zigzag(float u, float v)
{
	const float	a = fabsf(fract(u * 3.0f + 0) * 2.0f - 1.0f);
	const float	b = fabsf(fract(v * 6.0f + a) * 2.0f - 1.0f);

	return (b * b * (3.0 - 2.0 * b));
}

static float	texture_polkadot(float u, float v)
{
#if 0
	const float	r = 0.5f;
	const float	x = fract(u * 2.0f) - 0.5f;
	const float	y = fract(v * 2.0f) - 0.5f;
	const float	b = 1.0f - saturate(sqrtf(x * x + y * y) / r);

	return (b * b * (3.0 - 2.0 * b));
#else
	const float	a = fabsf(fract(u * 3.0f + 0) * 2.0f - 1.0f);
	float	b = fabsf(fract(v * 6.0f + a) * 2.0f - 1.0f);

	b *= b;
	return (b * b * (3.0 - 2.0 * b));
#endif
}

static float	texture_bump(float u, float v)
{
	return (texture_polkadot(u, v));
}

float	get_texture(t_texture texture, float u, float v)
{
	static const t_texture_function	functions[] = {
		texture_none,
		texture_checked,
		texture_zigzag,
		texture_polkadot,
		texture_bump,
	};

	return (functions[texture](u, v));
}
