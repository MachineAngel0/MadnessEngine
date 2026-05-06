#include "event.h"

Event_System* event_init(Memory_System* memory_system)
{
    Event_System* event_system = memory_system_alloc(memory_system, sizeof(Event_System), MEMORY_SUBSYSTEM_EVENT);

    u64 event_system_mem_requirement = MB(1);
    void* event_system_mem = memory_system_alloc(memory_system, event_system_mem_requirement, MEMORY_SUBSYSTEM_EVENT);

    event_system->mem_tracker = memory_system_get_memory_tracker(memory_system->memory_tracker_system,
                                                                 STRING("EVENT SYSTEM"), event_system_mem_requirement);

    allocator_init(&event_system->event_system_arena, event_system_mem, event_system_mem_requirement);

    INFO("EVENT SYSTEM INIT")

    return event_system;
}

bool event_shutdown(Event_System* event_system)
{
    MASSERT(event_system)
    INFO("EVENT SYSTEM SHUTDOWN")


    allocator_clear(&event_system->event_system_arena);

    return true;
}


void event_register(Event_System* event_system, Event_Type event, String subscriber, const event_callback callback)
{
    MASSERT(event_system)

    //TODO: hook in the linear allocator for this
    //check if the event arr has been alloacated
    if (event_system->events_table[event].subscriber_array == NULL)
    {
        event_system->events_table[event].subscriber_array = dynamic_array_create(
            Subscriber_Data, INITIAL_SUBSCRIBER_SIZE, allocator_inferface_create(&event_system->event_system_arena));
    }


    //TODO: wrap in a debug if/else
    uint32_t registered_count = event_system->events_table[event].subscriber_array->num_items;
    for (uint32_t i = 0; i < registered_count; i++)
    {
        // Subscriber_Data* sub_data = (Subscriber_Data*)_dynamic_array_get(event_system->events_table[event].subscriber_array, i);
        Subscriber_Data* sub_data = dynamic_array_get(event_system->events_table[event].subscriber_array,
                                                      Subscriber_Data*, i);
        if (!string_compare(&sub_data->subscriber_name, &subscriber))
        {
            WARN("SUBSCRIBER ALREADY REGISTERED TO EVENT");
            return;
        };
    }

    Subscriber_Data* new_sub_data = malloc(sizeof(Subscriber_Data));
    new_sub_data->subscriber_name = subscriber;
    new_sub_data->callback = callback;
    dynamic_array_push(event_system->events_table[event].subscriber_array, new_sub_data);
}

void event_unregister(Event_System* event_system, Event_Type event, String subscriber, event_callback callback)
{
    MASSERT(event_system)

    uint32_t registered_count = event_system->events_table[event].subscriber_array->num_items;
    for (uint32_t i = 0; i < registered_count; i++)
    {
        // subscriber_data* sub_data = (subscriber_data *) _dynamic_array_get(event_system->events_table[event].subs_arr, i);
        Subscriber_Data* sub_data = dynamic_array_get(event_system->events_table[event].subscriber_array,
                                                      Subscriber_Data*, i);
        if (string_compare(&sub_data->subscriber_name, &subscriber) && (sub_data->callback == callback))
        {
            dynamic_array_remove_swap(event_system->events_table[event].subscriber_array, i);
        }
    }
}

void event_fire(Event_System* event_system, Event_Type event, String sender_name, Event_Data context)
{
    MASSERT(event_system)

    //check if the event array has been allocated
    if (event_system->events_table[event].subscriber_array == NULL)
    {
        event_system->events_table[event].subscriber_array = dynamic_array_create(
            Subscriber_Data, INITIAL_SUBSCRIBER_SIZE, allocator_inferface_create(&event_system->event_system_arena));
    }

    for (uint32_t i = 0; i < event_system->events_table[event].subscriber_array->num_items; i++)
    {
        //trigger all the callbacks in the event table
        Subscriber_Data a = dynamic_array_get(event_system->events_table[event].subscriber_array, Subscriber_Data, i);
        // subscriber_data a = *(subscriber_data*)_array_get(event_system->events_table[event].subs_arr, i);
        if (a.callback(event, sender_name, a.subscriber_name, context))
        {
            //the subscriber/listener is telling us we want to not fire this off for anyone else
            return;
        };
    }
}

void event_queue(Event_System* event_system, Event_Queue_Packet event_queue_packet)
{
    ring_enqueue(event_system->event_queue, &event_queue_packet);
}

void event_flush_queue(Event_System* event_system)
{
    while (!ring_queue_is_empty(event_system->event_queue))
    {
        Event_Queue_Packet data;
        ring_dequeue(event_system->event_queue, &data);

        for (uint32_t i = 0; i < event_system->events_table[data.event].subscriber_array->num_items; i++)
        {
            //trigger all the callbacks in the event table
            Subscriber_Data a = dynamic_array_get(event_system->events_table[data.event].subscriber_array, Subscriber_Data, i);
            // subscriber_data a = *(subscriber_data*)_array_get(event_system->events_table[event].subs_arr, i);
            if (a.callback(data.event, data.sender_name, a.subscriber_name, data.context))
            {
                //the subscriber/listener is telling us we want to not fire this off for anyone else
                return;
            }
        }
    }
}


bool test_event(Event_Type code, String sender_name, String subscriber_name, Event_Data data)
{
    printf("i am the event, the awakening");
    return true;
}

void event_test(Event_System* event_system)
{
    MASSERT(event_system)

    // event_init();

    event_register(event_system, EVENT_TEST, STRING("Sub1"), test_event);


    Event_Data_Test test = {.yes = true, .numbers = 420, .words = STRING("Words")};

    Event_Data context;
    context.data.event_data_test.yes = true;
    context.data.event_data_test.numbers = 420;
    context.data.event_data_test.words = STRING("Words");

    Event_Data context2;
    context2.data.event_data_test = test;


    event_fire(event_system, EVENT_TEST, STRING("I am the sender"), context);
    event_unregister(event_system, EVENT_TEST, STRING("I am the sender"), test_event);

    event_shutdown(event_system);
}
