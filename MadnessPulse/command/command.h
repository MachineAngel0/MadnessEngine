#ifndef ACTION_H
#define ACTION_H

#include "game_enums.h"
#include "game_structs.h"
#include "logger.h"
#include "ring_queue.h"
#include "stack.h"
#include "str.h"

//ACTION INTERFACE:
typedef void (*action_execute)(Madness_Pulse_Game*);
typedef void (*action_skip)(Madness_Pulse_Game*);

typedef struct command
{
    action_execute execute;
    action_skip skip;
    String name; // TODO: this might just be an enum type instead
    bool is_skippable; //TODO:

    //TODO: figure this out when you have an animation system in place
    // FQueueTimer QueueTimer;

    // void ExecuteNextAction();
    // void StartQueueTimer();

    // void ExecuteNextActionWithActionOverride(ATurnBasedGameMode* GameMode);
    // void StartQueueTimerWithOverride(ATurnBasedGameMode* GameMode);
    // void BeforeExecuteNextActionOverride(ATurnBasedGameMode* GameMode);
} Command;


typedef struct Command_Handler
{
    /*FUNCTIONS AND VARIABLES*/

    //TYPE: Action
    ring_queue* command_queue;
    Command* current_command;

    // our already processed actions
    stack* command_stack;

    //set this to true to have the items used pushed to the stack
    // bool push_to_stack = false; //NOTE: idk what this is for
} Command_Handler;


void command_handler_create(Command_Handler* action_handler);
void command_handler_destroy(Command_Handler* action_handler);


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


#endif //ACTION_H
