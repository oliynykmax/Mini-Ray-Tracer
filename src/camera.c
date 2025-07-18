#include "minirt.h"

// Handle mouse movement of the camera, pitching around the x-axis when the
// mouse is moved up or down, and yawing around the y-axis when it's moved
// sideways.

static void	camera_mouse_movement(t_render *r)
{
	const float		fov = radians(r->scene->fov);
	const float		sensitivity = MOUSE_SENSITIVITY * sinf(fov * 0.5f);
	static int32_t	prev[2];
	int32_t			curr[2];

	mlx_get_mouse_pos(r->mlx, &curr[0], &curr[1]);
	if (mlx_is_mouse_down(r->mlx, MLX_MOUSE_BUTTON_LEFT))
	{
		r->camera_yaw -= sensitivity * (curr[0] - prev[0]);
		r->camera_pitch -= sensitivity * (curr[1] - prev[1]);
		r->camera_pitch = clamp(r->camera_pitch, radians(1), radians(179));
		r->scene->dir.x = sin(r->camera_pitch) * cos(r->camera_yaw);
		r->scene->dir.y = cos(r->camera_pitch);
		r->scene->dir.z = sin(r->camera_pitch) * sin(r->camera_yaw);
	}
	prev[0] = curr[0];
	prev[1] = curr[1];
}

// Handle keyboard movement of the camera, moving horizontally with the WASD
// keys, and vertically with the shift/space keys.

static void	camera_keyboard_movement(t_render *r)
{
	t_vec3	vec;
	t_vec3	move_x;
	t_vec3	move_z;
	t_keys	keys;

	move_x = vec3_normalize(vec3(r->camera_x.x, 0.0f, r->camera_x.z));
	move_z = vec3_normalize(vec3(r->camera_z.x, 0.0f, r->camera_z.z));
	keys.forward = mlx_is_key_down(r->mlx, KEY_FORWARD);
	keys.left = mlx_is_key_down(r->mlx, KEY_LEFT);
	keys.back = mlx_is_key_down(r->mlx, KEY_BACK);
	keys.right = mlx_is_key_down(r->mlx, KEY_RIGHT);
	keys.up = mlx_is_key_down(r->mlx, KEY_UP);
	keys.down = mlx_is_key_down(r->mlx, KEY_DOWN);
	vec = vec3(0.0f, keys.down - keys.up, 0.0f);
	vec = vec3_add(vec, vec3_scale(move_x, keys.right - keys.left));
	vec = vec3_add(vec, vec3_scale(move_z, keys.forward - keys.back));
	vec = vec3_scale(vec, 3.0f * r->mlx->delta_time);
	r->scene->pos = vec3_add(r->scene->pos, vec);
}

// Update the basis vectors for the camera coordinate system, and and find the
// direction vectors pointing toward the four corners of the viewport.

static void	camera_update_viewport(t_render *r)
{
	const float	view_h = 0.5f * tan(radians(r->scene->fov) * 0.5f);
	const float	view_w = view_h * (float) r->image->width / r->image->height;
	t_vec3		vec[4];

	r->camera_z = r->scene->dir;
	r->camera_x = vec3_cross(r->camera_z, vec3(0.0f, -1.0f, 0.0f));
	r->camera_x = vec3_normalize(r->camera_x);
	r->camera_y = vec3_cross(r->camera_z, r->camera_x);
	vec[0] = vec3_scale(r->camera_x, -view_w);
	vec[1] = vec3_scale(r->camera_x, +view_w);
	vec[2] = vec3_scale(r->camera_y, -view_h);
	vec[3] = vec3_scale(r->camera_y, +view_h);
	r->viewport[0] = vec3_add(vec3_add(vec[0], vec[2]), r->camera_z);
	r->viewport[1] = vec3_add(vec3_add(vec[1], vec[2]), r->camera_z);
	r->viewport[2] = vec3_add(vec3_add(vec[0], vec[3]), r->camera_z);
	r->viewport[3] = vec3_add(vec3_add(vec[1], vec[3]), r->camera_z);
}

// Update all camera parameters. Called before rendering a new frame.

void	camera_update(t_render *r)
{
	camera_update_viewport(r);
	camera_mouse_movement(r);
	camera_keyboard_movement(r);
}
