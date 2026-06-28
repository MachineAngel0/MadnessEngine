#ifndef HEAL_H
#define HEAL_H


#include "game_structs.h"

/*
void heal_ability(Unit* UnitCaster, Unit** Targets, Game_State& GameState)
{
    for (AUnitBase* Target : Targets)
    {
        Health_Component* CurrentTargetHealthComponent = Target->HealthComponent;

        if (heal_only_if_dead)
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


void heal_component_create(Ability_Component* ac, const float heal_amount,
                           const bool heal_only_if_dead)
{
    ac->type = Ability_Component_TYPE_HEAL;


    ac->data.heal = (Heal_Component){
        .heal_amount = heal_amount,
        .heal_only_if_dead = heal_only_if_dead
    };
}

void heal_ability(Unit* unit, const Heal_Component* component)
{
    heal_by_amount(&unit->health_component, component->heal_amount);
}

void heal_component_text(const Heal_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "Heal target by %f",
                                               component->heal_amount));
}






#endif //HEAL_H
