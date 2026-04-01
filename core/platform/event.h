#ifndef EVENT_H
#define EVENT_H

#include "arena.h"
#include "array.h"
#include "memory_system.h"


typedef enum event_type
{
    EVENT_APP_QUIT,
    EVENT_APP_RESIZE,
    EVENT_HOT_RELOAD,
    EVENT_TEST,
    EVENT_TEST2,

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



    MAX_EVENTS,
} event_type;

typedef struct event_context
{
    // 128 bytes
    union
    {
        int64_t i64[2];
        uint64_t u64[2];
        double f64[2];

        int32_t i32[4];
        uint32_t u32[4];
        float f32[4];

        int16_t i16[8];
        uint16_t u16[8];

        int8_t i8[16];
        uint8_t u8[16];

        char c[16];
    } data;
} event_context;


//event code to be implemented by anyone interested
typedef bool (*on_event)(event_type code, u32 sender_id, u32 subscriber_id, event_context data);

//someone that sends events

//list of events -> subscribers -> their respective reactions/callbacks

// can be increased
#define MAX_SUBSCRIBERS 1000 //idk if this is too much per event

typedef struct subscriber_data
{
    //array of senders
    u32 subscriber_id;
    on_event callback; //events
} subscriber_data;


typedef struct subscriber_list
{
    Array* subs_arr;
} subscriber_list;

typedef struct Event_System
{
    //look up table for events
    //the event if the index into the array
    subscriber_list events_table[MAX_EVENTS];

    Arena event_system_arena;

    Memory_Tracker* mem_tracker;
} Event_System;



bool event_init(Event_System* event_system, Memory_System* memory_system);

bool event_shutdown(Event_System* event_system);

void event_register(Event_System* event_system, const event_type event, const u32 subscriber, const on_event callback);

void event_unregister(Event_System* event_system, event_type event, u32 subscriber, on_event callback);

void event_fire(Event_System* event_system, event_type event, u32 sender_id, event_context context);

bool test_event(event_type code, u32 sender_id, u32 subscriber_id, event_context data);

bool test_event2(event_type code, u32 sender_id, u32 subscriber_id, event_context data);

void event_test(Event_System* event_system);


#endif //EVENT_H
