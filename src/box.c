#include "minirt.h"

static t_vec3	box_entry_exit(t_vec3 h, t_vec3 ro, t_vec3 rd)
{
	const t_vec3	m = vec3(1.0f / (rd.x + (rd.x == 0.0f) * 1e-6f), 1.0f
			/ (rd.y + (rd.y == 0.0f) * 1e-6f), 1.0f / (rd.z + (rd.z == 0.0f)
				* 1e-6f));
	const t_vec3	n = mul3(m, ro);
	const t_vec3	k = mul3(vec3(fabsf(m.x), fabsf(m.y), fabsf(m.z)), h);
	const t_vec3	t1 = sub3(scale3(n, -1.0f), k);
	const t_vec3	t2 = add3(scale3(n, -1.0f), k);

	return (vec3(fmaxf(fmaxf(t1.x, t1.y), t1.z), fminf(fminf(t2.x, t2.y), t2.z),
			0.0f));
}

float	box_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	t_vec3	h;
	float	tn;
	float	tf;
	t_vec3	tt;

	h = scale3(o->size, 0.5f);
	tt = box_entry_exit(h, ro, rd);
	tn = tt.x;
	tf = tt.y;
	if (tn > tf || tf < 0.0f)
		return (1e9f);
	if (tn > 0.0f)
		return (tn);
	return (tf);
}

static t_vec3	box_face_normal(t_vec3 p, t_vec3 h)
{
	t_vec3	q;
	t_vec3	d;

	q = vec3(fabsf(p.x), fabsf(p.y), fabsf(p.z));
	d = sub3(q, h);
	if (d.x > d.y && d.x > d.z)
		return (vec3(copysignf(1.0f, p.x), 0.0f, 0.0f));
	if (d.y > d.z)
		return (vec3(0.0f, copysignf(1.0f, p.y), 0.0f));
	return (vec3(0.0f, 0.0f, copysignf(1.0f, p.z)));
}

static t_vec3	box_face_uv(t_object *o, t_shading *s, t_vec3 p, t_vec3 h)
{
	float	u;
	float	v;

	if (fabsf(s->normal.x) > 0.5f)
	{
		s->tangent = vec3(0.0f, 0.0f, 1.0f);
		u = (p.z + h.z) / o->size.z;
		v = (p.y + h.y) / o->size.y;
	}
	else if (fabsf(s->normal.y) > 0.5f)
	{
		s->tangent = vec3(1.0f, 0.0f, 0.0f);
		u = (p.x + h.x) / o->size.x;
		v = (p.z + h.z) / o->size.z;
	}
	else
	{
		s->tangent = vec3(1.0f, 0.0f, 0.0f);
		u = (p.x + h.x) / o->size.x;
		v = (p.y + h.y) / o->size.y;
	}
	return (vec3(clamp(u, 0.0f, 1.0f), clamp(v, 0.0f, 1.0f), 0.0f));
}

void	box_params(t_object *o, t_shading *s)
{
	t_vec3	p;
	t_vec3	h;

	p = s->point;
	h = scale3(o->size, 0.5f);
	s->normal = box_face_normal(p, h);
	s->texcoord = box_face_uv(o, s, p, h);
}
