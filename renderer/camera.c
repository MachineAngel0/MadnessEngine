#include "camera.h"


void camera_init(camera* out_camera)
{
    memset(out_camera, 0, sizeof(camera));

    // Default camera values
    const float MOVE_SPEED = 3.5f;
    const float SENSITIVITY = 500.0f;
    const float FOV = 90.0f;
    const float ZNEAR = 0.1f;
    const float ZFAR = 100.0f;


    out_camera->rotation = glms_vec3_zero();
    out_camera->pos = glms_vec3_zero();

    out_camera->rotation_speed = SENSITIVITY;
    out_camera->move_speed = MOVE_SPEED;


    out_camera->fov = FOV;
    out_camera->znear = ZNEAR;
    out_camera->zfar = ZFAR;

    out_camera->projection = glms_mat4_identity();
    out_camera->view= glms_mat4_identity();

    out_camera->pitch = 90.0f;
    out_camera->yaw = 180.0f;

    // camera_update_view_matrix(out_camera);

    // event_register(EVENT_MOUSE_MOVED, 50, camera_mouse_movement_event);
    // event_register(EVENT_MOUSE_WHEEL, 50, camera_mouse_movement_scroll_event);
    // event_register(EVENT_KEY_PRESSED, 50, camera_keyboard_event);
}


// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void camera_process_keyboard(camera* cam, Camera_Movement movement_direction, float deltaTime)
{
    float velocity = cam->move_speed * deltaTime;
    float pitch = deg_to_rad(cam->pitch);
    float yaw = deg_to_rad(cam->yaw);

    float cos_pitch = cosf(pitch);
    float sin_pitch = sinf(pitch);
    float cos_yaw = cosf(yaw);
    float sin_yaw = sinf(yaw);

    // Standard FPS forward vector (left-handed)
    vec3s forward = {cos_pitch * sin_yaw, sin_pitch, cos_pitch * cos_yaw};
    forward = glms_vec3_normalize(forward);

    vec3s right = glms_vec3_normalize(glms_vec3_cross(GLMS_YUP, forward ));

    if (movement_direction == CAMERA_MOVEMENT_FORWARD)
    {
        vec3s t = glms_vec3_scale(forward, velocity);
        cam->pos = glms_vec3_add(cam->pos, t);
    }
    if (movement_direction == CAMERA_MOVEMENT_BACKWARD)
    {
        vec3s t = glms_vec3_scale(forward, velocity);
        cam->pos = glms_vec3_sub(cam->pos, t);
    }
    if (movement_direction == CAMERA_MOVEMENT_LEFT)
    {
        vec3s t = glms_vec3_scale(right, velocity);
        cam->pos = glms_vec3_add(cam->pos, t);
    }
    if (movement_direction == CAMERA_MOVEMENT_RIGHT)
    {
        vec3s t = glms_vec3_scale(right, velocity);
        cam->pos = glms_vec3_sub(cam->pos, t);
    }
}


// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void camera_process_mouse_movement(camera* cam, float dt, float x_offset, float y_offset, bool constrain_pitch)
{
    cam->yaw -= x_offset * cam->rotation_speed * dt;
    cam->pitch += y_offset * cam->rotation_speed * dt;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch)
    {
        cam->pitch = clamp_float(cam->pitch, -89.0f, 89.0f);
    }

    cam->yaw = clamp_float(cam->yaw, 0.1f, 360.0f);

    // DEBUG("PITCH: %f, YAW: %f", cam->pitch, cam->yaw);
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void camera_change_fov(camera* cam, float y_offset)
{
    cam->fov -= y_offset;
    if (cam->fov < 1.0f)
        cam->fov = 1.0f;
    if (cam->fov > 120.0f)
        cam->fov = 120.0f;
    DEBUG("FOV CHANGED: %f", cam->fov)
}


void camera_update(Input_System* input_system, camera* cam, float dt)
{
    if (input_is_key_pressed(input_system, KEY_W))
    {
        camera_process_keyboard(cam, CAMERA_MOVEMENT_FORWARD, dt);
    }
    if (input_is_key_pressed(input_system, KEY_S))
    {
        camera_process_keyboard(cam, CAMERA_MOVEMENT_BACKWARD, dt);
    }
    if (input_is_key_pressed(input_system, KEY_A))
    {
        camera_process_keyboard(cam, CAMERA_MOVEMENT_LEFT, dt);
    }
    if (input_is_key_pressed(input_system, KEY_D))
    {
        camera_process_keyboard(cam, CAMERA_MOVEMENT_RIGHT, dt);
    }


    if (input_is_key_pressed(input_system, KEY_LSHIFT))
    {
        s16 x;
        s16 y;
        input_get_mouse_change(input_system, &x, &y);
        camera_process_mouse_movement(cam, dt, x, y, true);
    }
    else
    {
        camera_process_mouse_movement(cam, dt, 0, 0, true);

    }

    if (input_key_released_unique(input_system, KEY_Q))
    {
        camera_change_fov(cam, -10.0f);
    }
    if (input_key_released_unique(input_system, KEY_E))
    {
        camera_change_fov(cam, 10.0f);
    }
}

mat4s camera_get_view_matrix(camera* cam)
{
    // glms_look()
    // mat4s temp_view = mat4_translation((vec3s){cam->pos.x, cam->pos.y, cam->pos.z + 1.0f});
    // return glms_mat4_inv(temp_view);
    //TODO:
    return glms_mat4_identity();
}

vec4s camera_get_world_position(camera* cam)
{
    // glms_look()
    // mat4s temp_view = mat4_translation((vec3s){cam->pos.x, cam->pos.y, cam->pos.z + 1.0f});
    // return glms_mat4_inv(temp_view);
    return (vec4s){cam->pos.x, cam->pos.y, cam->pos.z, 1.0};
}

mat4s camera_get_fps_view_matrix(camera* cam)
{
    /*float pitch = deg_to_rad(cam->pitch);
    float yaw = deg_to_rad(cam->yaw);

    float cos_pitch = cosf(pitch);
    float sin_pitch = sinf(pitch);
    float cos_yaw = cosf(yaw);
    float sin_yaw = sinf(yaw);

    // Standard FPS forward vector (left-handed)
    vec3s forward = {cos_pitch * sin_yaw, sin_pitch, cos_pitch * cos_yaw};
    forward = glms_vec3_normalize(forward);

    // Compute right & up
    vec3s worldUp = glms_vec3_up();
    vec3s right = glms_vec3_normalize(glms_vec3_cross(forward, worldUp));
    vec3s camera_up = glms_vec3_cross(forward, right);

    // Build view matrix directly

    mat4s view = (mat4s){
        .raw[0] = (vec4s){.raw[0] = right.x, .raw[1] =camera_up.x, .raw[2] =forward.x, .raw[3] = 0.0f},
        .raw[1] = (vec4s){right.y, camera_up.y, forward.y, 0.0f},
        .raw[2] = (vec4s){right.z, camera_up.z, forward.z, 0.0f},
        .raw[3] = (vec4s){
            -glms_vec3_dot(right, cam->pos),
            -glms_vec3_dot(camera_up, cam->pos),
            -glms_vec3_dot(forward, cam->pos),
            1.0f
        }
    };

    return view;*/

    float pitch = deg_to_rad(cam->pitch);
    float yaw = deg_to_rad(cam->yaw);

    float cos_pitch = cosf(pitch);
    float sin_pitch = sinf(pitch);
    float cos_yaw = cosf(yaw);
    float sin_yaw = sinf(yaw);

    // Standard FPS forward vector (left-handed)
    vec3s forward = {cos_pitch * sin_yaw, sin_pitch, cos_pitch * cos_yaw};
    forward = glms_vec3_normalize(forward);

    vec3s world_up = (vec3s){0.0f, 1.0f, 0.0f}; // GLM_YUP cast to vec3s

    vec3s target = glms_vec3_add(cam->pos, forward);

    return glms_lookat(cam->pos, target, world_up);


}


mat4s camera_get_projection(camera* cam, const float width, const float height)
{
    return glms_perspective(deg_to_rad(cam->fov), (float)(width / height), cam->znear, cam->zfar);
    // float fov = 1.5;
    // return mat4_orthographic(-10 * fov, 10 * fov, -10 * fov, 10 * fov, cam->znear, cam->zfar);
}


/*
void camera_update_view_matrix(camera* out_camera)
{

    mat4 rotM = mat4(1.0f);
    mat4 transM;
    glm_vec3_
    rotM = glm_rotate(rotM, glm_rad(out_camera->rotation) *  1.0f), vec3(1.0f, 0.0f, 0.0f));
    rotM = glm_rotate(rotM, glm_rad(rotation.y), vec3(0.0f, 1.0f, 0.0f));
    rotM = glm_rotate(rotM, glm_rad(rotation.z), vec3(0.0f, 0.0f, 1.0f));



    vec3 translation = position;

    transM = glm_translate(glm_mat4_identity, translation);

    matrices.view = rotM * transM;

    viewPos = glm::vec4(position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);


}


//TODO:
// void camera_destroy()
// {
// }
// calculates the front vector from the Camera's (updated) Euler Angles








bool camera_mouse_movement_event(u16 code, void* sender, void* listener_inst, event_context context)
{
    if (code == EVENT_MOUSE_MOVED)
    {
        DEBUG("mouse moved");
        uint16_t x = context.data.u16[0];
        uint16_t y = context.data.u16[1];
        camera_process_mouse_movement(&camera_temp, x, y, true);
    }

    return false;
}

bool camera_mouse_movement_scroll_event(u16 code, void* sender, void* listener_inst, event_context context)
{
    if (code == EVENT_MOUSE_WHEEL)
    {
        DEBUG("mouse scrolled for camera");

        uint8_t y = context.data.u8[0];
        process_mouse_scroll(&camera_temp, y);
    }

    return false;
}

bool camera_keyboard_event(u16 code, void* sender, void* listener_inst, event_context context)
{
    if (code == EVENT_KEY_PRESSED)
    {

        uint16_t key_code = context.data.u16[0];
        float fake_delta = 1.0f;

        if (key_code == KEY_W)
        {
            camera_process_keyboard(&camera_temp, CAMERA_MOVEMENT_FORWARD, fake_delta); //TODO: false delta
            DEBUG("WWWWWWW");
        }
        if (key_code == KEY_S)
        {
            camera_process_keyboard(&camera_temp, CAMERA_MOVEMENT_BACKWARD, fake_delta); //TODO: false delta
        }
        if (key_code == KEY_A)
        {
            camera_process_keyboard(&camera_temp, CAMERA_MOVEMENT_LEFT, fake_delta); //TODO: false delta
        }
        if (key_code == KEY_D)
        {
            camera_process_keyboard(&camera_temp, CAMERA_MOVEMENT_RIGHT, fake_delta); //TODO: false delta
        }
    }

    return false;
}

*/
