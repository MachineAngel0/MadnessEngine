#ifndef CAMERA_H
#define CAMERA_H
#include "input.h"
#include "maths/math_types.h"

//TODO: probably move out of the renderer and into resources

typedef enum Camera_Movement
{
    CAMERA_MOVEMENT_FORWARD,
    CAMERA_MOVEMENT_BACKWARD,
    CAMERA_MOVEMENT_LEFT,
    CAMERA_MOVEMENT_RIGHT
} Camera_Movement;


typedef struct Camera
{
    vec3s rotation;
    vec3s pos;

    float rotation_speed;
    float move_speed;

    //perspective options
    float fov;
    float znear;
    float zfar;

    mat4s projection;
    mat4s view;

    //fps
    float pitch;
    float yaw;
} Camera;

struct camera_arrays
{
    Camera lookat_cameras[10];
    Camera fps_cameras[10];
    Camera arcball_cameras[10];
};

// bool camera_mouse_movement_event(u16 code, void* sender, void* listener_inst, event_context context);
// bool camera_mouse_movement_scroll_event(u16 code, void* sender, void* listener_inst, event_context context);
// bool camera_keyboard_event(u16 code, void* sender, void* listener_inst, event_context context);

void camera_init(Camera* out_camera);


// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void camera_process_keyboard(Camera* cam, Camera_Movement movement_direction, float deltaTime);


// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void camera_process_mouse_movement(Camera* cam, float dt, float x_offset, float y_offset, bool constrain_pitch);
// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void camera_change_fov(Camera* cam, float y_offset);


void camera_update(Input_System* input_syste, Camera* cam, float dt);

mat4s camera_get_view_matrix(Camera* cam);
mat4s camera_get_fps_view_matrix(Camera* cam);

vec4s camera_get_world_position(Camera* cam);

mat4s camera_get_projection(Camera* cam, float width, float height);

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



*/


#endif //CAMERA_H
