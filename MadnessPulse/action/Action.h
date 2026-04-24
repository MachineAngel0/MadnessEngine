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

typedef struct Action
{
    action_execute execute;
    action_skip skip;
    String name; // TODO: this might just be an enum type instead

    //TODO: figure this out when you have an animation system in place
    // FQueueTimer QueueTimer;

    // void ExecuteNextAction();
    // void StartQueueTimer();

    // void ExecuteNextActionWithActionOverride(ATurnBasedGameMode* GameMode);
    // void StartQueueTimerWithOverride(ATurnBasedGameMode* GameMode);
    // void BeforeExecuteNextActionOverride(ATurnBasedGameMode* GameMode);
} Action;


typedef struct Action_Handler
{
    /*FUNCTIONS AND VARIABLES*/

    //TYPE: Action
    ring_queue* action_queue;
    Action* current_action;

    // our already processed actions
    stack* action_stack;

    //set this to true to have the items used pushed to the stack
    // bool push_to_stack = false; //NOTE: idk what this is for
} Action_Handler;


void action_handler_create(Action_Handler* action_handler);
void action_handler_destroy(Action_Handler* action_handler);


void action_handler_execute_next_action(Action_Handler* action_handler, Madness_Pulse_Game* game)
{
    if (ring_queue_is_empty(action_handler->action_queue)) return;

    ring_dequeue(action_handler->action_queue, action_handler->current_action);
    MASSERT(action_handler->current_action);

    //add to the stack only if we are not in the shipping build and we explicity want to push to the stack
    // if (!UUtilityFunctionLibrary::IsShippingBuild() && PushToStack)
    // {
    // ActionStack.Push(CurrentAction);
    // }

    stack_push(action_handler->action_stack, action_handler->current_action);


    action_handler->current_action->execute(game);

    // OnActionHandlerUpdate.Broadcast(ActionQueue, ActionStack);
}

void action_handler_empty_queue(Action_Handler* action_handler)
{
    ring_queue_empty(action_handler->action_queue);
}


void action_handler_skip_action(Action_Handler* action_handler, Madness_Pulse_Game* game)
{
    if (!action_handler->current_action) return;
    action_handler->current_action->skip(game);
}


void action_handler_empty_stack_memory(Action_Handler* action_handler)
{
    stack_empty(action_handler->action_stack);
    // OnActionHandlerUpdate.Broadcast(ActionQueue, ActionStack);
}

void action_handler_debug_print(Action_Handler* action_handler)
{
    for (int i = 0; i < action_handler->action_stack->num_items; i++)
    {
        Action* action = &action_handler->action_stack->data[i];
        DEBUG("index: %d, Action Name:", i);
        string_print(&action->name);
    }

    //TODO: print the queue

}

void action_handler_add_action(Action_Handler* action_handler, Action* actions)
{
    MASSERT(action_handler)
    MASSERT(actions)

    ring_enqueue(action_handler->action_queue, actions);
    // OnActionHandlerUpdate.Broadcast(ActionQueue, ActionStack);
}


#endif //ACTION_H
