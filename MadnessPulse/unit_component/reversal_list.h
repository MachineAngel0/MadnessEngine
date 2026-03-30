#ifndef REVERSAL_LIST_H
#define REVERSAL_LIST_H

struct reversal_list{
	
	AUnitBase* UnitWhoHitUs = nullptr;

	// there shouldn't be a list for conjure, and turn based abilties
	// each reversal here will be 
	
	TSet<EDamageType> DamageReversal;

	// //todo: heal, drain, buff/debuff, action changer, mp change,
	// how do we deal with things like, amount of damage done? or reversing how much someone healed by?
	// global damage list, reversal triggered. something like that
	
	TSet<EHealTypes> HealReversal;

	TSet<EDrainTypes> DrainReversal;
	
	TSet<EMPTypes> MPReversal;

	TSet<EDamageType> AugmentReversal;

	bool NegationPassiveReversal  = false;

	bool DamagePassiveReversal = false;

	TSet<EActionChangerType> ActionChangeReversal;
	

	TSet<EAbilityChangerType> AbilityChangeReversal;

	
};


#endif
