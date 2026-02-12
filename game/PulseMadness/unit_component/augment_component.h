#ifndef AUGMENT_COMPONENT_H
#define AUGMENT_COMPONENT_H


typedef struct Augment_List
{
    /*
    TMap<EDamageType, int> StatusPoints
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
    */

    /*
    EDamageType*  StatusPoints_damage // you know what, the index is implied by the enum, but you would have to have all of them
    int*  StatusPoints_value;
    int size = edamage_type_max;
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
    */



    int DamagePoints = 0;
    int NegationPoints = 0;
    int DamageAndNegationUpperLimitPoints = 100;
    int DamageAndNegationLowerLimitPoints = -100;
    int StatusUpperLimitPoints = 100;
    int StatusLowerLimitPoints = 0;
} Augment_List;

typedef struct augment_states
{
    Augment_List* augment_states; // array
} augment_states;


#endif //AUGMENT_COMPONENT_H
