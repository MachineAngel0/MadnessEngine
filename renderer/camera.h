#ifndef CAMERA_H
#define CAMERA_H
#include "math_lib.h"
#include "math_types.h"

typedef enum Camera_Movement
{
    CAMERA_MOVEMENT_FORWARD,
    CAMERA_MOVEMENT_BACKWARD,
    CAMERA_MOVEMENT_LEFT,
    CAMERA_MOVEMENT_RIGHT
} Camera_Movement;

typedef struct camera
{
    // camera Attributes
    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movement_speed;
    float mouse_sensitivity;
    float zoom;

    //perspective options
    float fov;
    float znear;
    float zfar;

    mat4 projection;

} camera;

static camera camera_temp;

camera* camera_temp_get(void)
{
    return &camera_temp;
}



 //forward declare
void update_camera_vectors(camera* camera); //forward declare

bool camera_mouse_movement_event(u16 code, void* sender, void* listener_inst, event_context context);

bool camera_mouse_movement_scroll_event(u16 code, void* sender, void* listener_inst, event_context context);

bool camera_keyboard_event(u16 code, void* sender, void* listener_inst, event_context context);

void camera_init(camera* out_camera)
{
    memset(out_camera, 0, sizeof(camera));

    // Default camera values
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 10.0f;
    const float ZOOM = 45.0f;
    const float FOV = 90.0f;
    const float ZNEAR = 0.1f;
    const float ZFAR = 100.0f;

    out_camera->pos = (vec3){0.0f, 0.0f, 0.0f};
    out_camera->up = (vec3){0.0f, 1.0f, 0.0f};
    out_camera->yaw = YAW;
    out_camera->pitch = PITCH;
    out_camera->world_up = (vec3){0.0f, 1.0f, 0.0f};
    out_camera->front = (vec3){0.0f, 0.0f, 1.0f};
    out_camera->movement_speed = SPEED;
    out_camera->mouse_sensitivity = SENSITIVITY;
    out_camera->zoom = ZOOM;
    out_camera->fov = FOV;
    out_camera->znear = ZNEAR;
    out_camera->zfar = ZFAR;

    update_camera_vectors(out_camera);

    event_register(EVENT_MOUSE_MOVED, 50, camera_mouse_movement_event);
    event_register(EVENT_MOUSE_WHEEL, 50, camera_mouse_movement_scroll_event);
    event_register(EVENT_KEY_PRESSED, 50, camera_keyboard_event);
}


void camera_bad_init()
{
    camera_init(&camera_temp);
}


//TODO:
// void camera_destroy()
// {
// }
// calculates the front vector from the Camera's (updated) Euler Angles


void update_camera_vectors(camera* cam)
{
    // calculate the new Front vector
    vec3 front;
    front.x = cos(deg_to_rad(cam->yaw)) * cos(deg_to_rad(cam->pitch));
    front.y = sin(deg_to_rad(cam->pitch));
    front.z = sin(deg_to_rad(cam->yaw)) * cos(deg_to_rad(cam->pitch));
    cam->front = vec3_normalize_functional(front);
    // also re-calculate the Right and Up vector
    cam->right = vec3_normalize_functional(vec3_cross(cam->front, cam->world_up));
    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    cam->up = vec3_normalize_functional(vec3_cross(cam->right, cam->front));
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix


mat4 camera_get_view_matrix(camera* cam)
{
    return mat4_look_at(cam->pos, vec3_add(cam->pos, cam->front), cam->up);
}

mat4 camera_get_projection_matrix(const camera* cam, const f32 width, const f32 height)
{
    return mat4_perspective(deg_to_rad(cam->fov), width/height, cam->znear, cam->zfar);
}

mat4 camera_get_view_matrix_bad()
{
    return camera_get_view_matrix(&camera_temp);
}

mat4 camera_get_projection_matrix_bad(f32 width, f32 height)
{
    return camera_get_projection_matrix(&camera_temp, width, height);
}



// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void camera_process_keyboard(camera* cam, Camera_Movement movement_direction, float deltaTime)
{
    float velocity = cam->movement_speed * deltaTime;
    if (movement_direction == CAMERA_MOVEMENT_FORWARD)
    {
        vec3 t = vec3_mul_scalar(cam->front, velocity);
        cam->pos = vec3_add(cam->pos, t);
    }
    if (movement_direction == CAMERA_MOVEMENT_BACKWARD)
    {
        vec3 t = vec3_mul_scalar(cam->front, velocity);
        cam->pos = vec3_sub(cam->pos, t);
    }
    if (movement_direction == CAMERA_MOVEMENT_LEFT)
    {
        vec3 t = vec3_mul_scalar(cam->right, velocity);
        cam->pos = vec3_sub(cam->pos, t);
    }
    if (movement_direction == CAMERA_MOVEMENT_RIGHT)
    {
        vec3 t = vec3_mul_scalar(cam->right, velocity);
        cam->pos = vec3_add(cam->pos, t);
    }
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void camera_process_mouse_movement(camera* cam, float x_offset, float y_offset, bool constrain_pitch)
{
    x_offset *= cam->mouse_sensitivity;
    y_offset *= cam->mouse_sensitivity;

    cam->yaw += x_offset;
    cam->pitch += y_offset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch)
    {
        if (cam->pitch > 89.0f)
            cam->pitch = 89.0f;
        if (cam->pitch < -89.0f)
            cam->pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors(cam);
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void process_mouse_scroll(camera* cam, float y_offset)
{
    cam->zoom -= y_offset;
    if (cam->zoom < 1.0f)
        cam->zoom = 1.0f;
    if (cam->zoom > 45.0f)
        cam->zoom = 45.0f;
}


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


#endif //CAMERA_H
