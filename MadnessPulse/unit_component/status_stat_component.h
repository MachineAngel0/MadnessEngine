#ifndef STATUS_STAT_COMPONENT_H
#define STATUS_STAT_COMPONENT_H

struct status_stat_component{

	TMap<EDamageType, float> StatusInfo
	{
		{EDamageType::ECS_Physical, 0},
		{EDamageType::ECS_Fire, 0},
		{EDamageType::ECS_Ice, 0},
		{EDamageType::ECS_Poison, 0},
		{EDamageType::ECS_Blood, 0},
		{EDamageType::ECS_Heavenly, 0},
		{EDamageType::ECS_Abyss, 0},
		{EDamageType::ECS_Madness, 0},
		{EDamageType::ECS_Insanity, 0},
	};

	TMap<EDamageType, float> StatusThreshold
	{
		{EDamageType::ECS_Physical, 100},
		{EDamageType::ECS_Fire, 100},
		{EDamageType::ECS_Ice, 100},
		{EDamageType::ECS_Poison, 100},
		{EDamageType::ECS_Blood, 100},
		{EDamageType::ECS_Heavenly, 100},
		{EDamageType::ECS_Abyss, 100},
		{EDamageType::ECS_Madness, 100},
		{EDamageType::ECS_Insanity, 100},
	};



	float LowerClampValue = 0.0f;
	float UpperClampValue = 1000.0f; // this is just an arbitrary value

	//status trigger
	int StatusTriggerOccurrence = 0;

	float LowerThresholdClampValue = 1.0f; // I dont know how low I actually want to let this go, 50 lowest maybe?
	float UpperThresholdClampValue = 10000.0f;  
};


#endif
