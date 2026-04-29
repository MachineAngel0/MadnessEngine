#ifndef ICON_TEXTURE_LIST_H
#define ICON_TEXTURE_LIST_H


#include "audio.h"
#include "game_enums.h"
#include "resource_types.h"

typedef struct Icon_Texture_List
{
    Texture_Handle* Physical;
    Texture_Handle* Fire;
    Texture_Handle* Ice;
    Texture_Handle* Poison;
    Texture_Handle* Blood;
    Texture_Handle* HolyEssense;
    Texture_Handle* Abyssal;
    Texture_Handle* Madness;
    Texture_Handle* Insanity;

    Texture_Handle* MultiElemental;

    Texture_Handle* Abnormal;

    Texture_Handle* Conjure;
    Texture_Handle* Reversal;
    Texture_Handle* Special;


    Texture_Handle* SuperWeak;
    Texture_Handle* Weak;
    Texture_Handle* Neutral;
    Texture_Handle* Strong;
    Texture_Handle* Resistant;
    Texture_Handle* Redirect;
    Texture_Handle* Spread;
} Icon_Texture_List;

//TODO: fix after i have a better idea of what the main game loop will be and how abilites will be displayed
Texture_Handle* ReturnAbilityIcon(Ability_Type ability_type)
{
    switch (ability_type)
    {
    case Ability_Type_Physical:
        return Physical;
    case EAbilityType::ECS_Fire:
        return Fire;
    case EAbilityType::ECS_Ice:
        return Ice;
    case EAbilityType::ECS_Poison:
        return Poison;
    case EAbilityType::ECS_Blood:
        return Blood;
    case EAbilityType::ECS_Heavenly:
        return HolyEssense;
    case EAbilityType::ECS_Abyss:
        return Abyssal;
    case EAbilityType::ECS_Madness:
        return Madness;
    case EAbilityType::ECS_Insanity:
        return Insanity;
    case EAbilityType::ECS_MultiElemental:
        return MultiElemental;
    case EAbilityType::ECS_Conjure:
        return Conjure;
    case EAbilityType::ECS_Reversal:
        return Reversal;
    case EAbilityType::ECS_SpecialFlag:
        return Special;
    case EAbilityType::ECS_Abnormal:
        return Abnormal;
    }


    return nullptr;
}

Texture_Handle* ReturnResistanceIcon(Resistance_Type resistance_type)
{
    switch (resistance_type)
    {
    case EResistanceType::ECS_SuperWeak:
        return SuperWeak;
    case EResistanceType::ECS_Weak:
        return Weak;
    case EResistanceType::ECS_Neutral:
        return Neutral;
    case EResistanceType::ECS_Strong:
        return Strong;
    case EResistanceType::ECS_Resistant:
        return Resistant;
    case EResistanceType::ECS_Redirect:
        return Redirect;
    case EResistanceType::ECS_Spread:
        return Spread;
    }

    return nullptr;
}


enum Audio_Lookup_Type
{
    ECS_Test,
    ECS_Debug,
    ECS_Failed,
    ECS_MultiElementalDamageAudio,
    ECS_Augment,
    ECS_Heal,
    ECS_Drain,
    ECS_MPChange,
    ECS_AbilityChange,
    ECS_ActionChange,
    ECS_Status,
    ECS_Reversal,
    ECS_ConjureChange,
    ECS_ConjureDeath,
    ECS_Passive,
    ECS_TurnAudio,
    ECS_SpecialFlagAudio,
    ECS_Redirect,
};


struct Game_Audio_List
{
    /*TQueue<Audio_Handle> AudioQueue;
    TMap<Audio_Handle, int> AudioCount;

    void AddToQueue(Audio_Handle Sound);
    void ProcessQueue();
    //main one to use
    void PlayAudioType(EAudioSFXType Audio);
    void PlayDamageAudio(EDamageType DamageType);*/


    Audio_Handle FailedAudio;
    Audio_Handle TestAudio;

    Audio_Handle DebugCrashGameErrorSound;
    Audio_Handle DamageAudioMap[Damage_Type_MAX];
    /*{
            {EDamageType::ECS_Physical, nullptr},
            {EDamageType::ECS_Fire, nullptr},
            {EDamageType::ECS_Ice, nullptr},
            {EDamageType::ECS_Poison, nullptr},
            {EDamageType::ECS_Heavenly, nullptr},
            {EDamageType::ECS_Blood, nullptr},
            {EDamageType::ECS_Abyss, nullptr},
            {EDamageType::ECS_Madness, nullptr},
            {EDamageType::ECS_Insanity, nullptr},
            {EDamageType::ECS_Insanity, nullptr},
        };*/
    Audio_Handle MultiElementalDamageAudio;
    Audio_Handle AugmentAudio;
    Audio_Handle HealAudio;
    Audio_Handle DrainAudio;
    Audio_Handle MPChangeAudio;
    Audio_Handle AbilityChangeAudio;
    Audio_Handle ActionChangeAudio;
    Audio_Handle ReversalAudio;
    Audio_Handle ConjureChangeAudio;
    Audio_Handle PassiveAudio;
    Audio_Handle TurnAudio;
    Audio_Handle StatusChangerAudio;
    Audio_Handle ConjureDeathAudio;
    Audio_Handle SpecialFlagAudio;
    Audio_Handle RedirectAudio;
};


struct VFX_LIST
{
    TMap<EDamageType, UNiagaraSystem*> DamageToAugmentType
    {
        {EDamageType::ECS_Physical, nullptr},
        {EDamageType::ECS_Fire, nullptr},
        {EDamageType::ECS_Ice, nullptr},
        {EDamageType::ECS_Poison, nullptr},
        {EDamageType::ECS_Heavenly, nullptr},
        {EDamageType::ECS_Blood, nullptr},
        {EDamageType::ECS_Abyss, nullptr},
        {EDamageType::ECS_Madness, nullptr},
        {EDamageType::ECS_Insanity, nullptr},
        {EDamageType::ECS_Insanity, nullptr},
    };
    TObjectPtr<UNiagaraSystem> MultiElementalAugment;


    TMap<EDamageType, UNiagaraSystem*> DamageVFXMap
    {
        {EDamageType::ECS_Physical, nullptr},
        {EDamageType::ECS_Fire, nullptr},
        {EDamageType::ECS_Ice, nullptr},
        {EDamageType::ECS_Poison, nullptr},
        {EDamageType::ECS_Heavenly, nullptr},
        {EDamageType::ECS_Blood, nullptr},
        {EDamageType::ECS_Abyss, nullptr},
        {EDamageType::ECS_Madness, nullptr},
        {EDamageType::ECS_Insanity, nullptr},
        {EDamageType::ECS_Insanity, nullptr},
    };
    TObjectPtr<UNiagaraSystem> MultiElementalDamage;


    TObjectPtr<UNiagaraSystem> HealVFX;

    TObjectPtr<UNiagaraSystem> DrainVFX;

    TObjectPtr<UNiagaraSystem> MPChangeVFX;

    TObjectPtr<UNiagaraSystem> AbilityChangeVFX;

    TObjectPtr<UNiagaraSystem> ActionChangeVFX;

    TObjectPtr<UNiagaraSystem> ReversalVFX;

    TObjectPtr<UNiagaraSystem> ConjureChangeVFX;

    TObjectPtr<UNiagaraSystem> PassiveVFX;

    TObjectPtr<UNiagaraSystem> TurnVFX;

    TObjectPtr<UNiagaraSystem> StatusChangerVFX;

    TObjectPtr<UNiagaraSystem> ChargeVFX;

    TObjectPtr<UNiagaraSystem> SpecialAbilityFlagVFX;

    TObjectPtr<UNiagaraSystem> ShieldVFX;
};


#endif //ICON_TEXTURE_LIST_H
