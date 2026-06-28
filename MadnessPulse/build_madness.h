#pragma once


#include "game_constants.h"
#include "game_enums.h"
#include "game_structs.h"
#include "game_luts.h"

#include "game_reflection_enums_generated.h"
#include "game_reflection_struct_generated.h"


//UNIT Related Stuff
#include "actions.h"
#include "augments.h"
// #include "BattleTurnInformation.h"
#include "charge_list.h"
#include "conjure_death_list.h"
#include "health.h"
#include "inventory.h"
#include "mp.h"
#include "overflow.h"
#include "resistant_stat.h"
#include "reversal_list.h"
#include "special_ability_flags.h"
#include "status_stat_component.h"
#include "unit_base.h"

//ability stuff
#include "ability.h"
#include "ability_registry.h"
#include "ability_creation.h"

//ability components
#include "heal.h"
#include "heal_percent.h"
#include "heal_setter.h"
#include "heal_to_full.h"

#include "damage.h"


#include "drain.h"
#include "drain_percent.h"
#include "drain_all.h"

#include "mp_adder.h"
#include "mp_remove.h"
#include "mp_full.h"
#include "mp_zero.h"

#include "ability_adder.h"
#include "ability_remove_all.h"
#include "ability_remove.h"

#include "action_add.h"
#include "action_remove.h"
#include "action_trade.h"

#include "conjure.h"
#include "summoner.h"
#include "charge.h"




//turn based game mode stuff
#include "ability_handler.h"
#include "command/command.h"
#include "targeting_handler.h"


#include "madness_pulse_game.h"
#include "turn_based_game.h"


//save stuff
#include "game_levels.h"
#include "save_game.h"






