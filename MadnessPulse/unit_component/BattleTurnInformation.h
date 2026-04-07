#ifndef BATTLE_TURN_INFO_H
#define BATTLE_TURN_INFO_H
#include "damage.h"


//TODO: have all the structs update when needed
typedef struct FRewindLifeTime
{
    //Snapshot of the game state that gets updated everytime all units have gone
    int TurnCount = 0;

    //Information character had at the start of their turn, turn is indicated by the indexed number, make sure theirs a check

    //NOTE: these are all arrays
    //Health
    float* Health;
    //MP
    float* MP;
    // Augment
    TArray<TMap<EDamageType, int>> AugmentStage;
    // Damage Points
    int* DamageStage;
    // Defense Points
    int* NegationStage;
} FRewindLifeTime;

typedef struct FLifeTimeEachAction
{
    //any information about the unit that needs to be reset when their turn ends

    //todo: heal amount, and maybe drain/blood loss
    float HealAmount;
    float DrainAmount;

    //Damage Taken
    float DamageTaken[Damage_Status_Type_MAX];
    int DamageTakenElementCount[Damage_Status_Type_MAX];
} FLifeTimeEachAction;

typedef struct FLifeTimeTurnStart
{
    //any information about the unit that lasts from when their turn starts, until their next turn starts


    // damage dealt this turn
    // elements used this turn
    // any healing done this turn
    // any drain done this turn
    // specific abilties used this turn


    //todo: heal amount, and maybe drain/blood loss
    float HealAmountType[Heal_Types_MAX];
    float HealAmount;
    float DrainAmountType[Drain_Types_MAX];
    float DrainAmount;


    // Damage dealt, should get reset at the first turn start
    float DamageDealt[Damage_Type_MAX];

    int DamageDealtElementCount[Damage_Type_MAX];
} FLifeTimeTurnStart;

typedef struct FLifeTimeBattleInfo
{
    //information that lasts until the battle is over

    float TotalHealAmount[Heal_Types_MAX];

    // Total damage taken
    float TotalDamageTaken[Damage_Type_MAX];


    int TotalDamageTakenElementCount[Damage_Type_MAX];
    float TotalDamageDealt[Damage_Type_MAX];


    int TotalDamageDealtElementCount[Damage_Type_MAX];

    float TotalDrainAmount;

    //TODO: have this update
    //everytime an ability is used, increment this count
    // TMap<FName, int> AbilityNameUsedCount;
} FLifeTimeBattleInfo;


/*TODO: this whole thing needs to be updated properly which it is currently not*/
typedef struct UBattleTurnInformation
{
    /*UNTIL THE CHARACTERS TURN STARTS*/
    FRewindLifeTime RewindLifeTime;


    /*UNTIL THE CHARACTERS TURN STARTS*/
    FLifeTimeTurnStart LifeTimeTurnStart;


    /*UNTIL THE CHARACTERS TURN ENDS, RESETS FOR EVERY CHARACTER*/
    FLifeTimeEachAction LifeTimeEachAction;


    /*Permanent category*/
    FLifeTimeBattleInfo LifeTimeBattleInfo;
} UBattleTurnInformation;

void UpdateGameStateSnapShot(int TurnCount);
void UpdateFirstTurnStartInfo();
void UpdateTurnEndInfo();

void AddToDamageTaken(Damage_Type DamageType, float DamageValue);
void AddToDamageDealt(Damage_Type DamageType, float DamageValue);

float ReturnDamageTakenByType(Damage_Type DamageType);
float ReturnAllDamageTaken();
float ReturnTotalDamageTakenByType(Damage_Type DamageType);

float ReturnAllTotalDamageTaken();

float ReturnDamageDealtByType(Damage_Type DamageType);
float ReturnAllDamageDealtByType(Damage_Type DamageType);
float ReturnTotalDamageDealtByType(Damage_Type DamageType);
float ReturnAllTotalDamageDealt(Damage_Type DamageType);


//resets all info besides maybe the turn count/rewind info
//TODO: implement it
void ClearInfoForEverything();

//TODO: implement it
//checking to make sure were not using it on the first turn
bool IsRewindAvailableForUse() const;
int GetRewindTurnCount() const;

float EachAction_DamageTaken();


#endif
