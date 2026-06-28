#ifndef ACTION_COMPONENT_H
#define ACTION_COMPONENT_H


#include <stdbool.h>
#include "logger.h"
#include "game_enums.h"
#include "game_structs.h"

#define MAX_ACTIONS_DEFAULT 3


void action_component_init_default(Action_Component* action)
{
    action->max_actions_available = MAX_ACTIONS_DEFAULT;
    //starts off at 0 but the will be increased on units turn start
    action->actions_available = 0;
}


void action_component_RefreshActionsAvailable(Action_Component* action)
{
    action->actions_available = action->max_actions_available;
}

void action_component_refresh_actions_by_addition(Action_Component* action)
{
    action->actions_available += action->max_actions_available;
}

void action_component_refresh_for_turn_start(Action_Component* action)
{
    action_component_refresh_actions_by_addition(action);
}

void action_component_decrease_actions(Action_Component* action, const int ActionsToDecrease)
{
    action->actions_available -= ActionsToDecrease;
}

void action_component_increase_actions(Action_Component* action, const int ActionsToIncrease)
{
    action->actions_available += ActionsToIncrease;
}

int action_component_return_actions_available(Action_Component* action)
{
    return action->actions_available;
}

bool action_component_is_action_available(Action_Component* action)
{
    return action->actions_available > 0;
}

void action_component_change_actions_by_type(Action_Component* action, Action_Changer_Type ActionType,
                                             int ActionsChangeAmount)
{
    switch (ActionType)
    {
    case Action_Changer_Type_ActionAdd:
        action_component_increase_actions(action, ActionsChangeAmount);
        break;
    case Action_Changer_Type_ActionRemove:
        action_component_decrease_actions(action, ActionsChangeAmount);
        break;
    default:
        FATAL("Change Actions By Type Function Given Invalid EActionChangerType");
        break;
    }
}

void action_component_change_max_actions_available_by_type(Action_Component* action,
                                                           const Action_Changer_Type action_type,
                                                           const int actions_change_amount)
{
    switch (action_type)
    {
    case Action_Changer_Type_ActionAdd:
        action->max_actions_available += actions_change_amount;
        break;
    case Action_Changer_Type_ActionRemove:
        action->max_actions_available -= actions_change_amount;
        break;
    default:
        FATAL("Change Actions By Type Function Given Invalid EActionChangerType");
        break;
    }
}


#endif
