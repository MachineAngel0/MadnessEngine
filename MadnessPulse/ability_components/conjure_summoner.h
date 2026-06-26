#ifndef CONJURE_SUMMONER_H
#define CONJURE_SUMMONER_H
#include "turn_based_game.h"


Conjure_Component conjure_component_create(Conjure_Type type)
{
    return (Conjure_Component){.conjure_type = type};
}

Summoner_Component summoner_component_create(Summoner_Type type)
{
    return (Summoner_Component){.summoner_type = type};
}

void conjure_component_process(Madness_Pulse_Game* game, Unit_Handle caster)
{
    // clear inventory, add the abilties, and then add the conjure type into the death list componentd
    /*
    unit_caster->InventoryManagerComponent->ClearBattleInventory();
    UnitCaster->InventoryManagerComponent->AddMultipleToBattleInventory(AbilitiesToAdd);

    UnitCaster->ConjureDeathListComponent->AddDeathCondition(ConjureType);

    ConjurePlayBack = UConjurePlayback::CreateConjurePlayBack(UnitCaster, ConjureType);
    */

}

void summoner_component_process(Madness_Pulse_Game* game, Unit_Handle caster, Summoner_Component* summoner_component)
{

    //do a check and see if the summon can even be summoned
    bool can_be_summoned = true;
    switch (madness_pulse_get_unit(game, caster)->character_type)
    {
    case Character_Type_Player:
        for (int i = 0; i < game->player_count; ++i)
        {
            if (madness_pulse_get_unit(game, game->players[i])->name == summoner_to_character_name[summoner_component->summoner_type])
            {
                can_be_summoned = false;
            }
        }
        break;
    case Character_Type_Enemy:
        for (int i = 0; i < game->enemy_count; ++i)
        {
            if (madness_pulse_get_unit(game, game->enemies[i])->name == summoner_to_character_name[summoner_component->summoner_type])
            {
                can_be_summoned = false;
            }
        }
        break;
    }


    if (!can_be_summoned) {return;}

    //TODO: do the summon
    DEBUG("SUMMON SUCCESSFUL")

}


#endif //CONJURE_SUMMONER_H
