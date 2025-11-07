#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>
#include <stdint.h>
#include "event.h"


typedef enum mouse_buttons
{
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_MAX_BUTTONS
} mouse_buttons;


#define DEFINE_KEY(name, code) KEY_##name = code

typedef enum keys {
    DEFINE_KEY(BACKSPACE, 0x08),
    DEFINE_KEY(ENTER, 0x0D),
    DEFINE_KEY(TAB, 0x09),
    DEFINE_KEY(SHIFT, 0x10),
    DEFINE_KEY(CONTROL, 0x11),

    DEFINE_KEY(PAUSE, 0x13),
    DEFINE_KEY(CAPITAL, 0x14),

    DEFINE_KEY(ESCAPE, 0x1B),

    DEFINE_KEY(CONVERT, 0x1C),
    DEFINE_KEY(NONCONVERT, 0x1D),
    DEFINE_KEY(ACCEPT, 0x1E),
    DEFINE_KEY(MODECHANGE, 0x1F),

    DEFINE_KEY(SPACE, 0x20),
    DEFINE_KEY(PRIOR, 0x21),
    DEFINE_KEY(NEXT, 0x22),
    DEFINE_KEY(END, 0x23),
    DEFINE_KEY(HOME, 0x24),
    DEFINE_KEY(LEFT, 0x25),
    DEFINE_KEY(UP, 0x26),
    DEFINE_KEY(RIGHT, 0x27),
    DEFINE_KEY(DOWN, 0x28),
    DEFINE_KEY(SELECT, 0x29),
    DEFINE_KEY(PRINT, 0x2A),
    DEFINE_KEY(EXECUTE, 0x2B),
    DEFINE_KEY(SNAPSHOT, 0x2C),
    DEFINE_KEY(INSERT, 0x2D),
    DEFINE_KEY(DELETE, 0x2E),
    DEFINE_KEY(HELP, 0x2F),

    DEFINE_KEY(A, 0x41),
    DEFINE_KEY(B, 0x42),
    DEFINE_KEY(C, 0x43),
    DEFINE_KEY(D, 0x44),
    DEFINE_KEY(E, 0x45),
    DEFINE_KEY(F, 0x46),
    DEFINE_KEY(G, 0x47),
    DEFINE_KEY(H, 0x48),
    DEFINE_KEY(I, 0x49),
    DEFINE_KEY(J, 0x4A),
    DEFINE_KEY(K, 0x4B),
    DEFINE_KEY(L, 0x4C),
    DEFINE_KEY(M, 0x4D),
    DEFINE_KEY(N, 0x4E),
    DEFINE_KEY(O, 0x4F),
    DEFINE_KEY(P, 0x50),
    DEFINE_KEY(Q, 0x51),
    DEFINE_KEY(R, 0x52),
    DEFINE_KEY(S, 0x53),
    DEFINE_KEY(T, 0x54),
    DEFINE_KEY(U, 0x55),
    DEFINE_KEY(V, 0x56),
    DEFINE_KEY(W, 0x57),
    DEFINE_KEY(X, 0x58),
    DEFINE_KEY(Y, 0x59),
    DEFINE_KEY(Z, 0x5A),

    DEFINE_KEY(LWIN, 0x5B),
    DEFINE_KEY(RWIN, 0x5C),
    DEFINE_KEY(APPS, 0x5D),

    DEFINE_KEY(SLEEP, 0x5F),

    DEFINE_KEY(NUMPAD0, 0x60),
    DEFINE_KEY(NUMPAD1, 0x61),
    DEFINE_KEY(NUMPAD2, 0x62),
    DEFINE_KEY(NUMPAD3, 0x63),
    DEFINE_KEY(NUMPAD4, 0x64),
    DEFINE_KEY(NUMPAD5, 0x65),
    DEFINE_KEY(NUMPAD6, 0x66),
    DEFINE_KEY(NUMPAD7, 0x67),
    DEFINE_KEY(NUMPAD8, 0x68),
    DEFINE_KEY(NUMPAD9, 0x69),
    DEFINE_KEY(MULTIPLY, 0x6A),
    DEFINE_KEY(ADD, 0x6B),
    DEFINE_KEY(SEPARATOR, 0x6C),
    DEFINE_KEY(SUBTRACT, 0x6D),
    DEFINE_KEY(DECIMAL, 0x6E),
    DEFINE_KEY(DIVIDE, 0x6F),
    DEFINE_KEY(F1, 0x70),
    DEFINE_KEY(F2, 0x71),
    DEFINE_KEY(F3, 0x72),
    DEFINE_KEY(F4, 0x73),
    DEFINE_KEY(F5, 0x74),
    DEFINE_KEY(F6, 0x75),
    DEFINE_KEY(F7, 0x76),
    DEFINE_KEY(F8, 0x77),
    DEFINE_KEY(F9, 0x78),
    DEFINE_KEY(F10, 0x79),
    DEFINE_KEY(F11, 0x7A),
    DEFINE_KEY(F12, 0x7B),
    DEFINE_KEY(F13, 0x7C),
    DEFINE_KEY(F14, 0x7D),
    DEFINE_KEY(F15, 0x7E),
    DEFINE_KEY(F16, 0x7F),
    DEFINE_KEY(F17, 0x80),
    DEFINE_KEY(F18, 0x81),
    DEFINE_KEY(F19, 0x82),
    DEFINE_KEY(F20, 0x83),
    DEFINE_KEY(F21, 0x84),
    DEFINE_KEY(F22, 0x85),
    DEFINE_KEY(F23, 0x86),
    DEFINE_KEY(F24, 0x87),

    DEFINE_KEY(NUMLOCK, 0x90),
    DEFINE_KEY(SCROLL, 0x91),

    DEFINE_KEY(NUMPAD_EQUAL, 0x92),

    DEFINE_KEY(LSHIFT, 0xA0),
    DEFINE_KEY(RSHIFT, 0xA1),
    DEFINE_KEY(LCONTROL, 0xA2),
    DEFINE_KEY(RCONTROL, 0xA3),
    DEFINE_KEY(LMENU, 0xA4),
    DEFINE_KEY(RMENU, 0xA5),

    DEFINE_KEY(SEMICOLON, 0xBA),
    DEFINE_KEY(PLUS, 0xBB),
    DEFINE_KEY(COMMA, 0xBC),
    DEFINE_KEY(MINUS, 0xBD),
    DEFINE_KEY(PERIOD, 0xBE),
    DEFINE_KEY(SLASH, 0xBF),
    DEFINE_KEY(GRAVE, 0xC0),

    KEYS_MAX_KEYS
} keys;


typedef struct keyboard_state
{
    bool keys[256]; //all the keys, and max u8 amount
} keyboard_state;

typedef struct mouse_state
{
    int16_t x;
    int16_t y;
    uint8_t buttons[MOUSE_BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state
{
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;
} input_state;


static input_state key_state;


bool input_init()
{
    memset(&key_state, 0, sizeof(input_state));
    return true;
}

void input_shutdown()
{
    //TODO: if needed
    INFO("INPUT SYSTEM SHUTDOWN")
}

void input_update()
{
    // Copy current states to previous states.
    memcpy(&key_state.keyboard_previous, &key_state.keyboard_current, sizeof(keyboard_state));
    memcpy(&key_state.mouse_previous, &key_state.mouse_current, sizeof(mouse_state));
}


bool input_process_key(uint8_t key, bool pressed)
{
    // Only handle this if the state actually changed.


    if (key_state.keyboard_current.keys[key] != pressed)
    {
        // Update internal state.
        key_state.keyboard_current.keys[key] = pressed;
        // Fire off an event for immediate processing.
        event_context context;
        context.data.u16[0] = key;
        event_fire(pressed ? EVENT_KEY_PRESSED : EVENT_KEY_RELEASED, 0, context);
    }
}

void input_process_mouse_move(i16 x, i16 y)
{
    // Only process if actually different
    if (key_state.mouse_current.x != x || key_state.mouse_current.y != y) {
        // NOTE: Enable this if debugging.
        //KDEBUG("Mouse pos: %i, %i!", x, y);

        // Update internal state.
        key_state.mouse_current.x = x;
        key_state.mouse_current.y = y;

        // Fire the event.
        event_context context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        event_fire(EVENT_MOUSE_MOVED, 0, context);
    }
}

void input_process_mouse_wheel(i8 z_delta) {
    // NOTE: no internal state to update.

    // Fire the event.
    event_context context;
    context.data.u8[0] = z_delta;
    event_fire(EVENT_MOUSE_WHEEL, 0, context);
}


//key related
bool input_is_key_pressed(uint8_t key)
{
}

bool input_is_key_released(uint8_t key)
{
}

bool input_was_key_pressed(uint8_t key)
{
}

bool input_was_key_released(uint8_t key)
{
}

//mouse related
bool input_get_mouse_pos(uint8_t key)
{
}

bool input_get_previous_mouse_pos(uint8_t key)
{
}

bool input_is_mouse_button_pressed(uint8_t key)
{
}

bool input_is_mouse_button_released(uint8_t key)
{
}

bool input_was_mouse_button_pressed(uint8_t key)
{
}

bool input_was_mouse_button_released(uint8_t key)
{
}


#endif //INPUT_H
