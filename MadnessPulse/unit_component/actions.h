#ifndef ACTION_COMPONENT_H
#define ACTION_COMPONENT_H


#include <stdbool.h>
#include "logger.h"
#include "game_enums.h"
#include "game_structs.h"



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


void action_component_decrease_actions_by_type(Action_Component* action, const Ability_Action_Cost_Type cost_type)
{
    MASSERT(cost_type != Ability_Action_Cost_Type_Max)

    action->actions_available -= ability_action_value_type_lut[cost_type];
}


void action_component_decrease_actions(Action_Component* action, const u32 ActionsToDecrease)
{
    action->actions_available -= ActionsToDecrease;
}

void action_component_increase_actions(Action_Component* action, const u32 ActionsToIncrease)
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




#endif
