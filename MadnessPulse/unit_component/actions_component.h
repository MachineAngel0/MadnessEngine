#ifndef ACTION_COMPONENT_H
#define ACTION_COMPONENT_H


#define MAX_ACTIONS_DEFAULT 3

typedef struct Action_Component
{
    int MaxActionsAvailable;
    int ActionsAvailable;
}Action_Component;

void action_component_init(Action_Component action)
{

    action.MaxActionsAvailable = MAX_ACTIONS_DEFAULT;
    //starts off at 0 but the will be increased on units turn start
    action.ActionsAvailable = 0;
}





#endif