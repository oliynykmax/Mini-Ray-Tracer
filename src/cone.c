#include "minirt.h"
#include <math.h>

#include "minirt.h"
#include <math.h>

// Robust quadratic solver for cone intersections
static float solve_quadratic_sp(float a, float b, float c, float *t0, float *t1) {
    const float d = b * b - 4.0f * a * c;
    if (d < 0.0f)
        return (0.0f);

    const float sqrt_d = sqrtf(d);
    float q;
    if (b < 0.0f)
        q = -0.5f * (b - sqrt_d);
    else
        q = -0.5f * (b + sqrt_d);

    *t0 = q / a;
    *t1 = c / q;

    if (*t0 > *t1) {
        float tmp = *t0;
        *t0 = *t1;
        *t1 = tmp;
    }
    return (d);
}

// Calculates intersection with cone body (handles rays inside cone)
static float body_distance(t_object *o, t_vec3 ro, t_vec3 rd) {
    const float apex_y = +0.5f * o->height;
    const float k = o->radius / o->height;
    const float k2 = k * k;

    // Shift y so apex is at y = 0
    float oy = ro.y - apex_y;

    const float a = rd.x * rd.x + rd.z * rd.z - k2 * rd.y * rd.y;
    const float b = 2.0f * (ro.x * rd.x + ro.z * rd.z - k2 * oy * rd.y);
    const float c = ro.x * ro.x + ro.z * ro.z - k2 * oy * oy;

    // Handle degenerate cases (linear equation)
    if (fabsf(a) < 1e-6f) {
        if (fabsf(b) < 1e-6f)
            return 1e9f;
        float t = -c / b;
        if (t < 1e-6f)
            return 1e9f;
        float y = ro.y + t * rd.y;
        if (y >= -o->height * 0.5f && y <= apex_y)
            return t;
        return 1e9f;
    }

    float t0, t1;
    const float d = solve_quadratic_sp(a, b, c, &t0, &t1);
    if (d < 0.0f)
        return 1e9f;

    float candidate = 1e9f;
    if (t0 > 1e-6f) {
        float y0 = ro.y + t0 * rd.y;
        if (y0 >= -o->height * 0.5f && y0 <= apex_y)
            candidate = t0;
    }
    if (t1 > 1e-6f && t1 < candidate) {
        float y1 = ro.y + t1 * rd.y;
        if (y1 >= -o->height * 0.5f && y1 <= apex_y)
            candidate = t1;
    }
    return candidate;
}

// Calculates intersection with base disk (handles rays from inside)
static float disk_distance(t_object *o, t_vec3 ro, t_vec3 rd) {
    const float h = -o->height * 0.5f;
    if (fabsf(rd.y) < 1e-6f)
        return 1e9f;

    const float t = (h - ro.y) / rd.y;
    if (t < 1e-6f)
        return 1e9f;

    const float x = ro.x + t * rd.x;
    const float z = ro.z + t * rd.z;
    if (x * x + z * z > o->radius * o->radius)
        return 1e9f;

    return t;
}

// Returns closest intersection (body or base)
float cone_distance(t_object *o, t_vec3 ro, t_vec3 rd) {
    const float body_dist = body_distance(o, ro, rd);
    const float base_dist = disk_distance(o, ro, rd);
    return fminf(body_dist, base_dist);
}

// Normal calculation (handles both inside and outside)
t_vec3 cone_normal(t_object *o, t_vec3 p) {
    // Base disk
    if (fabsf(p.y + o->height * 0.5f) < 1e-6f)
        return vec3(0.0f, -1.0f, 0.0f);

    // Cone body
    const float apex_y = 0.5f * o->height;
    const float k = o->radius / o->height;
    t_vec3 normal = vec3(p.x, -k * k * (p.y - apex_y), p.z);
    return vec3_normalize(normal);
}

// Texture coordinates (cylindrical for body, planar for base)
t_vec3 cone_texcoord(t_object *o, t_vec3 p) {
    float u, v;
    if (fabsf(p.y + o->height * 0.5f) < 1e-6f) {
        u = (p.x / o->radius + 1.0f) * 0.5f;
        v = (p.z / o->radius + 1.0f) * 0.5f;
    } else {
        u = 1.0f - (atan2f(p.z, p.x) / (2.0f * M_PI) + 0.5f);
        v = (p.y + o->height * 0.5f) / o->height;
    }
    return vec3(u, v, 0.0f);
}
