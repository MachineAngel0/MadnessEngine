#ifndef ACTION_COMPONENT_H
#define ACTION_COMPONENT_H


#define MAX_ACTIONS_DEFAULT 3

typedef struct action_component
{
    int MaxActionsAvailable;
    int ActionsAvailable;
}action_component;

void action_component_init(action_component action)
{

    action.MaxActionsAvailable = MAX_ACTIONS_DEFAULT;
    //starts off at 0 but the will be increased on units turn start
    action.ActionsAvailable = 0;
}





#endif