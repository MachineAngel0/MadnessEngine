#ifndef STATUS_STAT_COMPONENT_H
#define STATUS_STAT_COMPONENT_H
#include "game_enums.h"
#include "game_structs.h"
// Fill out your copyright notice in the Description page of Project Settings.



Status_Stat_Component Status_Stat_Component_create()
{
    Status_Stat_Component status_stat_component;

    status_stat_component.StatusInfo[Damage_Type_MAX] = {0};
    status_stat_component.StatusThreshold[Damage_Type_MAX]= {0};

    status_stat_component.LowerClampValue = 0.0f;
    status_stat_component.UpperClampValue = 1000.0f; // this is just an arbitrary value

    status_stat_component.StatusTriggerOccurrence = 0;

    status_stat_component.LowerThresholdClampValue = 1.0f;
    // I dont know how low I actually want to let this go, 50 lowest maybe?
    status_stat_component.UpperThresholdClampValue = 10000.0f;
}

//utility
void ClampAndTruncateStatusBuildup(Status_Stat_Component* status_stat_component, const Damage_Type StatusTypeToCheck)
{
    StatusInfo[StatusTypeToCheck] = FMath::Clamp(StatusInfo[StatusTypeToCheck], LowerClampValue, UpperClampValue);
    StatusInfo[StatusTypeToCheck] = FMath::TruncToInt32(StatusInfo[StatusTypeToCheck]);
}

void ClampStatusThreshold(Status_Stat_Component* status_stat_component, const Damage_Type StatusTypeToCheck)
{
    StatusThreshold[StatusTypeToCheck] = FMath::Clamp(StatusThreshold[StatusTypeToCheck], LowerThresholdClampValue,
                                                      UpperThresholdClampValue);
}

//build-ups
void ChangeStatusBuildUp(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                         const float StatusChangeValue)
{
    StatusInfo[StatusType] += StatusChangeValue;

    while (StatusInfo[StatusType] >= StatusThreshold[StatusType])
    {
        StatusInfo[StatusType] -= StatusThreshold[StatusType];
        StatusTriggerOccurrence++;
    }
    UE_LOG(Log_Status, Warning, TEXT("Status Trigger Occurence #: %d for: %s"), StatusTriggerOccurrence,
           *GetOwner()->GetName());
    ClampAndTruncateStatusBuildup(StatusType);
}

void ChangeStatusBuildUpByPercent(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                                  const float StatusChangePercent)
{
    StatusInfo[StatusType] += StatusThreshold[StatusType] * StatusChangePercent;
    while (StatusInfo[StatusType] >= StatusThreshold[StatusType])
    {
        StatusInfo[StatusType] -= 100.f;
        StatusTriggerOccurrence++;
    }
    ClampAndTruncateStatusBuildup(StatusType);
}

float ReturnStatusChangeByPercent(Status_Stat_Component* status_stat_component, Damage_Type StatusType,
                                  float StatusChangePercent)
{
    return StatusThreshold[StatusType] * StatusChangePercent;
}

void SetStatusBuildUpToSpecificPercent(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                                       const float StatusChangeToSetTo)
{
    StatusInfo[StatusType] = StatusThreshold[StatusType] * StatusChangeToSetTo; // 100 * .7 = 70%
    ClampAndTruncateStatusBuildup(StatusType);
}


void ResetStatusBuildUp(Status_Stat_Component* status_stat_component, Damage_Type StatusTypeToReset)
{
    StatusInfo[StatusTypeToReset] = 0;
}

float ReturnDivisionValueBasedOnStatusBuildUp(Status_Stat_Component* status_stat_component, Damage_Type StatusType,
                                              float DivisionValue)
{
    return StatusInfo[StatusType] / DivisionValue;
}

float ReturnStatusChangeToZero(Status_Stat_Component* status_stat_component, Damage_Type StatusTypeToReset)
{
    return StatusInfo[StatusTypeToReset];
}

float ReturnBuildUpPercentByType(Status_Stat_Component* status_stat_component, Damage_Type StatusTypeToReturn)
{
    return StatusInfo[StatusTypeToReturn] / StatusThreshold[StatusTypeToReturn];
}

float ReturnBuildUpNonDecimalPercentByType(Status_Stat_Component* status_stat_component,
                                           Damage_Type StatusTypeToReturn)
{
    return FMath::TruncToInt((StatusInfo[StatusTypeToReturn] / StatusThreshold[StatusTypeToReturn]) * 100);
}


//status trigger
void ResetStatusTriggerOccurence(Status_Stat_Component* status_stat_component)
{
    StatusTriggerOccurrence = 0;
}

//threshold change
void ChangeStatusThreshold(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                           const float StatusChangeValue)
{
    StatusThreshold[StatusType] += StatusChangeValue;
    ClampStatusThreshold(StatusType);
}

void SetStatusThreshold(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                        const float StatusChangeValue)
{
    StatusThreshold[StatusType] += StatusThreshold[StatusType] * StatusChangeValue;
    ClampStatusThreshold(StatusType);
}

void SetStatusByMultiplication(Status_Stat_Component* status_stat_component, Damage_Type StatusType,
                               float StatusChangeValue)
{
    StatusThreshold[StatusType] *= StatusChangeValue;
    ClampStatusThreshold(StatusType);
}


#endif
