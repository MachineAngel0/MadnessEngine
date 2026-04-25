#ifndef STATUS_STAT_COMPONENT_H
#define STATUS_STAT_COMPONENT_H
#include "game_enums.h"
#include "game_structs.h"
// Fill out your copyright notice in the Description page of Project Settings.


Status_Stat_Component Status_Stat_Component_create()
{
    Status_Stat_Component status_stat_component = {0};

    // status_stat_component.StatusInfo[Damage_Type_MAX] = {0};
    // status_stat_component.StatusThreshold[Damage_Type_MAX] = {0};

    status_stat_component.LowerClampValue = 0.0f;
    status_stat_component.UpperClampValue = 1000.0f; // this is just an arbitrary value

    status_stat_component.StatusTriggerOccurrence = 0;

    status_stat_component.LowerThresholdClampValue = 1.0f;
    // I dont know how low I actually want to let this go, 50 lowest maybe?
    status_stat_component.UpperThresholdClampValue = 10000.0f;

    return status_stat_component;
}

void Status_Stat_Component_init_default(Status_Stat_Component* status_stat_component)
{
    memset(status_stat_component, 0, sizeof(Status_Stat_Component));
    //TODO: This is so fucking wrong
    // status_stat_component->StatusInfo[Damage_Type_MAX];
    // status_stat_component->StatusThreshold[Damage_Type_MAX];

    status_stat_component->LowerClampValue = 0.0f;
    status_stat_component->UpperClampValue = 1000.0f; // this is just an arbitrary value

    status_stat_component->StatusTriggerOccurrence = 0;

    status_stat_component->LowerThresholdClampValue = 1.0f;
    // I dont know how low I actually want to let this go, 50 lowest maybe?
    status_stat_component->UpperThresholdClampValue = 10000.0f;
}


//utility
void ClampAndTruncateStatusBuildup(Status_Stat_Component* status_stat_component, const Damage_Type StatusTypeToCheck)
{
    status_stat_component->StatusInfo[StatusTypeToCheck] = clamp_float(
        status_stat_component->StatusInfo[StatusTypeToCheck], status_stat_component->LowerClampValue,
        status_stat_component->UpperClampValue);
    //truncate the value
    status_stat_component->StatusInfo[StatusTypeToCheck] = (u32)status_stat_component->StatusInfo[StatusTypeToCheck];
}

void ClampStatusThreshold(Status_Stat_Component* status_stat_component, const Damage_Type StatusTypeToCheck)
{
    status_stat_component->StatusInfo[StatusTypeToCheck] = clamp_float(
        status_stat_component->StatusInfo[StatusTypeToCheck], status_stat_component->LowerClampValue,
        status_stat_component->UpperClampValue);
}

//build-ups
void ChangeStatusBuildUp(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                         const float StatusChangeValue)
{
    status_stat_component->StatusInfo[StatusType] += StatusChangeValue;

    while (status_stat_component->StatusInfo[StatusType] >= status_stat_component->StatusThreshold[StatusType])
    {
        status_stat_component->StatusInfo[StatusType] -= status_stat_component->StatusThreshold[StatusType];
        status_stat_component->StatusTriggerOccurrence++;
    }
    // UE_LOG(Log_Status, Warning, TEXT("Status Trigger Occurence #: %d for: %s"), status_stat_component->StatusTriggerOccurrence,
    // *GetOwner()->GetName());
    ClampAndTruncateStatusBuildup(status_stat_component, StatusType);
}

void ChangeStatusBuildUpByPercent(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                                  const float StatusChangePercent)
{
    status_stat_component->StatusInfo[StatusType] += status_stat_component->StatusThreshold[StatusType] *
        StatusChangePercent;
    while (status_stat_component->StatusInfo[StatusType] >= status_stat_component->StatusThreshold[StatusType])
    {
        status_stat_component->StatusInfo[StatusType] -= 100.f;
        status_stat_component->StatusTriggerOccurrence++;
    }
    ClampAndTruncateStatusBuildup(status_stat_component, StatusType);
}

float ReturnStatusChangeByPercent(Status_Stat_Component* status_stat_component, Damage_Type StatusType,
                                  float StatusChangePercent)
{
    return status_stat_component->StatusThreshold[StatusType] * StatusChangePercent;
}

void SetStatusBuildUpToSpecificPercent(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                                       const float StatusChangeToSetTo)
{
    status_stat_component->StatusInfo[StatusType] = status_stat_component->StatusThreshold[StatusType] *
        StatusChangeToSetTo; // 100 * .7 = 70%
    ClampAndTruncateStatusBuildup(status_stat_component, StatusType);
}


void ResetStatusBuildUp(Status_Stat_Component* status_stat_component, Damage_Type StatusTypeToReset)
{
    status_stat_component->StatusInfo[StatusTypeToReset] = 0;
}

float ReturnDivisionValueBasedOnStatusBuildUp(Status_Stat_Component* status_stat_component, Damage_Type StatusType,
                                              float DivisionValue)
{
    return status_stat_component->StatusInfo[StatusType] / DivisionValue;
}

float ReturnStatusChangeToZero(Status_Stat_Component* status_stat_component, Damage_Type StatusTypeToReset)
{
    return status_stat_component->StatusInfo[StatusTypeToReset];
}

float ReturnBuildUpPercentByType(Status_Stat_Component* status_stat_component, Damage_Type StatusTypeToReturn)
{
    return status_stat_component->StatusInfo[StatusTypeToReturn] / status_stat_component->StatusThreshold[
        StatusTypeToReturn];
}

int ReturnBuildUpNonDecimalPercentByType(Status_Stat_Component* status_stat_component,
                                         Damage_Type StatusTypeToReturn)
{
    return (status_stat_component->StatusInfo[StatusTypeToReturn] / status_stat_component->StatusThreshold[
        StatusTypeToReturn]) * 100;
}


//status trigger
void ResetStatusTriggerOccurence(Status_Stat_Component* status_stat_component)
{
    status_stat_component->StatusTriggerOccurrence = 0;
}

//threshold change
void ChangeStatusThreshold(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                           const float StatusChangeValue)
{
    status_stat_component->StatusThreshold[StatusType] += StatusChangeValue;
    ClampStatusThreshold(status_stat_component, StatusType);
}

void SetStatusThreshold(Status_Stat_Component* status_stat_component, const Damage_Type StatusType,
                        const float StatusChangeValue)
{
    status_stat_component->StatusThreshold[StatusType] += status_stat_component->StatusThreshold[StatusType] *
        StatusChangeValue;
    ClampStatusThreshold(status_stat_component, StatusType);
}

void SetStatusByMultiplication(Status_Stat_Component* status_stat_component, Damage_Type StatusType,
                               float StatusChangeValue)
{
    status_stat_component->StatusThreshold[StatusType] *= StatusChangeValue;
    ClampStatusThreshold(status_stat_component, StatusType);
}


#endif
