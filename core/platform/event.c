#include "event.h"



bool event_init()
{
    INFO("EVENT SYSTEM INIT")

    u64 event_system_mem_requirement = MB(1);
    void* event_system_mem = memory_system_alloc(event_system_mem_requirement);
    arena_init(&event_system_internal.event_system_arena, event_system_mem, event_system_mem_requirement, MEMORY_SUBSYSTEM_EVENT);


    if (is_event_system_init == true) { return false; }

    is_event_system_init = true;
    //zero the memory
    memset(&event_system_internal, 0, sizeof(event_system_internal));

    return true;
}

bool event_shutdown()
{
    INFO("EVENT SYSTEM SHUTDOWN")


    arena_clear(&event_system_internal.event_system_arena);
    // //just free the memory
    // for (int i = 0; i < MAX_EVENTS; i++)
    // {
    //     //some arrays are not initialized
    //     if (event_state.events_table[i].subs_arr)
    //     {
    //         array_free(event_state.events_table[i].subs_arr);
    //         event_state.events_table[i].subs_arr = NULL;
    //     }
    // }

    return true;
}


void event_register(const event_type event, const u32 subscriber, const on_event callback)
{

    if (!is_event_system_init)
    {
        WARN("EVENT REGISTER: SYSTEM NOT INIT")
    }

    //TODO: hook in the linear allocator for this
    //check if the event arr has been alloacated
    if (event_system_internal.events_table[event].subs_arr == NULL)
    {
        event_system_internal.events_table[event].subs_arr = array_create(subscriber_data, MAX_SUBSCRIBERS);
    }


    //TODO: wrap in a debug if/else
    uint32_t registered_count = event_system_internal.events_table[event].subs_arr->num_items;
    for (uint32_t i = 0; i < registered_count; i++)
    {
        subscriber_data* sub_data = (subscriber_data *) array_get(event_system_internal.events_table[event].subs_arr, i);
        if (sub_data->subscriber_id == subscriber)
        {
            WARN("SUBSCRIBER ALREADY REGISTERED TO EVENT");
            return;
        };
    }

    subscriber_data* new_sub_data = malloc(sizeof(subscriber_data));
    new_sub_data->subscriber_id = subscriber;
    new_sub_data->callback = callback;
    array_push(event_system_internal.events_table[event].subs_arr, new_sub_data);
}

void event_unregister(event_type event, uint32_t subscriber, on_event callback)
{
    uint32_t registered_count = event_system_internal.events_table[event].subs_arr->num_items;
    for (uint32_t i = 0; i < registered_count; i++)
    {
        subscriber_data* sub_data = (subscriber_data *) array_get(event_system_internal.events_table[event].subs_arr, i);
        if ((sub_data->subscriber_id == subscriber) && (sub_data->callback == callback))
        {
            array_remove_swap(event_system_internal.events_table[event].subs_arr, i);
        };
    }
}

void event_fire(event_type event, uint32_t sender_id, event_context context)
{
    //check if the event arr has been allocated
    if (event_system_internal.events_table[event].subs_arr == NULL)
    {
        event_system_internal.events_table[event].subs_arr = array_create(subscriber_data, MAX_SUBSCRIBERS);
    }

    for (uint32_t i = 0; i < event_system_internal.events_table[event].subs_arr->num_items; i++)
    {
        //trigger all the callbacks in the event table
        subscriber_data a = *(subscriber_data *) array_get(event_system_internal.events_table[event].subs_arr, i);
        if (a.callback(event, sender_id, a.subscriber_id, context))
        {
            //the subscriber/listener is telling us we want to not fire this off for anyone else
            return;
        };
    }
}

bool test_event(event_type code, uint32_t sender_id, uint32_t subscriber_id, event_context data)
{
    printf("MESSAGE: %c\n", data.data.c[0]);
    return false;
}

bool test_event2(event_type code, uint32_t sender_id, uint32_t subscriber_id, event_context data)
{
    printf("MESSAGE2: %c\n", data.data.c[0]);
    return false;
}


void event_test()
{

    // event_init();

    event_register(EVENT_TEST, 0, test_event);
    event_register(EVENT_TEST, 1, test_event2);

    event_register(EVENT_TEST2, 0, test_event2);
    event_register(EVENT_TEST2, 1, test_event2);

    event_context context;
    context.data.c[0] = 'a';

    event_context context2;
    context2.data.c[0] = 'g';
    event_fire(EVENT_TEST, 0, context);
    event_fire(EVENT_TEST, 0, context2);
    event_fire(EVENT_TEST2, 0, context);

    event_unregister(EVENT_TEST, 0, test_event);

    event_shutdown();
}


