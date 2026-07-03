#ifndef CONDITIONAL_H
#define CONDITIONAL_H

#include "turn_based_game.h"


bool conditional_component_fail_ability(Conditional_Component* conditional,
                                        Ability_Target_Execution_Info* ability_target_info)
{
    if (conditional->was_set == false) { return false; }

    //... fail checks here
    if (conditional->test_condition)
    {
        return false;
    }
    if (conditional->caster_mp_at_zero)
    {
        if (ability_target_info->caster->mp_component.current_mp > 1) return false;
    }
    if (conditional->caster_has_positive_damage)
    {
        if (ability_target_info->caster->augment_component.damage_points < 0) return false;
    }


    //if we reach this point nothing failed
    return true;
}


#endif
