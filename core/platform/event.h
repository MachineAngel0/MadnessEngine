#ifndef EVENT_H
#define EVENT_H

#include "allocator.h"
#include "darray.h"
#include "memory_system.h"
#include "ring_queue.h"


typedef enum Event_Type
{
    EVENT_APP_QUIT,
    EVENT_APP_RESIZE,
    EVENT_HOT_RELOAD,

    //INPUT
    EVENT_KEY_PRESSED,
    EVENT_KEY_RELEASED,
    EVENT_MOUSE_MOVED,
    EVENT_MOUSE_WHEEL,
    EVENT_MOUSE_PRESSED,
    EVENT_MOUSE_RELEASED,
    //GAMEPAD
    EVENT_GAMEPAD_PRESS,
    EVENT_GAMEPAD_RELEASE,

    //Renderer
    EVENT_HOT_RELOAD_TEXTURE,
    EVENT_HOT_RELOAD_SHADER,


    EVENT_TEST,
    MAX_EVENTS,
} Event_Type;


typedef struct Event_Data_Test
{
    bool yes;
    u32 numbers;
    String words;
} Event_Data_Test;


typedef struct Event_Data_Quit
{
    bool quit;
} Event_Data_Quit;

typedef struct Event_Data_Window_Resize
{
    int width;
    int height;
} Event_Data_Window_Resize;

typedef struct Event_Data_Input_KEY
{
    // keys key;
    u16 key;
} Event_Data_Input_KEY;

typedef struct Event_Data_Input_Mouse_Movement
{
    // keys key;
    u16 x;
    u16 y;
} Event_Data_Input_Mouse_Movement;

typedef struct Event_Data_Input_Mouse_Wheel
{
    // keys key;
    u16 z_delta;
} Event_Data_Input_Mouse_Wheel;

typedef struct Event_Data_Input_Button
{
    // keys key;
    u16 button;
} Event_Data_Input_Button;

typedef struct Event_Data_Gamepad_Button
{
    // keys key;
    u32 button;
} Event_Data_Gamepad_Button;

typedef struct Event_Data
{
    //TODO: just use a proper struct type for the union data, we are not writing a generic engine anyway
    // https://www.youtube.com/watch?v=_LB_wjvTJD0
    // 128 bytes
    union
    {
        Event_Data_Test event_data_test;
        Event_Data_Quit event_data_quit;
        Event_Data_Window_Resize event_data_window_resize;
        Event_Data_Input_KEY event_data_input_key;
        Event_Data_Input_Mouse_Movement event_data_input_mouse_movement;
        Event_Data_Input_Mouse_Wheel event_data_input_mouse_wheel;
        Event_Data_Input_Button event_data_input_button;
        Event_Data_Gamepad_Button event_data_gamepad_button;
    } data;
} Event_Data;

typedef struct Event_Queue_Packet
{
    Event_Type event;
    String sender_name;
    Event_Data context;
} Event_Queue_Packet;


//event code to be implemented by anyone interested
typedef bool (*event_callback)(Event_Type code, String sender_name, String subscriber_name, Event_Data data);

//someone that sends events

//list of events -> subscribers -> their respective reactions/callbacks

// can be increased
#define INITIAL_SUBSCRIBER_SIZE 10 //idk if this is too much per event

typedef struct Subscriber_Data
{
    String subscriber_name;
    event_callback callback; //events
} Subscriber_Data;


typedef struct Subscriber_List
{
    DYNAMIC_ARRAY_TYPE(Subscriber_Data)* subscriber_array;
} Subscriber_List;

typedef struct Event_System
{
    //look up table for events
    //the event if the index into the array
    Subscriber_List events_table[MAX_EVENTS];

    RING_QUEUE_TYPE(Event_Queue_Packet)* event_queue; // TODO:

    Allocator event_system_arena;

    Memory_Tracker* mem_tracker;
} Event_System;

//TODO: event queue

MAPI Event_System* event_init(Memory_System* memory_system);

MAPI bool event_shutdown(Event_System* event_system);

MAPI void event_register(Event_System* event_system, Event_Type event, String subscriber,
                         event_callback callback);

MAPI void event_unregister(Event_System* event_system, Event_Type event, String subscriber, event_callback callback);

MAPI void event_fire(Event_System* event_system, Event_Type event, String sender_name, Event_Data context);

//TODO:
MAPI void event_queue(Event_System* event_system, Event_Queue_Packet event_queue_packet);
MAPI void event_flush_queue(Event_System* event_system);


#endif //EVENT_H
