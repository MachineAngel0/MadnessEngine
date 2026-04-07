#include "event.h"

Event_System* event_init(Memory_System* memory_system)
{
    Event_System* event_system = memory_system_alloc(memory_system, sizeof(Event_System), MEMORY_SUBSYSTEM_EVENT);

    u64 event_system_mem_requirement = MB(1);
    void* event_system_mem = memory_system_alloc(memory_system, event_system_mem_requirement, MEMORY_SUBSYSTEM_EVENT);

    event_system->mem_tracker = memory_system_get_memory_tracker(memory_system->memory_tracker_system, STRING("EVENT SYSTEM"), event_system_mem_requirement);
    
    arena_init(&event_system->event_system_arena, event_system_mem, event_system_mem_requirement, event_system->mem_tracker);
    
    INFO("EVENT SYSTEM INIT")

    return event_system;
}

bool event_shutdown(Event_System* event_system)
{
    MASSERT(event_system)
    INFO("EVENT SYSTEM SHUTDOWN")


    arena_clear(&event_system->event_system_arena);

    return true;
}


void event_register(Event_System* event_system, const event_type event, const u32 subscriber, const on_event callback)
{

    MASSERT(event_system)

    //TODO: hook in the linear allocator for this
    //check if the event arr has been alloacated
    if (event_system->events_table[event].subs_arr == NULL)
    {
        event_system->events_table[event].subs_arr = array_create(subscriber_data, MAX_SUBSCRIBERS);
    }


    //TODO: wrap in a debug if/else
    uint32_t registered_count = event_system->events_table[event].subs_arr->num_items;
    for (uint32_t i = 0; i < registered_count; i++)
    {
        subscriber_data* sub_data = (subscriber_data *) array_get(event_system->events_table[event].subs_arr, i);
        if (sub_data->subscriber_id == subscriber)
        {
            WARN("SUBSCRIBER ALREADY REGISTERED TO EVENT");
            return;
        };
    }

    subscriber_data* new_sub_data = malloc(sizeof(subscriber_data));
    new_sub_data->subscriber_id = subscriber;
    new_sub_data->callback = callback;
    array_push(event_system->events_table[event].subs_arr, new_sub_data);
}

void event_unregister(Event_System* event_system, event_type event, u32 subscriber, on_event callback)
{

    MASSERT(event_system)

    uint32_t registered_count = event_system->events_table[event].subs_arr->num_items;
    for (uint32_t i = 0; i < registered_count; i++)
    {
        subscriber_data* sub_data = (subscriber_data *) array_get(event_system->events_table[event].subs_arr, i);
        if ((sub_data->subscriber_id == subscriber) && (sub_data->callback == callback))
        {
            array_remove_swap(event_system->events_table[event].subs_arr, i);
        };
    }
}

void event_fire(Event_System* event_system, event_type event, u32 sender_id, event_context context)
{

    MASSERT(event_system)

    //check if the event arr has been allocated
    if (event_system->events_table[event].subs_arr == NULL)
    {
        event_system->events_table[event].subs_arr = array_create(subscriber_data, MAX_SUBSCRIBERS);
    }

    for (uint32_t i = 0; i < event_system->events_table[event].subs_arr->num_items; i++)
    {
        //trigger all the callbacks in the event table
        subscriber_data a = *(subscriber_data *) array_get(event_system->events_table[event].subs_arr, i);
        if (a.callback(event, sender_id, a.subscriber_id, context))
        {
            //the subscriber/listener is telling us we want to not fire this off for anyone else
            return;
        };
    }
}

bool test_event(event_type code, u32 sender_id, u32 subscriber_id, event_context data)
{
    printf("MESSAGE: %c\n", data.data.c[0]);
    return false;
}

bool test_event2(event_type code, u32 sender_id, u32 subscriber_id, event_context data)
{
    printf("MESSAGE2: %c\n", data.data.c[0]);
    return false;
}


void event_test(Event_System* event_system)
{
    MASSERT(event_system)

    // event_init();

    event_register(event_system, EVENT_TEST, 0, test_event);
    event_register(event_system, EVENT_TEST, 1, test_event2);

    event_register(event_system, EVENT_TEST2, 0, test_event2);
    event_register(event_system, EVENT_TEST2, 1, test_event2);

    event_context context;
    context.data.c[0] = 'a';

    event_context context2;
    context2.data.c[0] = 'g';
    event_fire(event_system, EVENT_TEST, 0, context);
    event_fire(event_system, EVENT_TEST, 0, context2);
    event_fire(event_system, EVENT_TEST2, 0, context);

    event_unregister(event_system, EVENT_TEST, 0, test_event);

    event_shutdown(event_system);
}


