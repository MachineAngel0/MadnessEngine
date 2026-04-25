#ifndef REVERSAL_LIST_H
#define REVERSAL_LIST_H
#include "game_structs.h"
#include "logger.h"


Reversal_List_Component Reversal_List_Component_create()
{
    Reversal_List_Component reversal_list_component = {0};
    return reversal_list_component;
}

void reversal_list_component_create_default(Reversal_List_Component* reversal_list_component)
{
    memset(reversal_list_component, 0, sizeof(Reversal_List_Component));
}

void ClearReversalList(Reversal_List_Component* reversal_list)
{
    memset(reversal_list, 0, sizeof(Reversal_List_Component));
}

void AddToDamageReversal(Reversal_List_Component* reversal_list, Damage_Type DamageTypeToAdd, u32 unit_id)
{
    // DEBUG("DamageType: %s and reversal_list->unit_who_hit_us: %s"), *UEnum::GetValueAsString(DamageTypeToAdd), *unit_id->GetName());
    reversal_list->unit_who_hit_us = unit_id;

    reversal_list->DamageReversal[DamageTypeToAdd] = true;
}

void AddToHealReversal(Reversal_List_Component* reversal_list, Heal_Types HealTypeToAdd, u32 unit_id)
{
    // DEBUG("DamageType: %s and reversal_list->unit_who_hit_us: %s"), *UEnum::GetValueAsString(HealTypeToAdd), *unit_id->GetName());
    reversal_list->unit_who_hit_us = unit_id;

    reversal_list->HealReversal[HealTypeToAdd] = true;
}

void AddToDrainReversal(Reversal_List_Component* reversal_list, Drain_Types DrainTypeToAdd, u32 unit_id)
{
    // DEBUG("DamageType: %s and reversal_list->unit_who_hit_us: %s"), *UEnum::GetValueAsString(DrainTypeToAdd), *unit_id->GetName());
    reversal_list->unit_who_hit_us = unit_id;

    reversal_list->DrainReversal[DrainTypeToAdd];
}

void AddToMPReversal(Reversal_List_Component* reversal_list, MP_Types MPTypeToAdd, u32 unit_id)
{
    // DEBUG("DamageType: %s and reversal_list->unit_who_hit_us: %s"), *UEnum::GetValueAsString(MPTypeToAdd), *unit_id->GetName());
    reversal_list->unit_who_hit_us = unit_id;

    reversal_list->MPReversal[MPTypeToAdd] = true;
}

void AddToAugmentReversal(Reversal_List_Component* reversal_list, Damage_Type AugmentTypeToAdd, u32 unit_id)
{
    // DEBUG("DamageType: %s and reversal_list->unit_who_hit_us: %s"), *UEnum::GetValueAsString(AugmentTypeToAdd), *unit_id->GetName());
    reversal_list->unit_who_hit_us = unit_id;

    reversal_list->AugmentReversal[AugmentTypeToAdd] = true;
}

void AddToNegationReversal(Reversal_List_Component* reversal_list, u32 unit_id)
{
    // DEBUG("Negation Changed, and reversal_list->unit_who_hit_us: %s"), *unit_id->GetName());
    reversal_list->unit_who_hit_us = unit_id;

    reversal_list->NegationPassiveReversal = true;
}

void AddToDamagePassiveReversal(Reversal_List_Component* reversal_list, u32 unit_id)
{
    // DEBUG("Damage Passive Changed, and reversal_list->unit_who_hit_us: %s"), *unit_id->GetName());
    reversal_list->unit_who_hit_us = unit_id;

    reversal_list->DamagePassiveReversal = true;
}

void AddToActionReversal(Reversal_List_Component* reversal_list, Ability_Changer_Type ActionTypeToAdd,
                         u32 unit_id)
{
    // DEBUG("DamageType: %s and reversal_list->unit_who_hit_us: %s"), *UEnum::GetValueAsString(ActionTypeToAdd), *unit_id->GetName());
    reversal_list->unit_who_hit_us = unit_id;

    reversal_list->ActionChangeReversal[ActionTypeToAdd] = true;
}

void AddToAbilityReversal(Reversal_List_Component* reversal_list, Ability_Changer_Type AbilityTypeToAdd,
                          u32 unit_id)
{
    // DEBUG("DamageType: %s and reversal_list->unit_who_hit_us: %s"), *UEnum::GetValueAsString(AbilityTypeToAdd), *unit_id->GetName());
    reversal_list->unit_who_hit_us = unit_id;

    reversal_list->AbilityChangeReversal[AbilityTypeToAdd] = true;
}


#endif
