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

static float	smoothstep(float edge0, float edge1, float t)
{
	t = saturate((t - edge0) / (edge1 - edge0));
	return (t * t * (3.0f - 2.0f * t));
}

static float	texture_zigzag(float u, float v)
{
	return saturate(fabsf((v - 0.5f) * 4.0f - sinf(u * 2.0f * M_PI)));
	const float	a = fabsf(fract(u * 3.0f + 0) * 2.0f - 1.0f);
	const float	b = fabsf(fract(v * 6.0f + a) * 2.0f - 1.0f);

	return smoothstep(0.2f, 0.8f, b);
	return (b * b * (3.0 - 2.0 * b));
	return (b > 0.5f);
}

static float	texture_polkadot(float u, float v)
{
	const float	r = 0.5f;

	u = fract(u * 10.0f) - 0.5f;
	v = fract(v * 10.0f) - 0.5f;
	float b = 1.0f - saturate(sqrtf(u * u + v * v) / r);
	return (b * b * (3.0 - 2.0 * b));
	// return (u * u + v * v > r * r);
}

float	get_texture(t_texture texture, float u, float v)
{
	static const t_texture_function	functions[] = {
		texture_none,
		texture_checked,
		texture_zigzag,
		texture_polkadot,
	};

	return (functions[texture](u, v));
}
