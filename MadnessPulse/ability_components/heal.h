#ifndef HEAL_H
#define HEAL_H
#include "game_structs.h"


typedef enum Heal_Types
{
    Heal_Types_HealAmount,
    Heal_Types_HealSetter,
    Heal_Types_HealPercent,
    Heal_Types_HealToFull,
    Heal_Types_HealByMultiplication,
}Heal_Types;


struct Heal_Component
{
    TMap<EHealTypes, float> HealTypes;

    bool HealOnlyIfDead = false;

};



void heal(Unit* UnitCaster, Unit** Targets, Game_State& GameState)
{

    for (AUnitBase* Target : Targets)
    {
        Health_Component* CurrentTargetHealthComponent = Target->HealthComponent;

        if(HealOnlyIfDead)
        {
            if (CurrentTargetHealthComponent->IsAlive())
            {
                return;
            };
        }

        for (const auto& HealInformation : HealTypes)
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
                NewHealFinal.Emplace(UHealPlayback::CreateHealPlayBack(Target, CurrentTargetHealthComponent->HealToFullValueChange()));
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


#endif //HEAL_H
