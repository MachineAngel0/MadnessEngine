#ifndef SUMMONER_H
#define SUMMONER_H

#include "turn_based_game.h"


void summoner_component_create(Ability_Component* ac, Summoner_Type type)
{
    ac->type = Ability_Component_TYPE_SUMMONER;
    ac->data.summoner =  (Summoner_Component){.summoner_type = type};
}

void summoner_component_ability(Madness_Pulse_Game* game, Unit_Handle caster, Summoner_Component* summoner_component)
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

void summoner_component_text(const Summoner_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator,
                                               "Conjure the demon of %s. Caster's abilities will be replaced by the conjured demons abilities.",
                                               Summoner_Type_enum_string[component->summoner_type]));
}



#endif
