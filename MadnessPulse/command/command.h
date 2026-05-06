#ifndef ACTION_H
#define ACTION_H

#include "game_structs.h"
#include "logger.h"
#include "ring_queue.h"
#include "stack.h"
#include "str.h"


Command_Handler* command_handler_init(Madness_Pulse_Game* game)
{
    Command_Handler* ch = allocator_alloc(&game->allocator, sizeof(Command_Handler));
    return ch;
}
void command_handler_destroy(Madness_Pulse_Game* game)
{
    UNIMPLEMENTED();
}


void command_handler_execute_next_action(Command_Handler* command_handler, Madness_Pulse_Game* game)
{
    if (ring_queue_is_empty(command_handler->command_queue)) return;

    ring_dequeue(command_handler->command_queue, command_handler->current_command);
    MASSERT(command_handler->current_command);

    //add to the stack only if we are not in the shipping build and we explicity want to push to the stack
    // if (!UUtilityFunctionLibrary::IsShippingBuild() && PushToStack)
    // {
    // ActionStack.Push(CurrentAction);
    // }

    stack_push(command_handler->command_stack, command_handler->current_command);


    command_handler->current_command->execute(game);

    // OnActionHandlerUpdate.Broadcast(ActionQueue, ActionStack);
}

void command_handler_empty_queue(Command_Handler* command_handler)
{
    ring_queue_empty(command_handler->command_queue);
}


void command_handler_skip_action(Command_Handler* command_handler, Madness_Pulse_Game* game)
{
    if (!command_handler->current_command) return;
    command_handler->current_command->skip(game);
}


void command_handler_empty_stack_memory(Command_Handler* command_handler)
{
    stack_empty(command_handler->command_stack);
    // OnActionHandlerUpdate.Broadcast(ActionQueue, ActionStack);
}

void command_handler_debug_print(Command_Handler* command_handler)
{
    for (int i = 0; i < command_handler->command_stack->num_items; i++)
    {
        Command action;
        memcpy(&action, (u8*)command_handler->command_stack->data + (i * sizeof(Command)), sizeof(Command));
        DEBUG("index: %d, Action Name:", i);
        string_print(&action.name);
    }

    //TODO: print the queue

}

void action_handler_add_action(Command_Handler* command_handler, Command* command)
{
    MASSERT(command_handler)
    MASSERT(command)

    ring_enqueue(command_handler->command_queue, command);
    // OnActionHandlerUpdate.Broadcast(ActionQueue, ActionStack);
}

void action_handler_add_multiple_actions(Command_Handler* command_handler, Command_array* command_array)
{
    MASSERT(command_handler)
    MASSERT(command_array)

    for (int i = 0; i < command_array->num_items; i++)
    {
        ring_enqueue(command_handler->command_queue, &command_array->data[i]);
    }
    // OnActionHandlerUpdate.Broadcast(ActionQueue, ActionStack);
}



#endif //ACTION_H
