#ifndef ACTION_H
#define ACTION_H


typedef struct action_component
{
    int MaxActionsAvailable;
    int ActionsAvailable;

}action_component;

void action_component_init_default(action_component action)
{

    action.MaxActionsAvailable = 3;
    //starts off at 0 but the will be increase on units turn start
    action.ActionsAvailable = 0;
}





#endif