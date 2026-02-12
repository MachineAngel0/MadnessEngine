#ifndef SPECIAL_ABILITY_FLAG_LIST_H
#define SPECIAL_ABILITY_FLAG_LIST_H


#include <stdbool.h>


struct Special_Ability_Flag_List
{

	bool FusionMania = false;
	
	//increases damage based on damage taken by 10%
	bool BlackSun = false;
	//take 100% more damage
	bool EmptySun = false;
	//reduce damage by 50%
	bool AbyssalSun = false;

	//increases Mp usage by 100%
	bool RampartTaunt = false;
	
	//allows for abilities to be used without mp 
	bool InfiniteMPFlag = false;
	bool PermanentInfiniteMPFlag = false;

	bool DesperateMagic = false;
	
	//will negate an ability used on the caster, up to the mirage count 
	bool Mirage = false;
	int MirageCount = 0;


	//25% chance any used move will fail, does not get reset, it's permanent
	bool DanceInTheDark = false;

	//stops any damage but leaves buildup 
	bool NonExistence = false;
	//stops any build up but leaves damage 
	bool TrueExistence= false;
};



#endif

