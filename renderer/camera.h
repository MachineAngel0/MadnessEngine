#ifndef CAMERA_H
#define CAMERA_H

/* CAMERA */
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

void camera_init(camera* out_camera);



// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void camera_process_keyboard(camera* cam, Camera_Movement movement_direction, float deltaTime);


// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void camera_process_mouse_movement(camera* cam, float dt, float x_offset, float y_offset, bool constrain_pitch);
// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void process_mouse_scroll(camera* cam, float y_offset);


void camera_update(camera* cam, float dt);

mat4 camera_get_view_matrix(camera* cam);
mat4 camera_get_fps_view_matrix(camera* cam);


mat4 camera_get_projection(camera* cam, const float width, const float height);

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
