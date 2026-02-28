#ifndef EVENT_H
#define EVENT_H

#include "arena.h"
#include "array.h"
#include "../memory/memory_system.h"


typedef enum event_type
{
    EVENT_APP_QUIT,
    EVENT_APP_RESIZE,
    EVENT_HOT_RELOAD_GAME,
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
typedef bool (*on_event)(event_type code, uint32_t sender_id, uint32_t subscriber_id, event_context data);

//someone that sends events

//list of events -> subscribers -> their respective reactions/callbacks

// can be increased
#define MAX_SUBSCRIBERS 1000 //idk if this is too much per event

typedef struct subscriber_data
{
    //array of senders
    uint32_t subscriber_id;
    on_event callback; //events
} subscriber_data;


typedef struct subscriber_list
{
    Array* subs_arr;
} subscriber_list;

typedef struct event_system
{
    //look up table for events
    //the event if the index into the array
    subscriber_list events_table[MAX_EVENTS];

    Arena event_system_arena;
} Event_System;

//someone that registers to receive the events
static bool is_event_system_init = false;
static Event_System event_system_internal;

bool event_init(Memory_System* memory_system);

bool event_shutdown();

void event_register(const event_type event, const u32 subscriber, const on_event callback);

void event_unregister(event_type event, uint32_t subscriber, on_event callback);

void event_fire(event_type event, uint32_t sender_id, event_context context);

bool test_event(event_type code, uint32_t sender_id, uint32_t subscriber_id, event_context data);

bool test_event2(event_type code, uint32_t sender_id, uint32_t subscriber_id, event_context data);

void event_test();


#endif //EVENT_H
