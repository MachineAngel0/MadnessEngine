#ifndef GAME_LUTS_H
#define GAME_LUTS_H

#include "game_enums.h"

u32 overflow_value_type_lut[Ability_Overflow_Value_Type_Max] = {
    [Ability_Overflow_Value_Type_Base] = 100,
    [Ability_Overflow_Value_Type_Created] = 20,
    [Ability_Overflow_Value_Type_Other1] = 50,
    [Ability_Overflow_Value_Type_Other2] = 10,
};

u8 ability_action_value_type_lut[Ability_Action_Cost_Type_Max] = {
    [Ability_Action_Cost_Type_1] = 1,
    [Ability_Action_Cost_Type_2] = 2,
    [Ability_Action_Cost_Type_3] = 3,
};


Character_Name summoner_to_character_name[Summoner_Type_Max] = {
    [Summoner_Type_Doll] = Character_Name_Doll,
    [Summoner_Type_RedJester] = Character_Name_Red_Jester,
    [Summoner_Type_Clown] = Character_Name_Clown,
    [Summoner_Type_Puppet] = Character_Name_Puppet,
};


#endif
