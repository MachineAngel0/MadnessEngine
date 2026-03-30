#ifndef CHARGE_LIST_COMPONENT_H
#define CHARGE_LIST_COMPONENT_H



typedef enum Charge_State
{
    Charge_State_None,
    Charge_State_Charge,
    Charge_State_HighCharge,
} Charge_State;


typedef struct charge_list_component
{
    /*
    TMap<EDamageType, EChargeState> ChargeList
{
            {EDamageType::ECS_Physical, EChargeState::ECS_None},
            {EDamageType::ECS_Fire, EChargeState::ECS_None},
            {EDamageType::ECS_Ice, EChargeState::ECS_None},
            {EDamageType::ECS_Poison,EChargeState::ECS_None},
            {EDamageType::ECS_Blood, EChargeState::ECS_None},
            {EDamageType::ECS_Heavenly, EChargeState::ECS_None},
            {EDamageType::ECS_Abyss, EChargeState::ECS_None},
            {EDamageType::ECS_Madness, EChargeState::ECS_None},
            {EDamageType::ECS_Insanity, EChargeState::ECS_None},
    };
    */

    // TSet<EDamageType> ChargeTypesMarkedForReset;


    float ChargeDamagePercent = 2.0f; //100 percent
    float HighChargeDamagePercent = 4.0f; //300 percent


    int ChargePassiveAmount = 5.0f; // flat bonuses
    int HighChargePassiveAmount = 10.0f; //flat bonuses
} charge_list_component;



#endif
