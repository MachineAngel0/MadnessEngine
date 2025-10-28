

#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>
#include <stdint.h>


typedef enum buttons {
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_MAX_BUTTONS
} buttons;

typedef struct keyboard_state
{
    bool keys[256]; //all the keys, and max u8 amount
}keyboard_state;

typedef struct mouse_state {
    int16_t x;
    int16_t y;
    uint8_t buttons[MOUSE_BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state {
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;
} input_state;


void input_init(){}
void input_shutdown(){}

void input_update(){}


//mouse related
bool input_is_key_pressed(uint8_t key){}
bool input_is_key_released(uint8_t key){}
bool input_was_key_pressed(uint8_t key){}
bool input_was_key_released(uint8_t key){}

//mouse related
bool input_get_mouse_pos(uint8_t key){}
bool input_get_previous_mouse_pos(uint8_t key){}

bool input_is_mouse_button_pressed(uint8_t key){}
bool input_is_mouse_button_released(uint8_t key){}
bool input_was_mouse_button_pressed(uint8_t key){}
bool input_was_mouse_button_released(uint8_t key){}


#endif //INPUT_H
