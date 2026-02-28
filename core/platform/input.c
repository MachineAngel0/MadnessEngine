#include "input.h"

#include "arena.h"
#include "logger.h"
#include "event.h"

bool input_init(Input_System* input_system,Memory_System* memory_system)
{
    MASSERT(input_system);

    //NOTE: the only reason you would want this is the input is expected to be initalized in here
    // memset(&input_system, 0, sizeof(Input_System));

    u64 input_system_mem_requirement = MB(1);
    void* input_system_mem = memory_system_alloc(memory_system, input_system_mem_requirement);
    arena_init(&input_system->input_system_arena, input_system_mem, input_system_mem_requirement, MEMORY_SUBSYSTEM_INPUT);


    return true;
}

void input_shutdown(Input_System* input_system)
{
    MASSERT(input_system);
    //TODO: if needed
    INFO("INPUT SYSTEM SHUTDOWN")
    arena_clear(&input_system->input_system_arena);
}

void input_update(Input_System* input_system)
{
    MASSERT(input_system);
    // Copy current states to previous states.
    memcpy(&input_system->keyboard_previous, &input_system->keyboard_current, sizeof(keyboard_state));
    memcpy(&input_system->mouse_previous, &input_system->mouse_current, sizeof(mouse_state));
}


void input_process_key(Input_System* input_system,keys key, bool pressed)
{
    MASSERT(input_system);
    // Only handle this if the state actually changed.
    if (input_system->keyboard_current.keys[key] != pressed)
    {
        // Update internal state.
        input_system->keyboard_current.keys[key] = pressed;

        // Fire off an event for immediate processing.
        event_context context;
        context.data.u16[0] = key;
        event_fire(pressed ? EVENT_KEY_PRESSED : EVENT_KEY_RELEASED, 0, context);
    }
}

void input_process_mouse_move(Input_System* input_system,i16 x, i16 y)
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
        event_context context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        event_fire(EVENT_MOUSE_MOVED, 0, context);
    }
}

void input_process_mouse_wheel(Input_System* input_system,i8 z_delta)
{
    MASSERT(input_system);
    // NOTE: no internal state to update.

    // Fire the event.
    event_context context;
    context.data.u8[0] = z_delta;
    event_fire(EVENT_MOUSE_WHEEL, 0, context);
}

void input_process_mouse_button(Input_System* input_system,mouse_buttons button, bool pressed)
{
    MASSERT(input_system);

    // Only handle this if the state actually changed.
    if (input_system->mouse_current.buttons[button] != pressed)
    {
        // Update internal state.
        input_system->mouse_current.buttons[button] = pressed;

        // Fire off an event for immediate processing.
        event_context context;
        context.data.u16[0] = button;
        event_fire(pressed ? EVENT_MOUSE_PRESSED : EVENT_MOUSE_RELEASED, 0, context);
    }
}


//key related


bool input_is_key_pressed(Input_System* input_system,uint8_t key)
{
    MASSERT(input_system);
    return input_system->keyboard_current.keys[key] == true;
}

//NOTE: this literally means the key isn't pressed, not that it was just released
bool input_is_key_released(Input_System* input_system,uint8_t key)
{
    MASSERT(input_system);
    return input_system->keyboard_current.keys[key] == false;
}

bool input_was_key_pressed(Input_System* input_system,uint8_t key)
{
    MASSERT(input_system);
    return input_system->keyboard_previous.keys[key] == true;
}

bool input_was_key_released(Input_System* input_system,uint8_t key)
{
    MASSERT(input_system);
    return input_system->keyboard_previous.keys[key] == true;
}

//checking for a one time press
bool input_key_pressed_unique(Input_System* input_system,uint8_t key)
{
    MASSERT(input_system);
    //if is released this frame (aka false) and was pressed last frame
    return input_is_key_pressed(input_system, key) &&
           input_was_key_released(input_system, key);
}

//checking for a one time release
bool input_key_released_unique(Input_System* input_system,uint8_t key)
{
    MASSERT(input_system);
    return input_was_key_pressed(input_system, key) &&
          input_is_key_released(input_system, key);
}


//mouse related
void input_get_mouse_pos(Input_System* input_system,i16* out_x, i16* out_y)
{
    MASSERT(input_system);
    *out_x = input_system->mouse_current.x;
    *out_y = input_system->mouse_current.y;
}

void input_get_previous_mouse_pos(Input_System* input_system,i16* out_x, i16* out_y)
{
    MASSERT(input_system);
    *out_x = input_system->mouse_previous.x;
    *out_y = input_system->mouse_previous.y;
}

void input_get_mouse_change(Input_System* input_system,i16* out_x, i16* out_y)
{
    MASSERT(input_system);
    *out_x = input_system->mouse_current.x - input_system->mouse_previous.x ;
    *out_y = input_system->mouse_current.y - input_system->mouse_previous.y;
    // DEBUG("MOUSE CHANGE X: %d, MOUSE CHANGE Y: %d", *out_x, *out_y);
}


bool input_is_mouse_button_pressed(Input_System* input_system,mouse_buttons key)
{
    MASSERT(input_system);
    return input_system->mouse_current.buttons[key] == true;
}

bool input_is_mouse_button_released(Input_System* input_system,mouse_buttons key)
{
    MASSERT(input_system);
    return input_system->mouse_current.buttons[key] == false;
}

bool input_was_mouse_button_pressed(Input_System* input_system,mouse_buttons key)
{
    MASSERT(input_system);
    return input_system->mouse_previous.buttons[key] == true;
}

bool input_was_mouse_button_released(Input_System* input_system,mouse_buttons key)
{
    MASSERT(input_system);
    return input_system->mouse_previous.buttons[key] == false;
}


