#ifndef DAMAGE_H
#define DAMAGE_H

typedef enum Damage_Status_Type
{
    Damage_Status_Type_Custom,

    /*
    ECS_Illusion UMETA(DisplayName = "Illusion"),
    ECS_Asphixiation UMETA(DisplayName = "Fogotten"),
    ECS_Demise UMETA(DisplayName = "Fogotten"),
    ECS_armageddon UMETA(DisplayName = "Fogotten"),
    ECS_lunacy UMETA(DisplayName = "lunacy"),
    */

    Damage_Status_Type_Dreamy,
    Damage_Status_Type_Low,
    Damage_Status_Type_Delusion,
    Damage_Status_Type_High,
    Damage_Status_Type_Visionary,
    Damage_Status_Type_Imaginary,
    Damage_Status_Type_Physical,


    // Power scaling
    // Imaginary > Visionary > Heavy > Moderate > Competent > Weak > Brittle
    // Damage and Status
    Damage_Status_Type_MAX,
} DamageStatusType;

float DamageValueLookUpTable[Damage_Status_Type_MAX] =
{
    [Damage_Status_Type_Custom] = 0.f,
    [Damage_Status_Type_Dreamy] = 10.0f,
    [Damage_Status_Type_Low] = 25.0f,
    [Damage_Status_Type_Delusion] = 50.0f,
    [Damage_Status_Type_High] = 75.0f,
    [Damage_Status_Type_Visionary] = 100.0f,
    [Damage_Status_Type_Imaginary] = 200.0f,
    //literally just for physical abilities
    [Damage_Status_Type_Physical] = 800.0f,

};

float StatusValueLookUpTable[] =
{
    //0.5,1,2,3,4,5,10
    [Damage_Status_Type_Custom] = 0.f,
		        {EDamageStatusType::ECS_Dreamy, 0.5f},
                {EDamageStatusType::ECS_Low, 1.0f},
                {EDamageStatusType::ECS_Delusion, 2.0f},
                {EDamageStatusType::ECS_High, 4.0f},
                {EDamageStatusType::ECS_Visionary, 6.0f},
                {EDamageStatusType::ECS_Imaginary, 10.0f},
                // buff and debuff abilties bieng at high status build up just feels right
                // damage abilties bieng at low just feels right

                //literally just for physical abilities
                {EDamageStatusType::ECS_Physical, 0.0f},
            };

typedef struct Ability_Damage
{
	//Final Values Used in Calculations, use custom enum below, if I want to set these manually
    // TMap<DamageType, float> DamageTypeToDamage;
    // TMap<DamageType, float> StatusValues;

    //Enums used to make creating abilities go quicker, anything set to custom will need to be set manually in the above maps
    TMap<EDamageType, EDamageStatusType> DamageToType;
    TMap<EDamageType, EDamageStatusType> StatusToType{
		    {EDamageType::ECS_Physical, EDamageStatusType::ECS_Physical},
            {EDamageType::ECS_Fire, EDamageStatusType::ECS_Low},
            {EDamageType::ECS_Ice, EDamageStatusType::ECS_Low},
            {EDamageType::ECS_Poison, EDamageStatusType::ECS_Low},
            {EDamageType::ECS_Blood, EDamageStatusType::ECS_Low},
            {EDamageType::ECS_Heavenly, EDamageStatusType::ECS_Low},
            {EDamageType::ECS_Abyss, EDamageStatusType::ECS_Low},
            {EDamageType::ECS_Madness, EDamageStatusType::ECS_Low},
            {EDamageType::ECS_Insanity, EDamageStatusType::ECS_Low},
        };


    float SingleTargetStatusModifierValue = 3.0f;
    float MultiTargetStatusModifierValue = 0.40f;



} Ability_Damage;


#endif //DAMAGE_H
