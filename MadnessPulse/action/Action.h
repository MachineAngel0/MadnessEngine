#ifndef ACTION_H
#define ACTION_H

#include "game_enums.h"
#include "game_structs.h"
#include "logger.h"
#include "ring_queue.h"
#include "str.h"

//ACTION INTERFACE:
typedef void (*action_execute)(Madness_Pulse_Game*,void*);
typedef void (*action_skip)(Madness_Pulse_Game*,void*);

typedef struct Action
{
    action_execute execute;
    action_skip skip;
    String name;

    //TODO: figure this out when you have an animation system in place
    // FQueueTimer QueueTimer;

    // void ExecuteNextAction();
    // void StartQueueTimer();

    // void ExecuteNextActionWithActionOverride(ATurnBasedGameMode* GameMode);
    // void StartQueueTimerWithOverride(ATurnBasedGameMode* GameMode);
    // void BeforeExecuteNextActionOverride(ATurnBasedGameMode* GameMode);
}Action;


struct UActionHandler
{
    /*FUNCTIONS AND VARIABLES*/

    //TYPE: Action
    ring_queue*  ActionQueue;
    Action* CurrentAction;

    // our already processed actions
    Action* ActionStack;
    u32 action_stack_size;

    //set this to true to have the items used pushed to the stack
    bool PushToStack = false;



    void ExecuteNextAction();

    void EmptyQueue();



    int ActionIndex = 0;

    void SkipAction();


    void EmptyStackMemory();

    void DebugPrintActionStack();

    void AddAction(Action* Actions);
    void AppendActions(Action* Actions);

};


#endif //ACTION_H
