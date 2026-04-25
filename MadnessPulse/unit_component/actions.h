#ifndef ACTION_COMPONENT_H
#define ACTION_COMPONENT_H


#include <stdbool.h>
#include "logger.h"
#include "game_enums.h"
#include "game_structs.h"

#define MAX_ACTIONS_DEFAULT 3



void action_component_init_default(Action_Component* action)
{
    action->MaxActionsAvailable = MAX_ACTIONS_DEFAULT;
    //starts off at 0 but the will be increased on units turn start
    action->ActionsAvailable = 0;
}




void RefreshActionsAvailable(Action_Component* action)
{
	action->ActionsAvailable = action->MaxActionsAvailable;
}

void RefreshActionsByAddition(Action_Component* action)
{
    action->ActionsAvailable += action->MaxActionsAvailable;
}

void RefreshForTurnStart(Action_Component* action)
{
	RefreshActionsByAddition(action);
}

void DecreaseActions(Action_Component* action, const int ActionsToDecrease)
{
    action->ActionsAvailable -= ActionsToDecrease;

}

void IncreaseActions(Action_Component* action, const int ActionsToIncrease)
{
	action->ActionsAvailable += ActionsToIncrease;
}

int ReturnActionsAvailable(Action_Component* action)
{
	return action->ActionsAvailable;
}

bool IsActionAvailable(Action_Component* action)
{
	return action->ActionsAvailable > 0;
}

void ChangeActionsByType(Action_Component* action, Action_Changer_Type ActionType, int ActionsChangeAmount)
{
    switch (ActionType)
    {
    case Action_Changer_Type_ActionAdd:
        IncreaseActions(action, ActionsChangeAmount);
        break;
    case Action_Changer_Type_ActionRemove:
        DecreaseActions(action, ActionsChangeAmount);
        break;
    default:
        FATAL("Change Actions By Type Function Given Invalid EActionChangerType");
        break;
    }
}

void ChangeMaxActionsAvailableByType(Action_Component* action, const Action_Changer_Type action_type, const int actions_change_amount)
{
    switch (action_type)
    {
    case Action_Changer_Type_ActionAdd:
        action->MaxActionsAvailable += actions_change_amount;
        break;
    case Action_Changer_Type_ActionRemove:
        action->MaxActionsAvailable -= actions_change_amount;
        break;
    default:
        FATAL("Change Actions By Type Function Given Invalid EActionChangerType");
        break;
    }
}


#endif
