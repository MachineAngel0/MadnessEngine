#ifndef HEAL_H
#define HEAL_H


#include "game_structs.h"


Heal_Component heal_component_create(const Heal_Types heal_types, const float heal_amount, bool heal_only_if_dead)
{
    return (Heal_Component){
        .type = Ability_Component_TYPE_HEAL,
        .heal_type = heal_types,
        .heal_amount = heal_amount,
        .heal_only_if_dead = heal_only_if_dead
    };
}

void heal_ability(Unit* unit, Heal_Component heal_component)
{
    unit->health_component.current_health += heal_component.heal_amount;
}

/*
void heal_ability(Unit* UnitCaster, Unit** Targets, Game_State& GameState)
{
    for (AUnitBase* Target : Targets)
    {
        Health_Component* CurrentTargetHealthComponent = Target->HealthComponent;

        if (HealOnlyIfDead)
        {
            if (CurrentTargetHealthComponent->IsAlive())
            {
                return;
            };
        }

        for (const auto&HealInformation
        :
        HealTypes
        )
        {
            float FinalValue = 0;
            float Bonus = 0;

            switch (HealInformation.Key)
            {
            case EHealTypes::ECS_HealAmount:
                Bonus = StatusBuildUpBonus.GetStatusBuildUpBonus(UnitCaster, Target, StatusBuildUpBonus);
                FinalValue = (HealInformation.Value + Bonus);
                //NewHealFinal.Emplace(UHealPlayback::CreateHealPlayBack(Target, FinalValue));
                NewHealFinal.Emplace(UHealPlayback::CreateHealPlayBack(Target, FinalValue));
                CurrentTargetHealthComponent->HealByAmount(FinalValue);
                break;
            case EHealTypes::ECS_HealSetter:
                FinalValue = HealInformation.Value;
                NewHealFinal.Emplace(UHealPlayback::CreateHealPlayBack(Target, FinalValue));
                CurrentTargetHealthComponent->SetHealth(FinalValue);
                break;
            case EHealTypes::ECS_HealPercent:
                NewHealFinal.Emplace(UHealPlayback::CreateHealPlayBack(
                    Target, CurrentTargetHealthComponent->HealthPercentValueChange(HealInformation.Value)));
                CurrentTargetHealthComponent->HealByPercent(HealInformation.Value);
                break;
            case EHealTypes::ECS_HealToFull:
                NewHealFinal.Emplace(
                    UHealPlayback::CreateHealPlayBack(Target, CurrentTargetHealthComponent->HealToFullValueChange()));
                CurrentTargetHealthComponent->HealToFullHealth();
                break;
            case EHealTypes::ECS_HealByMultiplication:
                FinalValue = CurrentTargetHealthComponent->HealthInfo.CurrentHealth * HealInformation.Value;
                NewHealFinal.Emplace(UHealPlayback::CreateHealPlayBack(Target, FinalValue));
                CurrentTargetHealthComponent->HealByMultiplication(HealInformation.Value);
                break;
            }
            Target->ReversalListComponent->AddToHealReversal(HealInformation.Key, UnitCaster);
        }
    }
}
*/

#endif //HEAL_H
