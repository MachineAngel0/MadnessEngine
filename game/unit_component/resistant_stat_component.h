#ifndef RESISTANT_STAT_COMPONENT_H
#define RESISTANT_STAT_COMPONENT_H

typedef struct Resistant_Stat_Component
{

	// TODO: these are to be set by me for the enemies, idk what it will be for the player yet
	//TODO: make this a map, and initiale them all, then fix all the functions
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EDamageType, EResistanceType> Resistance{
	{EDamageType::ECS_Physical, EResistanceType::ECS_Neutral},
	{EDamageType::ECS_Fire, EResistanceType::ECS_Neutral},
	{EDamageType::ECS_Ice, EResistanceType::ECS_Neutral},
	{EDamageType::ECS_Poison, EResistanceType::ECS_Neutral},
	{EDamageType::ECS_Blood, EResistanceType::ECS_Neutral},
	{EDamageType::ECS_Heavenly, EResistanceType::ECS_Neutral},
	{EDamageType::ECS_Abyss, EResistanceType::ECS_Neutral},
	{EDamageType::ECS_Madness, EResistanceType::ECS_Neutral},
	{EDamageType::ECS_Insanity, EResistanceType::ECS_Neutral},
	};

	
	
}Resistant_Stat_Component;


#endif
