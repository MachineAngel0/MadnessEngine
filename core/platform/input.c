#include "input.h"

#include "allocator.h"
#include "logger.h"
#include "event.h"
#include "platform.h"

Input_System* input_init(Memory_System* memory_system)
{

    input_system = memory_system_alloc(memory_system, sizeof(Input_System), MEMORY_SUBSYSTEM_INPUT);



    return input_system;
}

void input_shutdown(void)
{
    MASSERT(input_system);
    //TODO: if needed
    INFO("INPUT SYSTEM SHUTDOWN")
}

void input_update(void)
{
    MASSERT(input_system);
    // Copy current states to previous states.
    memcpy(&input_system->keyboard_previous, &input_system->keyboard_current, sizeof(keyboard_state));
    memcpy(&input_system->mouse_previous, &input_system->mouse_current, sizeof(mouse_state));

    //NOTE: as this only records the delta, we have no way of knowing if nothing happened, hence we set it to 0
    //and only check the previous frame for how the wheel moved
    input_system->mouse_current.mouse_wheel_delta = 0;
}


void input_process_key(keys key, bool pressed)
{
    MASSERT(input_system);
    // Only handle this if the state actually changed.
    if (input_system->keyboard_current.keys[key] != pressed)
    {
        // Update internal state.
        input_system->keyboard_current.keys[key] = pressed;

        // Fire off an event for immediate processing.
        Event_Data context;
        context.data.event_data_input_key.key = key;
        event_fire(pressed ? EVENT_KEY_PRESSED : EVENT_KEY_RELEASED, STRING("Input_System"), context);
    }
}

void input_process_mouse_move(s16 x, s16 y)
{
    MASSERT(input_system);
    // Only process if actually different
    if (input_system->mouse_current.x != x || input_system->mouse_current.y != y)
    {
        // NOTE: Enable this if debugging.
        //DEBUG("Mouse pos: %i, %i!", x, y);

        // Update internal state.
        input_system->mouse_current.x = x;
        input_system->mouse_current.y = y;

        // Fire the event.
        Event_Data context;
        context.data.event_data_input_mouse_movement.x = x;
        context.data.event_data_input_mouse_movement.y = y;
        event_fire(EVENT_MOUSE_MOVED, STRING("Input_System"), context);
    }
}

void input_process_mouse_wheel(s8 z_delta)
{
    MASSERT(input_system);
    // DEBUG("Z DELTA: %d", z_delta)
    // FATAL("Z DELTA: %d", z_delta)
    input_system->mouse_current.mouse_wheel_delta = z_delta;
    // Fire the event.
    Event_Data context;
    context.data.event_data_input_mouse_wheel.z_delta = z_delta;
    event_fire(EVENT_MOUSE_WHEEL, STRING("Input_System"), context);
}

void input_process_mouse_button(mouse_buttons button, bool pressed)
{
    MASSERT(input_system);

    // Only handle this if the state actually changed.
    if (input_system->mouse_current.buttons[button] != pressed)
    {
        // Update internal state.
        input_system->mouse_current.buttons[button] = pressed;

        // Fire off an event for immediate processing.
        Event_Data context;
        context.data.event_data_input_button.button = button;
        event_fire(pressed ? EVENT_MOUSE_PRESSED : EVENT_MOUSE_RELEASED, STRING("Input_System"), context);
    }
}


//key related


bool input_is_key_pressed(uint8_t key)
{
    MASSERT(input_system);
    return input_system->keyboard_current.keys[key] == true;
}

//NOTE: this literally means the key isn't pressed, not that it was just released
bool input_is_key_released(uint8_t key)
{
    MASSERT(input_system);
    return input_system->keyboard_current.keys[key] == false;
}

bool input_was_key_pressed(uint8_t key)
{
    MASSERT(input_system);
    return input_system->keyboard_previous.keys[key] == true;
}

bool input_was_key_released(uint8_t key)
{
    MASSERT(input_system);
    return input_system->keyboard_previous.keys[key] == true;
}

//checking for a one time press
bool input_key_pressed_unique(uint8_t key)
{
    MASSERT(input_system);
    //if is released this frame (aka false) and was pressed last frame
    return input_is_key_pressed(key) &&
        input_was_key_released(key);
}

//checking for a one time release
bool input_key_released_unique(uint8_t key)
{
    MASSERT(input_system);
    return input_was_key_pressed(key) &&
        input_is_key_released(key);
}

char input_get_first_released_key(void)
{
    for (int i = 0; i < MAX_KEY_COUNT; i++)
    {
        if (input_key_released_unique(i))
        {
            return input_key_to_char(i);
        }
    }

    return 0;
}

char input_key_to_char(keys key)
{
    keys shift = input_system->keyboard_current.keys[KEY_LSHIFT];
    keys caps = input_system->keyboard_current.keys[KEY_CAPITAL];

    // Letters
    if (key >= KEY_A && key <= KEY_Z)
    {
        char c = 'a' + (key - KEY_A);

        if (shift ^ caps) // XOR: one or the other
            c -= 32;

        return c;
    }

    // Numbers row
    switch (key)
    {
    case KEY_0: return shift ? ')' : '0';
    case KEY_1: return shift ? '!' : '1';
    case KEY_2: return shift ? '@' : '2';
    case KEY_3: return shift ? '#' : '3';
    case KEY_4: return shift ? '$' : '4';
    case KEY_5: return shift ? '%' : '5';
    case KEY_6: return shift ? '^' : '6';
    case KEY_7: return shift ? '&' : '7';
    case KEY_8: return shift ? '*' : '8';
    case KEY_9: return shift ? '(' : '9';
    }

    // Numpad (no shift variants typically)
    if (key >= KEY_NUMPAD0 && key <= KEY_NUMPAD9)
    {
        return '0' + (key - KEY_NUMPAD0);
    }

    switch (key)
    {
    case KEY_SPACE: return ' ';
    case KEY_TAB: return '\t';
    case KEY_ENTER: return '\n';

    // Symbols
    case KEY_MINUS: return shift ? '_' : '-';
    case KEY_PLUS: return shift ? '+' : '=';
    case KEY_COMMA: return shift ? '<' : ',';
    case KEY_PERIOD: return shift ? '>' : '.';
    case KEY_SLASH: return shift ? '?' : '/';
    case KEY_SEMICOLON: return shift ? ':' : ';';
    case KEY_GRAVE: return shift ? '~' : '`';

    // These depend on your enum completeness
    // Add if you define them:
    // case KEY_LBRACKET: return shift ? '{' : '[';
    // case KEY_RBRACKET: return shift ? '}' : ']';
    // case KEY_BACKSLASH: return shift ? '|' : '\\';
    // case KEY_APOSTROPHE: return shift ? '"' : '\'';

    case KEY_MULTIPLY: return '*';
    case KEY_ADD: return '+';
    case KEY_SUBTRACT: return '-';
    case KEY_DIVIDE: return '/';
    case KEY_DECIMAL: return '.';
    }

    return 0; // not a printable key
}


//mouse related
void input_get_mouse_pos(s16* out_x, s16* out_y)
{
    MASSERT(input_system);
    *out_x = input_system->mouse_current.x;
    *out_y = input_system->mouse_current.y;
}

void input_get_previous_mouse_pos(s16* out_x, s16* out_y)
{
    MASSERT(input_system);
    *out_x = input_system->mouse_previous.x;
    *out_y = input_system->mouse_previous.y;
}

void input_get_mouse_change(s16* out_x, s16* out_y)
{
    MASSERT(input_system);
    *out_x = input_system->mouse_current.x - input_system->mouse_previous.x;
    *out_y = input_system->mouse_current.y - input_system->mouse_previous.y;
    // DEBUG("MOUSE CHANGE X: %d, MOUSE CHANGE Y: %d", *out_x, *out_y);
}

void input_set_cursor_pos(int x, int y)
{
    //TODO: have a check to make sure we are within the window coordinates
    //otherwise, this can set the position to anywhere it wants on the screen, even outside the window
    platform_set_cursor_pos(x,y);
}


bool input_is_mouse_button_pressed(mouse_buttons key)
{
    MASSERT(input_system);
    return input_system->mouse_current.buttons[key] == true;
}

bool input_is_mouse_button_released(mouse_buttons key)
{
    MASSERT(input_system);
    return input_system->mouse_current.buttons[key] == false;
}

bool input_was_mouse_button_pressed(mouse_buttons key)
{
    MASSERT(input_system);
    return input_system->mouse_previous.buttons[key] == true;
}

bool input_was_mouse_button_released(mouse_buttons key)
{
    MASSERT(input_system);
    return input_system->mouse_previous.buttons[key] == false;
}

bool input_is_mouse_button_pressed_unique(mouse_buttons key)
{
    //check that it was released last frame and pressed this frame
    return
        input_system->mouse_previous.buttons[key] == MOUSE_BUTTON_RELEASED &&
        input_system->mouse_current.buttons[key] == MOUSE_BUTTON_PRESSED;
}

bool input_is_mouse_button_released_unique(mouse_buttons key)
{
    //check that it was pressed last frame and released this frame
    return
        input_system->mouse_previous.buttons[key] == MOUSE_BUTTON_PRESSED &&
        input_system->mouse_current.buttons[key] == MOUSE_BUTTON_RELEASED;
}

bool input_has_mouse_wheel_changed_this_frame(void)
{
    return input_system->mouse_current.mouse_wheel_delta != 0;
}

bool input_has_mouse_wheel_changed_last_frame(void)
{
    return input_system->mouse_previous.mouse_wheel_delta != 0;
}

void input_get_mouse_wheel_value(s32* z_delta)
{
    MASSERT(input_system);
    *z_delta = input_system->mouse_previous.mouse_wheel_delta;
}

bool input_is_mouse_wheel_up(void)
{
    return input_system->mouse_previous.mouse_wheel_delta == MOUSE_WHEEL_UP;
}

bool input_is_mouse_wheel_down(void)
{
    return input_system->mouse_previous.mouse_wheel_delta == MOUSE_WHEEL_DOWN;
}
