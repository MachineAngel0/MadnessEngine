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
    vec3 position;
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
    const float SENSITIVITY = 10.0f;
    const float FOV = 90.0f;
    const float ZNEAR = 0.1f;
    const float ZFAR = 100.0f;

    out_camera->rotation = vec3_zero();
    out_camera->position = vec3_zero();
    out_camera->viewPos = vec4_zero();

    out_camera->rotation_speed = SENSITIVITY;
    out_camera->move_speed = 1.0f;


    out_camera->fov = FOV;
    out_camera->znear = ZNEAR;
    out_camera->zfar = ZFAR;

    out_camera->projection;
    out_camera->view;

    out_camera->pitch = 0.0f;
    out_camera->yaw = -90.f;

    // camera_update_view_matrix(out_camera);

    // event_register(EVENT_MOUSE_MOVED, 50, camera_mouse_movement_event);
    // event_register(EVENT_MOUSE_WHEEL, 50, camera_mouse_movement_scroll_event);
    // event_register(EVENT_KEY_PRESSED, 50, camera_keyboard_event);
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void camera_process_mouse_movement(camera* cam, float dt, float x_offset, float y_offset, bool constrain_pitch)
{
    x_offset *= cam->rotation_speed;
    y_offset *= cam->rotation_speed;

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
        cam->position.z -= cam->move_speed * dt;
    }
    if (input_is_key_pressed(KEY_S))
    {
        cam->position.z += cam->move_speed * dt;
    }
    if (input_is_key_pressed(KEY_A))
    {
        cam->position.x -= cam->move_speed * dt;
    }
    if (input_is_key_pressed(KEY_D))
    {
        cam->position.x += cam->move_speed * dt;
    }

    u16 x;
    u16 y;
    input_get_mouse_change(&x, &y);
    camera_process_mouse_movement(cam, dt, x, y, true);

    input_get_mouse_change(&x, &y);

    //TODO:this is bugged, its acting like on pressed
    if (input_key_released_unique(KEY_Q))
    {
        process_mouse_scroll(cam,-10.0f);
    }
    if (input_key_released_unique(KEY_E))
    {
        process_mouse_scroll(cam,10.0f);
    }


}

mat4 camera_get_view_matrix(camera* cam)
{
    mat4 temp_view = mat4_translation((vec3){cam->position.x, cam->position.y, cam->position.z + 1.0f});
    return mat4_inverse(temp_view);
}

mat4 camera_get_fps_view_matrix(camera* cam)
{
    // I assume the values are already converted to radians.
    float cosPitch = cos(cam->pitch);
    float sinPitch = sin(cam->pitch);
    float cosYaw = cos(cam->yaw);
    float sinYaw = sin(cam->yaw);

    vec3 xaxis = {cosYaw, 0, -sinYaw};
    vec3 yaxis = {sinYaw * sinPitch, cosPitch, cosYaw * sinPitch};
    vec3 zaxis = {sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw};

    // Create a 4x4 view matrix from the right, up, forward and eye position vectors
    mat4 viewMatrix = (mat4){
        .rows[0] = (vec4){xaxis.x, yaxis.x, zaxis.x, 0},
        .rows[1] =(vec4){xaxis.y, yaxis.y, zaxis.y, 0},
        .rows[2] =(vec4){xaxis.z, yaxis.z, zaxis.z, 0},
        .rows[3] =(vec4) {(-vec3_dot(xaxis, cam->position), -vec3_dot(yaxis, cam->position), -vec3_dot(zaxis, cam->position), 1)}
    };

    return mat4_inverse(viewMatrix);

    // return viewMatrix;
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
