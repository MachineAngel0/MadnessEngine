#ifndef MP_COMPONENT_H
#define MP_COMPONENT_H

typedef struct MP_Component
{
	float* MP_States; // array

	//MP Struct
	float CurrentMP = 100;
	float MaxMP = 100;
	float MaxAllowedMP = MaxMP*2;
}MP_Component;


#endif
