#ifndef CONJURE_DEATH_LIST_COMPONENT_H
#define CONJURE_DEATH_LIST_COMPONENT_H


typedef struct conjure_death_list_component
{
    TSet<EConjureType> ConjureDeathList;
    bool IsDeathListEmpty();
    void AddDeathCondition(EConjureType ConjureTypeToAdd);

    bool KillPlayer = false;
}conjure_death_list_component;


#endif
