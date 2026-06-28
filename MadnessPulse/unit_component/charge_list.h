#ifndef CHARGE_LIST_H
#define CHARGE_LIST_H

#include "game_enums.h"
#include "game_structs.h"





Charge_List_Component Charge_List_Component_Create()
{
    Charge_List_Component Charge_List_Component = {0};

    Charge_List_Component.charge = false;
    Charge_List_Component.high_charge = false;

    return Charge_List_Component;
}

void charge_list_component_create_default(Charge_List_Component* charge_list_component)
{
    memset(charge_list_component, 0, sizeof(Charge_List_Component));
    // Charge_State ChargeList[Damage_Type_MAX];
    // bool ChargeTypesMarkedForReset[Damage_Type_MAX];


    charge_list_component->charge = false;
    charge_list_component->high_charge = false;
}


void charge_list_component_add_charge_states(Charge_List_Component* charge_list_component, Charge_State state)
{
    switch (state)
    {
    case Charge_State_Charge:
        charge_list_component->charge = true;
        break;
    case Charge_State_HighCharge:
        charge_list_component->high_charge = true;
        break;
    case Charge_State_MAX:
        MASSERT(false);
        break;
    }
}


float charge_list_return_multiplier(Charge_List_Component* charge_list_component)
{
    // I want status Triggers to have charge applied to them too
    //we return charge type and then in the action manager we call clear charge list, if any charge was used

    float out_multiplier = 1.f;

    if (charge_list_component->high_charge)
    {
        out_multiplier *= High_Charge_Percent;
    }
    if (charge_list_component->charge)
    {
        out_multiplier *= Charge_Percent;

    }


    return out_multiplier;
}



void charge_list_clear(Charge_List_Component* charge_list_component)
{
    charge_list_component->charge = false;
    charge_list_component->high_charge = false;

}



#endif
