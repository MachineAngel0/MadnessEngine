#ifndef CHARGE_LIST_H
#define CHARGE_LIST_H

#include "game_enums.h"
#include "game_structs.h"



#define Charge_Percent 2.0f; //100 percent
#define High_Charge_Percent 4.0f //300 percent

//flat bonuses
#define Charge_Passive 5.0f;
#define High_Charge_Passive 10.0f



Charge_List_Component Charge_List_Component_Create()
{
    Charge_List_Component Charge_List_Component = {0};
    //percent based bonus
    Charge_List_Component.ChargeDamagePercent = Charge_Percent;
    Charge_List_Component.HighChargeDamagePercent = High_Charge_Percent;

    //flat bonuses for things like augments
    Charge_List_Component.ChargePassiveAmount = Charge_Passive;
    Charge_List_Component.HighChargePassiveAmount = High_Charge_Passive;
}


void AppendChargeStates(Charge_List_Component* charge_list_component, TMap<EDamageType, EChargeState> ElementToChargeType)
{
    for (const auto& NewChargeType : ElementToChargeType)
    {
        ChargeList[NewChargeType.Key] = NewChargeType.Value;
    }
}

float ReturnChargeDamage(Charge_List_Component* charge_list_component, EDamageType DamageType)
{
    // I want status Triggers to have charge applied to them too
    //we return charge type and then in the action manager we call clear charge list, if any charge was used

    switch (ChargeList[DamageType])
    {
    case EChargeState::ECS_None:
        return 1.0f;
    case EChargeState::ECS_Charge:
        ChargeTypesMarkedForReset.Emplace(DamageType);
        return ChargeDamagePercent;
    case EChargeState::ECS_HighCharge:
        ChargeTypesMarkedForReset.Emplace(DamageType);
        return HighChargeDamagePercent;
    };

    return 1.0f;
}

int ReturnChargePassive(Charge_List_Component* charge_list_component,const Damage_Type damage_type)
{
    // I want status Triggers to have charge applied to them too
    //we return charge type and then in the action manager we call clear charge list, if any charge was used

    switch (charge_list_component->ChargeList[damage_type])
    {
    case Charge_State_None:
        return 0;
    case Charge_State_Charge:
        charge_list_component->ChargeTypesMarkedForReset[damage_type] = true;
        return charge_list_component->ChargePassiveAmount;
    case Charge_State_HighCharge:
        charge_list_component->ChargeTypesMarkedForReset[damage_type] = true;
        return HighChargePassiveAmount;
    case Charge_State_MAX:
        break;
    };

    return 0;
}


void ClearActiveChargeList(Charge_List_Component* charge_list_component)
{
    for (const auto& TypeToReset: ChargeTypesMarkedForReset)
    {
        ChargeList[TypeToReset] = EChargeState::ECS_None;
    }
}

void BroadcastChargeStates(Charge_List_Component* charge_list_component)
{
    for (const auto& List : ChargeList)
    {
        OnCharge.Broadcast(List.Key, List.Value);
    }
}


#endif
