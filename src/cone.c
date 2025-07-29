#include "minirt.h"
#include <math.h>

// Calculates the intersection distance of a ray with the body of a single cone.
static float body_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
    const float apex_y = +0.5f * o->height;
    const float k = o->radius / o->height;
    const float k2 = k * k;

    // Shift y so apex is at y = 0
    float oy = ro.y - apex_y;
    float dy = rd.y;

    const float a = rd.x * rd.x + rd.z * rd.z - k2 * dy * dy;
    const float b = 2.0f * (ro.x * rd.x + ro.z * rd.z - k2 * oy * dy);
    const float c = ro.x * ro.x + ro.z * ro.z - k2 * oy * oy;

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f)
        return 1e9f; // No real roots

    float sqrt_discriminant = sqrtf(discriminant);
    float t0 = (-b - sqrt_discriminant) / (2.0f * a);
    float t1 = (-b + sqrt_discriminant) / (2.0f * a);

    // Check both solutions for valid range
    float y0 = ro.y + t0 * rd.y;
    float y1 = ro.y + t1 * rd.y;

    bool valid_t0 = (y0 >= -o->height * 0.5f) && (y0 <= apex_y);
    bool valid_t1 = (y1 >= -o->height * 0.5f) && (y1 <= apex_y);

    if (valid_t0 && valid_t1)
        return fminf(t0, t1);
    else if (valid_t0)
        return t0;
    else if (valid_t1)
        return t1;
    else
        return 1e9f;
}


// Calculates the intersection distance of a ray with the base cap of the cone.
static float	disk_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	const float	h = -o->height * 0.5f;
	if (fabsf(rd.y) < 1e-6f)
		return (1e9f);
	const float	t = (h - ro.y) / rd.y;
	if (t < 1e-6f)
		return (1e9f);
	const float	x = ro.x + t * rd.x;
	const float	z = ro.z + t * rd.z;
	if (x * x + z * z > o->radius * o->radius)
		return (1e9f);

	return (t);
}


// Calculates the closest intersection distance of a ray with a cone.
float	cone_distance(t_object *o, t_vec3 ro, t_vec3 rd)
{
	const float	body_dist = body_distance(o, ro, rd);
	const float	base_dist = disk_distance(o, ro, rd);

	return (fminf(body_dist, base_dist));
}

// Calculates the normal vector at a point 'p' on the cone's surface.
t_vec3	cone_normal(t_object *o, t_vec3 p)
{
	// Check if the point is on the base of the cone (outward normal is -y).
	if (fabsf(p.y + o->height * 0.5f) < 1e-6f)
		return (vec3(0.0f, -1.0f, 0.0f));

	// The point is on the body of the cone.
	const float	m = o->radius / o->height;
	const float	r = sqrtf(p.x * p.x + p.z * p.z);

	// The y-component of the normal depends on the slope.
	return (vec3_normalize(vec3(p.x, r * m, p.z)));
}

// Calculates the texture coordinates at a point 'p' on the cone's surface.
t_vec3	cone_texcoord(t_object *o, t_vec3 p)
{
	float u;
	float v;

	// Check if the point is on the base for planar mapping.
	if (fabsf(p.y + o->height * 0.5f) < 1e-6f)
	{
		u = (p.x / o->radius + 1.0f) * 0.5f;
		v = (p.z / o->radius + 1.0f) * 0.5f;
	}
	else // Otherwise, use cylindrical mapping for the body.
	{
		u = 1.0f - (atan2f(p.z, p.x) / (2.0f * M_PI) + 0.5f);
		v = (p.y + o->height * 0.5f) / o->height;
	}
	return (vec3(u, v, 0.0f));
}
