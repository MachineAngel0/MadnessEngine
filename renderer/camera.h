#ifndef CAMERA_H
#define CAMERA_H
#include "input.h"
#include "math_lib.h"

typedef enum Camera_Movement
{
    CAMERA_MOVEMENT_FORWARD,
    CAMERA_MOVEMENT_BACKWARD,
    CAMERA_MOVEMENT_LEFT,
    CAMERA_MOVEMENT_RIGHT
} Camera_Movement;


typedef struct camera
{
    vec3 rotation;
    vec3 pos;
    vec4 viewPos;

    float rotation_speed;
    float move_speed;

    //perspective options
    float fov;
    float znear;
    float zfar;

    mat4 projection;
    mat4 view;

    //fps
    float pitch;
    float yaw;
} camera;

struct camera_arrays
{
    camera lookat_cameras[10];
    camera fps_cameras[10];
    camera arcball_cameras[10];
};


// bool camera_mouse_movement_event(u16 code, void* sender, void* listener_inst, event_context context);
// bool camera_mouse_movement_scroll_event(u16 code, void* sender, void* listener_inst, event_context context);
// bool camera_keyboard_event(u16 code, void* sender, void* listener_inst, event_context context);

void camera_init(camera* out_camera)
{
    memset(out_camera, 0, sizeof(camera));

    // Default camera values
    const float SPEED = 2.5f;
    const float SENSITIVITY = 20.0f;
    const float FOV = 90.0f;
    const float ZNEAR = 0.1f;
    const float ZFAR = 100.0f;

    out_camera->rotation = vec3_zero();
    out_camera->pos = vec3_zero();
    out_camera->viewPos = vec4_zero();

    out_camera->rotation_speed = SENSITIVITY;
    out_camera->move_speed = 1.0f;


    out_camera->fov = FOV;
    out_camera->znear = ZNEAR;
    out_camera->zfar = ZFAR;

    out_camera->projection;
    out_camera->view;

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
    float yaw   = deg_to_rad(cam->yaw);

    float cos_pitch = cosf(pitch);
    float sin_pitch = sinf(pitch);
    float cos_yaw = cosf(yaw);
    float sin_yaw = sinf(yaw);

    // Standard FPS forward vector (left-handed)
    vec3 forward = { cos_pitch * sin_yaw, sin_pitch, cos_pitch * cos_yaw };
    forward = vec3_normalize_functional(forward);

    vec3 right =  vec3_normalize_functional(vec3_cross( forward, vec3_up()));

    if (movement_direction == CAMERA_MOVEMENT_FORWARD)
    {
        vec3 t = vec3_mul_scalar(forward, velocity);
        cam->pos = vec3_sub(cam->pos, t);
    }
    if (movement_direction == CAMERA_MOVEMENT_BACKWARD)
    {
        vec3 t = vec3_mul_scalar(forward, velocity);
        cam->pos = vec3_add(cam->pos, t);
    }
    if (movement_direction == CAMERA_MOVEMENT_LEFT)
    {
        vec3 t = vec3_mul_scalar(right , velocity);
        cam->pos = vec3_add(cam->pos, t);
    }
    if (movement_direction == CAMERA_MOVEMENT_RIGHT)
    {
        vec3 t = vec3_mul_scalar(right , velocity);
        cam->pos = vec3_sub(cam->pos, t);
    }
}


// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void camera_process_mouse_movement(camera* cam, float dt, float x_offset, float y_offset, bool constrain_pitch)
{

    cam->yaw += x_offset * dt * cam->rotation_speed;
    cam->pitch += y_offset * dt * cam->rotation_speed;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch)
    {
        cam->pitch = clamp_float(cam->pitch, -89.0f, 89.0f);
    }

    cam->yaw = clamp_float(cam->yaw, 0.0f, 360.0f);

    DEBUG("PITCH: %f, YAW: %f", cam->pitch, cam->yaw);

}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void process_mouse_scroll(camera* cam, float y_offset)
{
    cam->fov -= y_offset;
    if (cam->fov < 1.0f)
        cam->fov = 1.0f;
    if (cam->fov > 120.0f)
        cam->fov = 120.0f;
    DEBUG("FOV CHANGED: %f", cam->fov)
}


mat4 camera_update(camera* cam, float dt)
{
    //TODO: This will need to be configurable at some point
    if (input_is_key_pressed(KEY_W))
    {
        camera_process_keyboard(cam, CAMERA_MOVEMENT_FORWARD, dt);
    }
    if (input_is_key_pressed(KEY_S))
    {
        camera_process_keyboard(cam, CAMERA_MOVEMENT_BACKWARD, dt);
    }
    if (input_is_key_pressed(KEY_A))
    {
        camera_process_keyboard(cam, CAMERA_MOVEMENT_LEFT, dt);
    }
    if (input_is_key_pressed(KEY_D))
    {
        camera_process_keyboard(cam, CAMERA_MOVEMENT_RIGHT, dt);
    }

    i16 x;
    i16 y;
    input_get_mouse_change(&x, &y);
    camera_process_mouse_movement(cam, dt, -x, -y, true);


    //TODO:this is bugged, its acting like on pressed
    if (input_key_released_unique(KEY_Q))
    {
        process_mouse_scroll(cam, -10.0f);
    }
    if (input_key_released_unique(KEY_E))
    {
        process_mouse_scroll(cam, 10.0f);
    }


}

mat4 camera_get_view_matrix(camera* cam)
{
    mat4 temp_view = mat4_translation((vec3){cam->pos.x, cam->pos.y, cam->pos.z + 1.0f});
    return mat4_inverse(temp_view);
}

mat4 camera_get_fps_view_matrix(camera* cam)
{
    float pitch = deg_to_rad(cam->pitch);
    float yaw   = deg_to_rad(cam->yaw);

    float cos_pitch = cosf(pitch);
    float sin_pitch = sinf(pitch);
    float cos_yaw = cosf(yaw);
    float sin_yaw = sinf(yaw);

    // Standard FPS forward vector (left-handed)
    vec3 forward = { cos_pitch * sin_yaw, sin_pitch, cos_pitch * cos_yaw };
    forward = vec3_normalize_functional(forward);

    // Compute right & up
    vec3 worldUp = vec3_up();
    vec3 right   = vec3_normalize_functional(vec3_cross(worldUp, forward));
    vec3 up      = vec3_cross(forward, right);

    // Build view matrix directly
    mat4 view = (mat4){
        .rows[0] = (vec4){ right.x,   up.x,   forward.x,   0.0f },
        .rows[1] = (vec4){ right.y,   up.y,   forward.y,   0.0f },
        .rows[2] = (vec4){ right.z,   up.z,   forward.z,   0.0f },
        .rows[3] = (vec4){
            -vec3_dot(right, cam->pos),
            -vec3_dot(up, cam->pos),
            -vec3_dot(forward, cam->pos),
            1.0f
        }
    };

    return view;
}


mat4 camera_get_projection(camera* cam, const float width, const float height)
{
    return mat4_perspective(cam->fov, (float) (width / height), cam->znear, cam->zfar);
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
#endif //CAMERA_H
