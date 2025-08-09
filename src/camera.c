#include "minirt.h"

// Handle mouse movement of the camera, pitching around the x-axis when the
// mouse is moved up or down, and yawing around the y-axis when it's moved
// sideways. Returns true if the camera moved.

static bool	camera_mouse_movement(t_render *r)
{
	const float		fov = radians(r->scene->fov);
	const float		sensitivity = MOUSE_SENSITIVITY * sinf(fov * 0.5f);
	static int32_t	prev[2];
	int32_t			curr[2];
	int32_t			delta[2];

	mlx_get_mouse_pos(r->mlx, &curr[0], &curr[1]);
	delta[0] = curr[0] - prev[0];
	delta[1] = curr[1] - prev[1];
	prev[0] = curr[0];
	prev[1] = curr[1];
	if (mlx_is_mouse_down(r->mlx, MLX_MOUSE_BUTTON_LEFT)
		&& (delta[0] != 0 || delta[1] != 0))
	{
		r->camera_yaw += sensitivity * delta[0];
		r->camera_pitch += sensitivity * delta[1];
		r->camera_pitch = clamp(r->camera_pitch, radians(1), radians(179));
		r->scene->dir.x = sin(r->camera_pitch) * cos(r->camera_yaw);
		r->scene->dir.y = cos(r->camera_pitch);
		r->scene->dir.z = sin(r->camera_pitch) * sin(r->camera_yaw);
		return (true);
	}
	return (false);
}

// Handle keyboard movement of the camera, moving horizontally with the WASD
// keys, and vertically with the shift/space keys. Returns true if the camera
// moved.

static bool	camera_keyboard_movement(t_render *r)
{
	t_vec3	vec;
	t_vec3	move_x;
	t_vec3	move_z;
	t_keys	keys;

	move_x = norm3(vec3(r->camera_x.x, 0.0f, r->camera_x.z));
	move_z = norm3(vec3(r->camera_z.x, 0.0f, r->camera_z.z));
	keys.forward = mlx_is_key_down(r->mlx, KEY_FORWARD);
	keys.left = mlx_is_key_down(r->mlx, KEY_LEFT);
	keys.back = mlx_is_key_down(r->mlx, KEY_BACK);
	keys.right = mlx_is_key_down(r->mlx, KEY_RIGHT);
	keys.up = mlx_is_key_down(r->mlx, KEY_UP);
	keys.down = mlx_is_key_down(r->mlx, KEY_DOWN);
	vec = vec3(0.0f, keys.up - keys.down, 0.0f);
	vec = add3(vec, scale3(move_x, keys.right - keys.left));
	vec = add3(vec, scale3(move_z, keys.back - keys.forward));
	vec = scale3(vec, 3.0f * r->mlx->delta_time);
	r->scene->pos = add3(r->scene->pos, vec);
	return (len3(vec) > 0.0f);
}

// Update the basis vectors for the camera coordinate system, and and find the
// direction vectors pointing toward the four corners of the viewport.

static void	camera_update_viewport(t_render *r)
{
	const float	view_h = 0.5f * tan(radians(r->scene->fov) * 0.5f);
	const float	view_w = view_h * (float) r->image->width / r->image->height;
	t_vec3		vec[4];

	r->camera_z = scale3(r->scene->dir, -1.0f);
	r->camera_x = norm3(cross3(vec3(0, 1, 0), r->camera_z));
	r->camera_y = cross3(r->camera_z, r->camera_x);
	vec[0] = scale3(r->camera_x, -view_w);
	vec[1] = scale3(r->camera_x, +view_w);
	vec[2] = scale3(r->camera_y, -view_h);
	vec[3] = scale3(r->camera_y, +view_h);
	r->viewport[0] = sub3(add3(vec[0], vec[2]), r->camera_z);
	r->viewport[1] = sub3(add3(vec[1], vec[2]), r->camera_z);
	r->viewport[2] = sub3(add3(vec[0], vec[3]), r->camera_z);
	r->viewport[3] = sub3(add3(vec[1], vec[3]), r->camera_z);
}

// Update all camera parameters. Called before rendering a new frame.

void	camera_update(t_render *r)
{
	if (camera_mouse_movement(r))
		r->frame_samples = 0;
	camera_update_viewport(r);
	if (camera_keyboard_movement(r))
		r->frame_samples = 0;
	r->jitter = random_point_in_square(r->frame_samples++);
}
