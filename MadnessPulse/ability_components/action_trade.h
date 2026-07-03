#ifndef ACTION_TRADE_H
#define ACTION_TRADE_H

#include "game_enums.h"
#include "game_structs.h"
#include "turn_based_game.h"


void action_trade_component_create(Ability_Component* ac)
{
    ac->type = Ability_Component_TYPE_ACTION_TRADE;

    ac->data.action_trade = (Action_Trade_Component){
        .lose_all = true,
    };
}

void action_trade_ability(Madness_Pulse_Game* game, Unit* caster, ARRAY_TYPE(Character_Name)* targets,
                          const Action_Trade_Component* component)
{
    const int amount_to_change = caster->action_component.actions_available;
    DEBUG("Amount To Change By %d", amount_to_change);
    action_component_decrease_actions(&caster->action_component, amount_to_change);
    DEBUG("Unit Caster After Change %d", caster->action_component.actions_available);

    for (u32 i = 0; i < targets->num_items; i++)
    {
        Character_Name current_target = array_get(targets, Character_Name, i);
        Unit* current_unit_target = madness_pulse_get_unit(game, current_target);

        DEBUG("Target Before Change %d", current_unit_target->action_component.actions_available);
        action_component_increase_actions(&current_unit_target->action_component, amount_to_change);
        DEBUG("Target After Change %d", current_unit_target->action_component.actions_available);
        // ActionChangerFinal.Emplace(UActionPlayback::CreateActionPlayBack(Target, amount_to_change));
    }

}

void action_trade_component_text(const Action_Trade_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 &STRING("Give all of the Casters Remaining Actions to the targets. Does not overflow"));
}

#endif
