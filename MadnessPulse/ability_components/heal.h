#ifndef HEAL_H
#define HEAL_H
#include "ability.h"
#include "game_structs.h"





typedef struct Heal_Component
{
    // TMap<EHealTypes, float> HealTypes;
    Heal_Types HealType;
    float heal_amount;

    //should be false be default
    bool HealOnlyIfDead;
} Heal_Ability;

Heal_Ability heal_ability_create(Heal_Types heal_types, float heal_amount, bool HealOnlyIfDead)
{
    return (Heal_Ability){ .HealType = heal_types, .heal_amount = heal_amount, .HealOnlyIfDead = HealOnlyIfDead};
}

void heal_ability(Unit* unit, void* data)
{
    Heal_Ability* heal_component = (Heal_Ability*)data;
    unit->health_component.current_health += heal_component->heal_amount;
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
