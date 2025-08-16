#include "minirt.h"

// Store the current mouse position and mouse delta for later use.

static void	update_mouse(t_render *r)
{
	int32_t	x;
	int32_t	y;
	t_vec3	new_mouse_pos;

	mlx_get_mouse_pos(r->mlx, &x, &y);
	new_mouse_pos = vec3(x, y, 0.0f);
	r->mouse_delta = sub3(new_mouse_pos, r->mouse_pos);
	r->mouse_pos = new_mouse_pos;
}

// Translate the currently selected object. The movement is scaled by the
// distance to the viewer, to make the controls more intuitive.

static void	translate_selection(t_render *r, t_object *obj)
{
	t_vec3	delta;

	delta = div3(r->mouse_delta, vec3(r->image->width, r->image->height, 1.0f));
	delta = scale3(delta, len3(sub3(obj->pos, r->scene->pos)));
	obj->pos = add3(obj->pos, scale3(r->camera_x, delta.x));
	obj->pos = sub3(obj->pos, scale3(r->camera_y, delta.y));
}

// Rotate the currently selected object. Mouse movement in the xy-plane
// correspond to rotations around the camera x and y axes.

static void	rotate_selection(t_render *r, t_object *obj)
{
	t_vec3	delta;
	t_quat	rotate;

	delta = scale3(r->mouse_delta, MOUSE_SENSITIVITY);
	rotate = quat_from_axis_angle(r->camera_y, delta.x);
	obj->rot = quat_multiply(rotate, obj->rot);
	rotate = quat_from_axis_angle(r->camera_x, delta.y);
	obj->rot = quat_multiply(rotate, obj->rot);
}

// Scale the currently selected object. All object properties related to size
// are scaled by an amount proportional to mouse movement. Unused properties are
// simply ignored by objects that don't use them. There are no safeguards
// against making an object too small to see or select, so just don't do that!

static void	scale_selection(t_render *r, t_object *obj)
{
	const t_vec3	delta = scale3(r->mouse_delta, MOUSE_SENSITIVITY);
	const float		scale = fmaxf(1e-3f, 1.0f - (delta.y + delta.x));

	obj->radius *= scale;
	obj->height *= scale;
	obj->size = scale3(obj->size, scale);
}

// Transform (translate/rotate/scale) the currently selected object, if any.

void	transform_selection(t_render *r)
{
	update_mouse(r);
	if (r->selection == NULL || len3(r->mouse_delta) < 1e-5f)
		return ;
	if (r->mode == MODE_TRANSLATE)
		translate_selection(r, r->selection);
	if (r->mode == MODE_ROTATE)
		rotate_selection(r, r->selection);
	if (r->mode == MODE_SCALE)
		scale_selection(r, r->selection);
	r->frame_samples = 0;
}
