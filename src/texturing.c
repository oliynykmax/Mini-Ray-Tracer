#include "minirt.h"

static float	cloudy(t_vec3 x, float ampl, float freq)
{
	float	sum = 0.0f;
	t_vec3	n = vec3(1.0, 0.0, 0.0f);
	const float t = radians(180.0) * (3.0 - sqrt(5.0));

	for (int i = 0; i < 30; i++) {
		float p = freq * dot3(x, n);
		float y = fract(p);
		float v = 4.0f * (fabsf(y) * y - y);
		v = 1.0f - 2.0f * fabsf(v);
		sum += ampl * v;
		freq *= 1.25;
		ampl *= 0.9;
		n = vec3(cosf(t) * n.x - sinf(t) * n.y, sin(t) * n.x + cosf(t) * n.y, 0.0f);
	}
	return (saturate(sum * 0.1f + 1.5f) * 0.9f + 0.1f);
}

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
#if 1
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

static float	texture_marble(float u, float v)
{
	return (cloudy(vec3(u, v, 0.0f), 4.0f, 0.5f));
}

float	get_texture(t_texture texture, float u, float v)
{
	static const t_texture_function	functions[] = {
		texture_none,
		texture_checked,
		texture_zigzag,
		texture_polkadot,
		texture_marble,
	};

	return (functions[texture](u, v));
}
