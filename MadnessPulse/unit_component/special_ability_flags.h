#ifndef SPECIAL_ABILITY_FLAG_LIST_H
#define SPECIAL_ABILITY_FLAG_LIST_H

#include <stdbool.h>
#include "game_structs.h"

Special_Ability_Flag_List_Component special_ability_flag_list_component_create()
{
    Special_Ability_Flag_List_Component special_ability_flag_list = {0};
    return special_ability_flag_list;
}

void SetFlags(Special_Ability_Flag_List_Component* units_list, Special_Ability_Flag_List_Component flags_to_set)
{
    if (flags_to_set.BlackSun)
    {
        units_list->BlackSun = true;
    }
    if (flags_to_set.AbyssalSun)
    {
        units_list->AbyssalSun = true;
    }
    if (flags_to_set.EmptySun)
    {
        units_list->EmptySun = true;
    }
    if (flags_to_set.Mirage)
    {
        units_list->Mirage = true;
        units_list->MirageCount = flags_to_set.MirageCount;
    }

    if (flags_to_set.RampartTaunt)
    {
        units_list->RampartTaunt = true;
    }
    if (flags_to_set.InfiniteMPFlag)
    {
        units_list->InfiniteMPFlag = true;
    }
    if (flags_to_set.PermanentInfiniteMPFlag)
    {
        units_list->PermanentInfiniteMPFlag = true;
    }
    if (flags_to_set.DesperateMagic)
    {
        units_list->DesperateMagic = true;
    }
    if (flags_to_set.DanceInTheDark)
    {
        units_list->DanceInTheDark = true;
    }
    if (flags_to_set.NonExistence)
    {
        units_list->NonExistence = flags_to_set.NonExistence;
    }
    if (flags_to_set.TrueExistence)
    {
        units_list->TrueExistence = flags_to_set.TrueExistence;
    }

}

void ResetFlags(Special_Ability_Flag_List_Component* special_ability_flag_list)
{
    memset(special_ability_flag_list, 0, sizeof(Special_Ability_Flag_List_Component));
}


bool CanUseMirage(Special_Ability_Flag_List_Component* special_ability_flag_list)
{
    //check if we can use and that the count is above zero
    // we also want to decrement the count
    if (special_ability_flag_list->Mirage && special_ability_flag_list->MirageCount > 0)
    {
        special_ability_flag_list->MirageCount--;
        return true;
    }

	
    special_ability_flag_list->Mirage = false;
    special_ability_flag_list->MirageCount = 0;
    return false;
}


#endif
