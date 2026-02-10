#ifndef GAMEPAD_H
#define GAMEPAD_H

#if MPLATFORM_WINDOWS


#include "logger.h"
#include "../core/platform/event.h"


#include <windows.h>
#include <winerror.h>
#include <xinput.h>


//FUTURE: ADD CHECKS FOR DISCONNECTED CONTROLLERS,
// HAVE SOME METHOD OF SETTING A MAIN CONTROLLER SO I DONT NEED TO KEEP PASSING IN AN ID, I WON'T LIKELY HAVE MORE THAN A SINGLE PLAYER FOR MOST OF MY GAMES
// SINCE THIS IS A WINDOWS API I WOULD NEED TO ABSTRACT THAT AWAY INTO ITS OWN PLATFORM LAYER

//make a copy of the function with a different name
typedef DWORD WINAPI X_InputGetState(DWORD, XINPUT_STATE*);
typedef DWORD WINAPI X_InputSetState(DWORD, XINPUT_VIBRATION*);

#define XInputGetState X_InputGetState_Internal
#define XInputSetState X_InputSetState_Internal

//global var for the functions
static X_InputGetState* X_InputGetState_Internal = 0;
static X_InputSetState* X_InputSetState_Internal = 0;


void x_input_load_dll(void)
{
    //we are loading the dll ourselves, which is really cool

    //Windows 8 (XInput 1.4), DirectX SDK (XInput 1.3), Windows Vista (XInput 9.1.0)

    // Target Platform 	Windows
    // Header 	xinput.h
    // Library 	Xinput.lib; Xinput9_1_0.lib
    // DLL 	Xinput1_4.dll; Xinput9_1_0.dll; Xinputuap.dll

    HMODULE XInputLib = LoadLibrary("xinput1_4.dll");
    if (!XInputLib)
    {
        XInputLib = LoadLibrary("xinput1_3.dll");
    }


    if (XInputLib)
    {
        INFO("XInputLib loaded");
        //takes the loaded module and name of the function/ procedure address to be technical
        //it return a void* which must be cast
        X_InputGetState_Internal = (X_InputGetState *) GetProcAddress(XInputLib, "XInputGetState");
        X_InputSetState_Internal = (X_InputSetState *) GetProcAddress(XInputLib, "XInputSetState");

        if (!X_InputGetState_Internal || !X_InputSetState_Internal)
        {
            WARN("XInputLib Functions failed to load\n");
        }
    }
    else
    {
        WARN("Failed to load xinput dll\n");
    }
}

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

//up to 4 controllers
static gamepad_state gamepad_system[XUSER_MAX_COUNT];


bool gamepad_button_recieve(event_type code, uint32_t sender_id, uint32_t subscriber_id, event_context data)
{
    DEBUG("%s: RELEASED", gamepad_enum_to_string[data.data.u32[0]]);
    return true;
}

bool gamepad_button_press(event_type code, uint32_t sender_id, uint32_t subscriber_id, event_context data)
{
    DEBUG("%s: PRESSED", gamepad_enum_to_string[data.data.u32[0]]);
    return true;
}

void gamepad_init(void)
{
    x_input_load_dll();

    //set memory to zero
    memset(gamepad_system, 0, sizeof(gamepad_state) * XUSER_MAX_COUNT);

    //TODO: DEBUG WRAP
    event_register(EVENT_GAMEPAD_RELEASE, 0, gamepad_button_recieve);
    event_register(EVENT_GAMEPAD_PRESS, 0, gamepad_button_press);
}

void gamepad_shutdown(void)
{
    //set memory to zero
    memset(gamepad_system, 0, sizeof(gamepad_state) * XUSER_MAX_COUNT);

    //TODO: DEBUG WRAP
    event_unregister(EVENT_GAMEPAD_RELEASE, 0, gamepad_button_recieve);
    event_unregister(EVENT_GAMEPAD_PRESS, 0, gamepad_button_press);
}

void gamepad_poll(void)
{
    //update the prev with the current


    //there can be up to 4 controllers

    // state.dwPacketNumber
    // State packet number. The packet number indicates whether there have been any changes in the state of the controller. If the dwPacketNumber member is the same in sequentially returned XINPUT_STATE structures, the controller state has not changed.
    // state.Gamepad
    // XINPUT_GAMEPAD structure containing the current state of a controller.

    //TODO: move out of the loop
    while (true)
    {
        DWORD dwResult;
        for (DWORD controller_index = 0; controller_index < XUSER_MAX_COUNT; controller_index++)
        {
            //copy the current state to prev, before we update cur state
            memcpy(gamepad_system[controller_index].gamepad_previous, gamepad_system[controller_index].gamepad_current,
                   GAMEPAD_MAX_KEYS);


            XINPUT_STATE gamepad_state;
            ZeroMemory(&gamepad_state, sizeof(XINPUT_STATE));

            // Simply get the state of the controller from XInput.
            dwResult = XInputGetState(controller_index, &gamepad_state);
            // dwResult = X_InputGetState_Internal(controller_index, &gamepad_state);

            if (dwResult == ERROR_SUCCESS)
            {
                // Controller is connected
                // printf("controller connected\n");
                // printf("gamepad A: %d\n", gamepad_system[controller_index].gamepad_current[GAMEPAD_A]);
                // printf("gamepad B: %d\n", gamepad_system[controller_index].gamepad_current[GAMEPAD_B]);
                // printf("%hd, %hd\n", gamepad_state.Gamepad.sThumbLX, gamepad_state.Gamepad.sThumbLX);

                gamepad_system[controller_index].connected = true;

                //bitwise to get the state, returns a 1 or 0/ bool
                gamepad_system[controller_index].gamepad_current[DPAD_UP] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
                gamepad_system[controller_index].gamepad_current[DPAD_DOWN] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                gamepad_system[controller_index].gamepad_current[DPAD_LEFT] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                gamepad_system[controller_index].gamepad_current[DPAD_RIGHT] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_START] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_BACK] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_LEFT_THUMB] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_RIGHT_THUMB] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_LEFT_SHOULDER] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_RIGHT_SHOULDER] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_A] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_B] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_X] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
                gamepad_system[controller_index].gamepad_current[GAMEPAD_Y] = (
                    gamepad_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);

                gamepad_system[controller_index].joy_current.joy_stick_left_x = gamepad_state.Gamepad.sThumbLX;
                gamepad_system[controller_index].joy_current.joy_stick_left_y = gamepad_state.Gamepad.sThumbLY;

                gamepad_system[controller_index].joy_current.joy_stick_right_x = gamepad_state.Gamepad.sThumbRX;
                gamepad_system[controller_index].joy_current.joy_stick_left_x = gamepad_state.Gamepad.sThumbRY;


                for (int i = 0; i < GAMEPAD_MAX_KEYS; i++)
                {
                    if (gamepad_system[controller_index].gamepad_current[i] != gamepad_system[controller_index].
                        gamepad_previous[i])
                    {
                        event_context context;
                        context.data.u32[0] = i; // gamepad enum
                        event_fire(gamepad_system[controller_index].gamepad_current[i]
                                       ? EVENT_GAMEPAD_PRESS
                                       : EVENT_GAMEPAD_RELEASE, 0, context);
                    }
                }
            }
            else
            {
                gamepad_system[controller_index].connected = false;
                // Controller is not connected
                // printf("controller not connected\n");
            }
        }
    }


    // XINPUT_STATE state = g_Controllers[i].state;
    //
    // float LX = state.Gamepad.sThumbLX;
    // float LY = state.Gamepad.sThumbLY;
    //
    // //determine how far the controller is pushed
    // float magnitude = sqrt(LX*LX + LY*LY);
    //
    // //determine the direction the controller is pushed
    // float normalizedLX = LX / magnitude;
    // float normalizedLY = LY / magnitude;
    //
    // float normalizedMagnitude = 0;
    //
    // //check if the controller is outside a circular dead zone
    // if (magnitude > INPUT_DEADZONE)
    // {
    //     //clip the magnitude at its expected maximum value
    //     if (magnitude > 32767) magnitude = 32767;
    //
    //     //adjust magnitude relative to the end of the dead zone
    //     magnitude -= INPUT_DEADZONE;
    //
    //     //optionally normalize the magnitude with respect to its expected range
    //     //giving a magnitude value of 0.0 to 1.0
    //     normalizedMagnitude = magnitude / (32767 - INPUT_DEADZONE);
    // }
    // else //if the controller is in the deadzone zero out the magnitude
    // {
    //     magnitude = 0.0;
    //     normalizedMagnitude = 0.0;
    // }
    //
    // //repeat for right thumb stick
}


void set_gamepad_vibration(uint8_t gamepad_index, unsigned short left_motor_speed, unsigned short right_motor_speed)
{
    //Each motor can use any value between 0-65535 here
    XINPUT_VIBRATION vibration;
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
    vibration.wLeftMotorSpeed = left_motor_speed;
    vibration.wRightMotorSpeed = right_motor_speed;
    XInputSetState(gamepad_index, &vibration);
}

void set_gamepad_vibration_default_on(uint8_t gamepad_index)
{
    set_gamepad_vibration(gamepad_index, 32000, 16000);
}

void set_gamepad_vibration_default_off(uint8_t gamepad_index)
{
    set_gamepad_vibration(gamepad_index, 0, 0);
}


bool gamepad_is_pressed(uint8_t gamepad_id, gamepad_keys gamepad_enum)
{
    return gamepad_system[gamepad_id].gamepad_current[gamepad_enum];
}

bool gamepad_is_released(uint8_t gamepad_id, gamepad_keys gamepad_enum)
{
    return gamepad_system[gamepad_id].gamepad_current[gamepad_enum] == 0;
}

bool gamepad_was_pressed(uint8_t gamepad_id, gamepad_keys gamepad_enum)
{
    return gamepad_system[gamepad_id].gamepad_previous[gamepad_enum] == 1;
}

bool gamepad_was_released(uint8_t gamepad_id, gamepad_keys gamepad_enum)
{
    return gamepad_system[gamepad_id].gamepad_previous[gamepad_enum] == 0;
}

void gamepad_get_left_joy_pos(uint8_t gamepad_id, uint16_t* left_x, uint16_t* left_y)
{
    *left_x = gamepad_system[gamepad_id].joy_current.joy_stick_left_x;
    *left_y = gamepad_system[gamepad_id].joy_current.joy_stick_left_y;
}

void gamepad_get_right_joy_pos(uint8_t gamepad_id, int16_t* left_x, int16_t* left_y)
{
    *left_x = gamepad_system[gamepad_id].joy_current.joy_stick_left_x;
    *left_y = gamepad_system[gamepad_id].joy_current.joy_stick_left_y;
}

void gamepad_get_triggers(uint8_t gamepad_id, int16_t* left_trigger, int16_t* right_trigger)
{
    *left_trigger = gamepad_system[gamepad_id].joy_current.left_trigger;
    *right_trigger = gamepad_system[gamepad_id].joy_current.right_trigger;
}

#endif

#endif //GAMEPAD_H
