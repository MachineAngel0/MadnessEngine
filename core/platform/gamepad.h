#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "logger.h"
#include "../core/event.h"



typedef enum gamepad_keys
{
    DPAD_UP,
    DPAD_DOWN,
    DPAD_LEFT,
    DPAD_RIGHT,
    GAMEPAD_START,
    GAMEPAD_BACK,
    GAMEPAD_LEFT_THUMB,
    GAMEPAD_RIGHT_THUMB,
    GAMEPAD_LEFT_SHOULDER,
    GAMEPAD_RIGHT_SHOULDER,
    GAMEPAD_A,
    GAMEPAD_B,
    GAMEPAD_X,
    GAMEPAD_Y,

    GAMEPAD_MAX_KEYS,
} gamepad_keys;

typedef enum gamepad_joy_keys
{
    GAMEPAD_JOY_LEFT,
    GAMEPAD_JOY_RIGHT,
    GAMEPAD_TRIGGER_LEFT,
    GAMEPAD_TRIGGER_RIGHT,

    GAMEPAD_JOY_MAX_KEYS,
} gamepad_joy_keys;

//not in use rn, but i might want to,
typedef enum CONTROLLERS_ID
{
    CONTROLLER_1,
    CONTROLLER_2,
    CONTROLLER_3,
    CONTROLLER_4,
    CONTROLLER_MAX,
} CONTROLLERS_ID;

char* gamepad_enum_to_string[] = {
    "DPAD_UP",
    "DPAD_DOWN",
    "DPAD_LEFT",
    "DPAD_RIGHT",
    "GAMEPAD_START",
    "GAMEPAD_BACK",
    "GAMEPAD_LEFT_THUMB",
    "GAMEPAD_RIGHT_THUMB",
    "GAMEPAD_LEFT_SHOULDER",
    "GAMEPAD_RIGHT_SHOULDER",
    "GAMEPAD_A",
    "GAMEPAD_B",
    "GAMEPAD_X",
    "GAMEPAD_Y",
    "GAMEPAD_MAX_KEYS",
};


typedef struct gamepad_joy_state
{
    // between -32768 and 32767.
    int16_t joy_stick_left_x;
    int16_t joy_stick_left_y;
    int16_t joy_stick_right_x;
    int16_t joy_stick_right_y;

    // 0-255
    int16_t left_trigger;
    int16_t right_trigger;
} gamepad_joy_state;


typedef struct gamepad_state
{
    bool gamepad_current[GAMEPAD_MAX_KEYS];
    bool gamepad_previous[GAMEPAD_MAX_KEYS];
    gamepad_joy_state joy_current;
    gamepad_joy_state joy_previous;
    bool connected;
} gamepad_state;

typedef struct gamepad_system
{
    gamepad_state* gamepad_player_info;
    u32 player_count;
}gamepad_system;

static gamepad_system gamepad;

void gamepad_load_dll(void);
u32 gamepad_get_max_player_count(void);



bool gamepad_button_recieve(event_type code, uint32_t sender_id, uint32_t subscriber_id, event_context data);


bool gamepad_button_press(event_type code, uint32_t sender_id, uint32_t subscriber_id, event_context data);

void gamepad_init(void);

void gamepad_shutdown(void);

void gamepad_update(void);
void gamepad_poll(void);

void set_gamepad_vibration(uint8_t gamepad_index, unsigned short left_motor_speed, unsigned short right_motor_speed);

void set_gamepad_vibration_default_on(uint8_t gamepad_index);

void set_gamepad_vibration_default_off(uint8_t gamepad_index);


bool gamepad_is_pressed(uint8_t gamepad_id, gamepad_keys gamepad_enum);

bool gamepad_is_released(uint8_t gamepad_id, gamepad_keys gamepad_enum);

bool gamepad_was_pressed(uint8_t gamepad_id, gamepad_keys gamepad_enum);

bool gamepad_was_released(uint8_t gamepad_id, gamepad_keys gamepad_enum);

void gamepad_get_left_joy_pos(uint8_t gamepad_id, uint16_t* left_x, uint16_t* left_y);

void gamepad_get_right_joy_pos(uint8_t gamepad_id, int16_t* left_x, int16_t* left_y);

void gamepad_get_triggers(uint8_t gamepad_id, int16_t* left_trigger, int16_t* right_trigger);


#endif //GAMEPAD_H
