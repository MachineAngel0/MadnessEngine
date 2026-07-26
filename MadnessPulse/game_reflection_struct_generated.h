#include <stddef.h>
#include "game_constants.h"
#include "game_enums.h"
#include "game_structs.h"

#include "runtime_registry.h"

void generate_runtime_structs(Reflection_Registry* reflection_registry)
{
	Reflection_Runtime_Struct_Field Resistance_Resetter_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Resistance_Resetter_Component, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Resistance_Resetter_Component_Runtime_Struct =
	{
		.name = "Resistance_Resetter_Component",
		.fields = Resistance_Resetter_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Resistance_Resetter_Component)
	};

	reflection_registry_add_struct(reflection_registry, Resistance_Resetter_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field High_Low_Negation_Component_Fields[] =
	{
		{
			.name = "to_zero",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(High_Low_Negation_Component, to_zero)
		},
		{
			.name = "value_change",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(High_Low_Negation_Component, value_change)
		},
	};

	 Reflection_Runtime_Struct High_Low_Negation_Component_Runtime_Struct =
	{
		.name = "High_Low_Negation_Component",
		.fields = High_Low_Negation_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(High_Low_Negation_Component)
	};

	reflection_registry_add_struct(reflection_registry, High_Low_Negation_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Damage_Passive_Component_Fields[] =
	{
		{
			.name = "damage_passive_change_amount",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Damage_Passive_Component, damage_passive_change_amount)
		},
	};

	 Reflection_Runtime_Struct Damage_Passive_Component_Runtime_Struct =
	{
		.name = "Damage_Passive_Component",
		.fields = Damage_Passive_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Damage_Passive_Component)
	};

	reflection_registry_add_struct(reflection_registry, Damage_Passive_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Heal_Bounce_Component_Fields[] =
	{
		{
			.name = "heal_bounce_multiplier",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Heal_Bounce_Component, heal_bounce_multiplier)
		},
	};

	 Reflection_Runtime_Struct Heal_Bounce_Component_Runtime_Struct =
	{
		.name = "Heal_Bounce_Component",
		.fields = Heal_Bounce_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Heal_Bounce_Component)
	};

	reflection_registry_add_struct(reflection_registry, Heal_Bounce_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Conjure_Component_Fields[] =
	{
		{
			.name = "conjure_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Conjure_Type",
			.offset = offsetof(Conjure_Component, conjure_type)
		},
	};

	 Reflection_Runtime_Struct Conjure_Component_Runtime_Struct =
	{
		.name = "Conjure_Component",
		.fields = Conjure_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Conjure_Component)
	};

	reflection_registry_add_struct(reflection_registry, Conjure_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Sacrificial_Bomb_Component_Fields[] =
	{
		{
			.name = "element_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Element_Type",
			.offset = offsetof(Sacrificial_Bomb_Component, element_type)
		},
		{
			.name = "damage_multiplier",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Sacrificial_Bomb_Component, damage_multiplier)
		},
	};

	 Reflection_Runtime_Struct Sacrificial_Bomb_Component_Runtime_Struct =
	{
		.name = "Sacrificial_Bomb_Component",
		.fields = Sacrificial_Bomb_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Sacrificial_Bomb_Component)
	};

	reflection_registry_add_struct(reflection_registry, Sacrificial_Bomb_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Status_Threshold_Changer_Fields[] =
	{
		{
			.name = "status_threshold_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Status_Threshold_Types",
			.offset = offsetof(Status_Threshold_Changer, status_threshold_type)
		},
		{
			.name = "damage_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Element_Type",
			.offset = offsetof(Status_Threshold_Changer, damage_type)
		},
		{
			.name = "amount",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Status_Threshold_Changer, amount)
		},
	};

	 Reflection_Runtime_Struct Status_Threshold_Changer_Runtime_Struct =
	{
		.name = "Status_Threshold_Changer",
		.fields = Status_Threshold_Changer_Fields,
		.field_count = 3,
		.struct_size = sizeof(Status_Threshold_Changer)
	};

	reflection_registry_add_struct(reflection_registry, Status_Threshold_Changer_Runtime_Struct);

	Reflection_Runtime_Struct_Field Action_Remove_Component_Fields[] =
	{
		{
			.name = "action_amount_to_remove",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Action_Remove_Component, amount)
		},
	};

	 Reflection_Runtime_Struct Action_Remove_Component_Runtime_Struct =
	{
		.name = "Action_Remove_Component",
		.fields = Action_Remove_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Action_Remove_Component)
	};

	reflection_registry_add_struct(reflection_registry, Action_Remove_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Ability_Fields[] =
	{
		{
			.name = "id",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Ability, id)
		},
		{
			.name = "components",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Ability_Component",
			.offset = offsetof(Ability, normal_components)
		},
		{
			.name = "component_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Ability, normal_component_count)
		},
	};

	 Reflection_Runtime_Struct Ability_Runtime_Struct =
	{
		.name = "Ability",
		.fields = Ability_Fields,
		.field_count = 3,
		.struct_size = sizeof(Ability)
	};

	reflection_registry_add_struct(reflection_registry, Ability_Runtime_Struct);

	Reflection_Runtime_Struct_Field Targeting_Handler_Fields[] =
	{
		{
			.name = "current_lock_on_target",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Unit_Handle",
			.offset = offsetof(Targeting_Handler, current_lock_on_target)
		},
		{
			.name = "targeting_count",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Targeting_Handler, targeting_count)
		},
	};

	 Reflection_Runtime_Struct Targeting_Handler_Runtime_Struct =
	{
		.name = "Targeting_Handler",
		.fields = Targeting_Handler_Fields,
		.field_count = 2,
		.struct_size = sizeof(Targeting_Handler)
	};

	reflection_registry_add_struct(reflection_registry, Targeting_Handler_Runtime_Struct);

	Reflection_Runtime_Struct_Field Reversal_Component_Fields[] =
	{
		{
			.name = "reversal_duration",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Reversal_Duration",
			.offset = offsetof(Reversal_Base_Component, reversal_duration)
		},
		{
			.name = "AnythingNotDamage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, anything_not_damage)
		},
		{
			.name = "AnyDamage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, any_damage)
		},
		{
			.name = "Heal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, heal)
		},
		{
			.name = "Drain",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, drain)
		},
		{
			.name = "Mp",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, mp)
		},
		{
			.name = "DamageConditions",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, DamageConditions)
		},
		{
			.name = "AugmentCondition",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, AugmentCondition)
		},
		{
			.name = "ActionChangeCondition",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, ActionChangeCondition)
		},
		{
			.name = "AbilityChangeCondition",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, AbilityChangeCondition)
		},
		{
			.name = "NegationCondition",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, NegationCondition)
		},
		{
			.name = "DamagePassiveCondition",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Base_Component, DamagePassiveCondition)
		},
	};

	 Reflection_Runtime_Struct Reversal_Component_Runtime_Struct =
	{
		.name = "Reversal_Component",
		.fields = Reversal_Component_Fields,
		.field_count = 12,
		.struct_size = sizeof(Reversal_Base_Component)
	};

	reflection_registry_add_struct(reflection_registry, Reversal_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Reflection_Test_Struct_Fields[] =
	{
		{
			.name = "type",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Reflection_Test_Struct, type)
		},
		{
			.name = "type_ptr",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Reflection_Test_Struct, type_ptr)
		},
		{
			.name = "type_stack_ptr",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Reflection_Test_Struct, type_stack_ptr)
		},
		{
			.name = "enum_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Character_Name",
			.offset = offsetof(Reflection_Test_Struct, enum_type)
		},
		{
			.name = "enum_type_ptr",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Character_Name",
			.offset = offsetof(Reflection_Test_Struct, enum_type_ptr)
		},
		{
			.name = "enum_type_stack_ptr",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Character_Name",
			.offset = offsetof(Reflection_Test_Struct, enum_type_stack_ptr)
		},
		{
			.name = "struct_type",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Nonsense_Struct",
			.offset = offsetof(Reflection_Test_Struct, struct_type)
		},
		{
			.name = "struct_type_ptr",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Nonsense_Struct",
			.offset = offsetof(Reflection_Test_Struct, struct_type_ptr)
		},
		{
			.name = "struct_type_stack_ptr",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Nonsense_Struct",
			.offset = offsetof(Reflection_Test_Struct, struct_type_stack_ptr)
		},
	};

	 Reflection_Runtime_Struct Reflection_Test_Struct_Runtime_Struct =
	{
		.name = "Reflection_Test_Struct",
		.fields = Reflection_Test_Struct_Fields,
		.field_count = 9,
		.struct_size = sizeof(Reflection_Test_Struct)
	};

	reflection_registry_add_struct(reflection_registry, Reflection_Test_Struct_Runtime_Struct);

	Reflection_Runtime_Struct_Field Random_Ability_Remover_Component_Fields[] =
	{
		{
			.name = "removal_amount",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Random_Ability_Remover_Component, removal_amount)
		},
	};

	 Reflection_Runtime_Struct Random_Ability_Remover_Component_Runtime_Struct =
	{
		.name = "Random_Ability_Remover_Component",
		.fields = Random_Ability_Remover_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Random_Ability_Remover_Component)
	};

	reflection_registry_add_struct(reflection_registry, Random_Ability_Remover_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Drain_Bounce_Component_Fields[] =
	{
		{
			.name = "drain_twice",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Drain_Bounce_Component, drain_twice)
		},
		{
			.name = "heal_after_drain",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Drain_Bounce_Component, heal_after_drain)
		},
	};

	 Reflection_Runtime_Struct Drain_Bounce_Component_Runtime_Struct =
	{
		.name = "Drain_Bounce_Component",
		.fields = Drain_Bounce_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Drain_Bounce_Component)
	};

	reflection_registry_add_struct(reflection_registry, Drain_Bounce_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Ability_Remover_Component_Fields[] =
	{
		{
			.name = "abilities_to_remove",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Ability_Remover_Component, ability_to_remove)
		},
		{
			.name = "count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Ability_Remover_Component, count)
		},
	};

	 Reflection_Runtime_Struct Ability_Remover_Component_Runtime_Struct =
	{
		.name = "Ability_Remover_Component",
		.fields = Ability_Remover_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Ability_Remover_Component)
	};

	reflection_registry_add_struct(reflection_registry, Ability_Remover_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Status_Stat_Component_Fields[] =
	{
		{
			.name = "StatusInfo",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Status_Stat_Component, StatusInfo)
		},
		{
			.name = "StatusThreshold",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Status_Stat_Component, StatusThreshold)
		},
		{
			.name = "LowerClampValue",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Status_Stat_Component, LowerClampValue)
		},
		{
			.name = "UpperClampValue",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Status_Stat_Component, UpperClampValue)
		},
		{
			.name = "LowerThresholdClampValue",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Status_Stat_Component, LowerThresholdClampValue)
		},
		{
			.name = "UpperThresholdClampValue",
			.type = REFLECTION_TYPE_F32,
			.type_name = "f32",
			.offset = offsetof(Status_Stat_Component, UpperThresholdClampValue)
		},
	};

	 Reflection_Runtime_Struct Status_Stat_Component_Runtime_Struct =
	{
		.name = "Status_Stat_Component",
		.fields = Status_Stat_Component_Fields,
		.field_count = 6,
		.struct_size = sizeof(Status_Stat_Component)
	};

	reflection_registry_add_struct(reflection_registry, Status_Stat_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Drain_Battle_Info_Component_Fields[] =
	{
		{
			.name = "division_value",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Drain_Battle_Info_Component, division_value)
		},
		{
			.name = "gain_defense",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Drain_Battle_Info_Component, gain_defense)
		},
		{
			.name = "gain_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Drain_Battle_Info_Component, gain_damage)
		},
		{
			.name = "deal_damage_of_drain_lost",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Drain_Battle_Info_Component, deal_damage_of_drain_lost)
		},
	};

	 Reflection_Runtime_Struct Drain_Battle_Info_Component_Runtime_Struct =
	{
		.name = "Drain_Battle_Info_Component",
		.fields = Drain_Battle_Info_Component_Fields,
		.field_count = 4,
		.struct_size = sizeof(Drain_Battle_Info_Component)
	};

	reflection_registry_add_struct(reflection_registry, Drain_Battle_Info_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Inventory_Component_Fields[] =
	{
		{
			.name = "battle_list_starting",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Inventory_Component, battle_list_starting)
		},
		{
			.name = "battle_list_size",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Inventory_Component, battle_list_size)
		},
		{
			.name = "ability_reserve",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Inventory_Component, ability_reserve)
		},
		{
			.name = "ability_reserve_size",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Inventory_Component, ability_reserve_size)
		},
	};

	 Reflection_Runtime_Struct Inventory_Component_Runtime_Struct =
	{
		.name = "Inventory_Component",
		.fields = Inventory_Component_Fields,
		.field_count = 4,
		.struct_size = sizeof(Inventory_Component)
	};

	reflection_registry_add_struct(reflection_registry, Inventory_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Random_Augment_Component_Fields[] =
	{
		{
			.name = "augment_change_amount",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Random_Augment_Component, augment_change_amount)
		},
	};

	 Reflection_Runtime_Struct Random_Augment_Component_Runtime_Struct =
	{
		.name = "Random_Augment_Component",
		.fields = Random_Augment_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Random_Augment_Component)
	};

	reflection_registry_add_struct(reflection_registry, Random_Augment_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Negation_Based_On_Health_Component_Fields[] =
	{
		{
			.name = "division_value",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Negation_Based_On_Health_Component, division_value)
		},
	};

	 Reflection_Runtime_Struct Negation_Based_On_Health_Component_Runtime_Struct =
	{
		.name = "Negation_Based_On_Health_Component",
		.fields = Negation_Based_On_Health_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Negation_Based_On_Health_Component)
	};

	reflection_registry_add_struct(reflection_registry, Negation_Based_On_Health_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Turn_Trigger_Component_Info_Fields[] =
	{
		{
			.name = "turn_component_base_info",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Turn_Component_Base",
			.offset = offsetof(Turn_Trigger_Component_Info, turn_component_base_info)
		},
		{
			.name = "Ability",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Ability",
			.offset = offsetof(Turn_Trigger_Component_Info, Ability)
		},
		{
			.name = "TurnComponent",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Ability_Component",
			.offset = offsetof(Turn_Trigger_Component_Info, TurnComponent)
		},
		{
			.name = "Caster",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Unit",
			.offset = offsetof(Turn_Trigger_Component_Info, Caster)
		},
		{
			.name = "TurnTarget",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Unit",
			.offset = offsetof(Turn_Trigger_Component_Info, TurnTarget)
		},
	};

	 Reflection_Runtime_Struct Turn_Trigger_Component_Info_Runtime_Struct =
	{
		.name = "Turn_Trigger_Component_Info",
		.fields = Turn_Trigger_Component_Info_Fields,
		.field_count = 5,
		.struct_size = sizeof(Turn_Trigger_Component_Info)
	};

	reflection_registry_add_struct(reflection_registry, Turn_Trigger_Component_Info_Runtime_Struct);

	Reflection_Runtime_Struct_Field Primavera_Faith_Component_Fields[] =
	{
		{
			.name = "damage_amount",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Primavera_Faith_Component, damage_amount)
		},
	};

	 Reflection_Runtime_Struct Primavera_Faith_Component_Runtime_Struct =
	{
		.name = "Primavera_Faith_Component",
		.fields = Primavera_Faith_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Primavera_Faith_Component)
	};

	reflection_registry_add_struct(reflection_registry, Primavera_Faith_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Passive_Pooling_Component_Fields[] =
	{
		{
			.name = "elements_to_check_for",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Pooling_Component, elements_to_check_for)
		},
		{
			.name = "pool_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Pooling_Component, pool_damage)
		},
		{
			.name = "pool_status",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Pooling_Component, pool_status)
		},
		{
			.name = "pool_negation",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Pooling_Component, pool_negation)
		},
	};

	 Reflection_Runtime_Struct Passive_Pooling_Component_Runtime_Struct =
	{
		.name = "Passive_Pooling_Component",
		.fields = Passive_Pooling_Component_Fields,
		.field_count = 4,
		.struct_size = sizeof(Passive_Pooling_Component)
	};

	reflection_registry_add_struct(reflection_registry, Passive_Pooling_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Passive_Transfer_Component_Fields[] =
	{
		{
			.name = "min_max_value_for_transfer",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Passive_Transfer_Component, min_max_value_for_transfer)
		},
		{
			.name = "status_passive_to_transfer",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Transfer_Component, status_passive_to_transfer)
		},
		{
			.name = "damage_passives_to_transfer",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Transfer_Component, damage_passives_to_transfer)
		},
		{
			.name = "negation_passive_to_transfer",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Transfer_Component, negation_passive_to_transfer)
		},
	};

	 Reflection_Runtime_Struct Passive_Transfer_Component_Runtime_Struct =
	{
		.name = "Passive_Transfer_Component",
		.fields = Passive_Transfer_Component_Fields,
		.field_count = 4,
		.struct_size = sizeof(Passive_Transfer_Component)
	};

	reflection_registry_add_struct(reflection_registry, Passive_Transfer_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Damage_Bounce_Component_Fields[] =
	{
		{
			.name = "impact_mirror",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Damage_Bounce_Component, impact_mirror)
		},
		{
			.name = "damage_multiplier",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Damage_Bounce_Component, damage_multiplier)
		},
	};

	 Reflection_Runtime_Struct Damage_Bounce_Component_Runtime_Struct =
	{
		.name = "Damage_Bounce_Component",
		.fields = Damage_Bounce_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Damage_Bounce_Component)
	};

	reflection_registry_add_struct(reflection_registry, Damage_Bounce_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Game_Settings_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Game_Settings, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Game_Settings_Runtime_Struct =
	{
		.name = "Game_Settings",
		.fields = Game_Settings_Fields,
		.field_count = 1,
		.struct_size = sizeof(Game_Settings)
	};

	reflection_registry_add_struct(reflection_registry, Game_Settings_Runtime_Struct);

	Reflection_Runtime_Struct_Field Ability_Info_Fields[] =
	{
		{
			.name = "ability_name",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Ability_Info, ability_name)
		},
		{
			.name = "ability_primary_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Primary_Type",
			.offset = offsetof(Ability_Info, ability_primary_type)
		},
		{
			.name = "ability_secondary_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Secondary_Type",
			.offset = offsetof(Ability_Info, ability_secondary_type)
		},
		{
			.name = "ability_target_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Target_Type",
			.offset = offsetof(Ability_Info, ability_target_type)
		},
		{
			.name = "ability_target_area",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Target_Area_Affect",
			.offset = offsetof(Ability_Info, ability_target_area)
		},
		{
			.name = "ability_action_cost",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Action_Cost_Type",
			.offset = offsetof(Ability_Info, ability_action_cost)
		},
		{
			.name = "mp_cost",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Ability_Info, mp_cost)
		},
		{
			.name = "overflow_value",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Overflow_Value_Type",
			.offset = offsetof(Ability_Info, overflow_value)
		},
	};

	 Reflection_Runtime_Struct Ability_Info_Runtime_Struct =
	{
		.name = "Ability_Info",
		.fields = Ability_Info_Fields,
		.field_count = 8,
		.struct_size = sizeof(Ability_Info)
	};

	reflection_registry_add_struct(reflection_registry, Ability_Info_Runtime_Struct);

	Reflection_Runtime_Struct_Field Summoner_Component_Fields[] =
	{
		{
			.name = "summoner_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Summoner_Type",
			.offset = offsetof(Summoner_Component, summoner_type)
		},
	};

	 Reflection_Runtime_Struct Summoner_Component_Runtime_Struct =
	{
		.name = "Summoner_Component",
		.fields = Summoner_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Summoner_Component)
	};

	reflection_registry_add_struct(reflection_registry, Summoner_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Bonus_Based_On_Status_Component_Fields[] =
	{
		{
			.name = "elements_to_gain_bonus_from",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Bonus_Based_On_Status_Component, elements_to_gain_bonus_from)
		},
		{
			.name = "bonus_per_point",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Bonus_Based_On_Status_Component, bonus_per_point)
		},
		{
			.name = "gain_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Bonus_Based_On_Status_Component, gain_damage)
		},
		{
			.name = "gain_negation",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Bonus_Based_On_Status_Component, gain_negation)
		},
		{
			.name = "status_bar_element_to_increase",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Bonus_Based_On_Status_Component, status_bar_element_to_increase)
		},
		{
			.name = "gain_status_bar",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Bonus_Based_On_Status_Component, gain_status_bar)
		},
	};

	 Reflection_Runtime_Struct Bonus_Based_On_Status_Component_Runtime_Struct =
	{
		.name = "Bonus_Based_On_Status_Component",
		.fields = Bonus_Based_On_Status_Component_Fields,
		.field_count = 6,
		.struct_size = sizeof(Bonus_Based_On_Status_Component)
	};

	reflection_registry_add_struct(reflection_registry, Bonus_Based_On_Status_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Emperors_Insanity_Component_Fields[] =
	{
		{
			.name = "count_for_ability_to_be_removed",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Emperors_Insanity_Component, count_for_ability_to_be_removed)
		},
	};

	 Reflection_Runtime_Struct Emperors_Insanity_Component_Runtime_Struct =
	{
		.name = "Emperors_Insanity_Component",
		.fields = Emperors_Insanity_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Emperors_Insanity_Component)
	};

	reflection_registry_add_struct(reflection_registry, Emperors_Insanity_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Conjure_List_Component_Fields[] =
	{
		{
			.name = "active_conjure_list",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Conjure_List_Component, active_conjure_list)
		},
		{
			.name = "kill_player",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Conjure_List_Component, kill_player)
		},
	};

	 Reflection_Runtime_Struct Conjure_List_Component_Runtime_Struct =
	{
		.name = "Conjure_List_Component",
		.fields = Conjure_List_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Conjure_List_Component)
	};

	reflection_registry_add_struct(reflection_registry, Conjure_List_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Action_Component_Fields[] =
	{
		{
			.name = "MaxActionsAvailable",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Action_Component, max_actions_available)
		},
		{
			.name = "ActionsAvailable",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Action_Component, actions_available)
		},
	};

	 Reflection_Runtime_Struct Action_Component_Runtime_Struct =
	{
		.name = "Action_Component",
		.fields = Action_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Action_Component)
	};

	reflection_registry_add_struct(reflection_registry, Action_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Ability_Registry_Fields[] =
	{
		{
			.name = "ability_info",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Ability_Info",
			.offset = offsetof(Ability_Registry, ability_info)
		},
		{
			.name = "ability_info_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Ability_Registry, ability_info_count)
		},
		{
			.name = "ability_list",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Ability",
			.offset = offsetof(Ability_Registry, ability_list)
		},
	};

	 Reflection_Runtime_Struct Ability_Registry_Runtime_Struct =
	{
		.name = "Ability_Registry",
		.fields = Ability_Registry_Fields,
		.field_count = 3,
		.struct_size = sizeof(Ability_Registry)
	};

	reflection_registry_add_struct(reflection_registry, Ability_Registry_Runtime_Struct);

	Reflection_Runtime_Struct_Field Ability_Adder_Component_Fields[] =
	{
		{
			.name = "abilities_to_add",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Ability_Adder_Component, abilities_to_add)
		},
		{
			.name = "count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Ability_Adder_Component, count)
		},
	};

	 Reflection_Runtime_Struct Ability_Adder_Component_Runtime_Struct =
	{
		.name = "Ability_Adder_Component",
		.fields = Ability_Adder_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Ability_Adder_Component)
	};

	reflection_registry_add_struct(reflection_registry, Ability_Adder_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Primavera_Bloom_Component_Fields[] =
	{
		{
			.name = "resistance_type_to_check",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Resistance_Type",
			.offset = offsetof(Primavera_Bloom_Component, resistance_type_to_check)
		},
		{
			.name = "damage_amount",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Primavera_Bloom_Component, damage_amount)
		},
	};

	 Reflection_Runtime_Struct Primavera_Bloom_Component_Runtime_Struct =
	{
		.name = "Primavera_Bloom_Component",
		.fields = Primavera_Bloom_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Primavera_Bloom_Component)
	};

	reflection_registry_add_struct(reflection_registry, Primavera_Bloom_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Ability_Swapper_Component_Fields[] =
	{
		{
			.name = "number_of_abilities_to_swap",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Ability_Swapper_Component, number_of_abilities_to_swap)
		},
	};

	 Reflection_Runtime_Struct Ability_Swapper_Component_Runtime_Struct =
	{
		.name = "Ability_Swapper_Component",
		.fields = Ability_Swapper_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Ability_Swapper_Component)
	};

	reflection_registry_add_struct(reflection_registry, Ability_Swapper_Component_Runtime_Struct);




	Reflection_Runtime_Struct_Field Game_Level_Data_Fields[] =
	{
		{
			.name = "mission_name",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Level_Name",
			.offset = offsetof(Game_Level_Data, mission_name)
		},
		{
			.name = "mission_to_unlock",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Level_Name",
			.offset = offsetof(Game_Level_Data, mission_to_unlock)
		},
		{
			.name = "enemy_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Game_Level_Data, enemy_count)
		},
		{
			.name = "enemy_units",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Character_Name",
			.offset = offsetof(Game_Level_Data, enemy_units)
		},
		{
			.name = "starting_turn_initiative",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Turn_Initiative",
			.offset = offsetof(Game_Level_Data, starting_turn_initiative)
		},
		{
			.name = "is_debug_level",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Game_Level_Data, is_debug_level)
		},
		{
			.name = "ready_for_playtest",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Game_Level_Data, ready_for_playtest)
		},
	};

	 Reflection_Runtime_Struct Game_Level_Data_Runtime_Struct =
	{
		.name = "Game_Level_Data",
		.fields = Game_Level_Data_Fields,
		.field_count = 7,
		.struct_size = sizeof(Game_Level_Data)
	};

	reflection_registry_add_struct(reflection_registry, Game_Level_Data_Runtime_Struct);

	Reflection_Runtime_Struct_Field Random_Damage_Component_Fields[] =
	{
		{
			.name = "exclude_insanity",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Random_Damage_Component, exclude_insanity)
		},
		{
			.name = "intended_damage_amount",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Damage_Status_Type",
			.offset = offsetof(Random_Damage_Component, intended_damage_amount)
		},
		{
			.name = "intended_damage_build_up_amount",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Damage_Status_Type",
			.offset = offsetof(Random_Damage_Component, intended_damage_build_up_amount)
		},
	};

	 Reflection_Runtime_Struct Random_Damage_Component_Runtime_Struct =
	{
		.name = "Random_Damage_Component",
		.fields = Random_Damage_Component_Fields,
		.field_count = 3,
		.struct_size = sizeof(Random_Damage_Component)
	};

	reflection_registry_add_struct(reflection_registry, Random_Damage_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Heal_By_Target_Health_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Heal_By_Target_Health_Component, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Heal_By_Target_Health_Component_Runtime_Struct =
	{
		.name = "Heal_By_Target_Health_Component",
		.fields = Heal_By_Target_Health_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Heal_By_Target_Health_Component)
	};

	reflection_registry_add_struct(reflection_registry, Heal_By_Target_Health_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field High_Low_Augment_Component_Fields[] =
	{
		{
			.name = "high_low_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "High_Low_Type",
			.offset = offsetof(High_Low_Augment_Component, high_low_type)
		},
		{
			.name = "to_zero",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(High_Low_Augment_Component, to_zero)
		},
		{
			.name = "value_change",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(High_Low_Augment_Component, value_change)
		},
	};

	 Reflection_Runtime_Struct High_Low_Augment_Component_Runtime_Struct =
	{
		.name = "High_Low_Augment_Component",
		.fields = High_Low_Augment_Component_Fields,
		.field_count = 3,
		.struct_size = sizeof(High_Low_Augment_Component)
	};

	reflection_registry_add_struct(reflection_registry, High_Low_Augment_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Fusion_Mania_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Fusion_Mania_Component, unimplemented)
		},
		{
			.name = "kill_damage_amount",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Fusion_Mania_Component, kill_damage_amount)
		},
	};

	 Reflection_Runtime_Struct Fusion_Mania_Component_Runtime_Struct =
	{
		.name = "Fusion_Mania_Component",
		.fields = Fusion_Mania_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Fusion_Mania_Component)
	};

	reflection_registry_add_struct(reflection_registry, Fusion_Mania_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Nonsense_Struct_Fields[] =
	{
		{
			.name = "hi",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Nonsense_Struct, hi)
		},
	};

	 Reflection_Runtime_Struct Nonsense_Struct_Runtime_Struct =
	{
		.name = "Nonsense_Struct",
		.fields = Nonsense_Struct_Fields,
		.field_count = 1,
		.struct_size = sizeof(Nonsense_Struct)
	};

	reflection_registry_add_struct(reflection_registry, Nonsense_Struct_Runtime_Struct);

	Reflection_Runtime_Struct_Field Madness_Pulse_Game_Fields[] =
	{
		{
			.name = "game_state",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Madness_Pulse_Game_State",
			.offset = offsetof(Madness_Pulse_Game, game_state)
		},
		{
			.name = "save_meta_data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Save_Meta_Data",
			.offset = offsetof(Madness_Pulse_Game, save_meta_data)
		},
		{
			.name = "saves_found",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Pulse_Game, saves_found)
		},
		{
			.name = "current_save_slot_number",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Pulse_Game, current_save_slot_number)
		},
		{
			.name = "save_game",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Save_Game",
			.offset = offsetof(Madness_Pulse_Game, save_game)
		},
		{
			.name = "turn_phase",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Turn_Phase",
			.offset = offsetof(Madness_Pulse_Game, turn_phase)
		},
		{
			.name = "ability_registry",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Ability_Registry",
			.offset = offsetof(Madness_Pulse_Game, ability_registry)
		},
		{
			.name = "targeting_handler",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Targeting_Handler",
			.offset = offsetof(Madness_Pulse_Game, targeting_handler)
		},
		{
			.name = "ability_handler",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Ability_Handler",
			.offset = offsetof(Madness_Pulse_Game, ability_handler)
		},
		{
			.name = "command_handler",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Command_Handler",
			.offset = offsetof(Madness_Pulse_Game, command_handler)
		},
		{
			.name = "units_count",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Pulse_Game, units_count)
		},

		{
			.name = "unit_handles_count",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Pulse_Game, player_count)
		},

		{
			.name = "player_count",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Pulse_Game, player_count)
		},

		{
			.name = "enemy_count",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Madness_Pulse_Game, enemy_count)
		},
		{
			.name = "current_units_turn",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Unit_Handle",
			.offset = offsetof(Madness_Pulse_Game, current_units_turn)
		},
		{
			.name = "currently_selected_ability",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Madness_Pulse_Game, currently_selected_ability_by_player)
		},
		{
			.name = "picked_ability",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Madness_Pulse_Game, picked_ability)
		},
		{
			.name = "starting_turn_initiative",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Turn_Initiative",
			.offset = offsetof(Madness_Pulse_Game, starting_turn_initiative)
		},
	};

	 Reflection_Runtime_Struct Madness_Pulse_Game_Runtime_Struct =
	{
		.name = "Madness_Pulse_Game",
		.fields = Madness_Pulse_Game_Fields,
		.field_count = 21,
		.struct_size = sizeof(Madness_Pulse_Game)
	};

	reflection_registry_add_struct(reflection_registry, Madness_Pulse_Game_Runtime_Struct);

	Reflection_Runtime_Struct_Field High_Low_Damage_Component_Fields[] =
	{
		{
			.name = "to_zero",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(High_Low_Damage_Component, to_zero)
		},
		{
			.name = "value_change",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(High_Low_Damage_Component, value_change)
		},
	};

	 Reflection_Runtime_Struct High_Low_Damage_Component_Runtime_Struct =
	{
		.name = "High_Low_Damage_Component",
		.fields = High_Low_Damage_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(High_Low_Damage_Component)
	};

	reflection_registry_add_struct(reflection_registry, High_Low_Damage_Component_Runtime_Struct);




	Reflection_Runtime_Struct_Field MP_Action_Gainer_Component_Fields[] =
	{
		{
			.name = "action_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Action_Changer_Type",
			.offset = offsetof(MP_Action_Gainer_Component, action_type)
		},
		{
			.name = "mp_per_action_conversion",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(MP_Action_Gainer_Component, mp_per_action_conversion)
		},
	};

	 Reflection_Runtime_Struct MP_Action_Gainer_Component_Runtime_Struct =
	{
		.name = "MP_Action_Gainer_Component",
		.fields = MP_Action_Gainer_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(MP_Action_Gainer_Component)
	};

	reflection_registry_add_struct(reflection_registry, MP_Action_Gainer_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Rewind_Stats_Component_Fields[] =
	{
		{
			.name = "rewind_health",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Rewind_Stats_Component, rewind_health)
		},
		{
			.name = "rewind_status",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Rewind_Stats_Component, rewind_status)
		},
		{
			.name = "rewind_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Rewind_Stats_Component, rewind_damage)
		},
		{
			.name = "rewind_negation",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Rewind_Stats_Component, rewind_negation)
		},
	};

	 Reflection_Runtime_Struct Rewind_Stats_Component_Runtime_Struct =
	{
		.name = "Rewind_Stats_Component",
		.fields = Rewind_Stats_Component_Fields,
		.field_count = 4,
		.struct_size = sizeof(Rewind_Stats_Component)
	};

	reflection_registry_add_struct(reflection_registry, Rewind_Stats_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Command_Fields[] =
	{
		{
			.name = "is_skippable",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Command, is_skippable)
		},
	};

	 Reflection_Runtime_Struct Command_Runtime_Struct =
	{
		.name = "Command",
		.fields = Command_Fields,
		.field_count = 1,
		.struct_size = sizeof(Command)
	};

	reflection_registry_add_struct(reflection_registry, Command_Runtime_Struct);

	Reflection_Runtime_Struct_Field Heal_Component_Fields[] =
	{

		{
			.name = "heal_amount",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Heal_Component, heal_amount)
		},
		{
			.name = "heal_only_if_dead",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Heal_Component, heal_only_if_dead)
		},
	};

	 Reflection_Runtime_Struct Heal_Component_Runtime_Struct =
	{
		.name = "Heal_Component",
		.fields = Heal_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Heal_Component)
	};

	reflection_registry_add_struct(reflection_registry, Heal_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Resistance_Changer_Component_Fields[] =
	{
		{
			.name = "resistance_to_change_map",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Resistance_Type",
			.offset = offsetof(Resistance_Changer_Component, resistance_to_change_map)
		},
	};

	 Reflection_Runtime_Struct Resistance_Changer_Component_Runtime_Struct =
	{
		.name = "Resistance_Changer_Component",
		.fields = Resistance_Changer_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Resistance_Changer_Component)
	};

	reflection_registry_add_struct(reflection_registry, Resistance_Changer_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Emperor_Zero_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Emperor_Zero_Component, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Emperor_Zero_Component_Runtime_Struct =
	{
		.name = "Emperor_Zero_Component",
		.fields = Emperor_Zero_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Emperor_Zero_Component)
	};

	reflection_registry_add_struct(reflection_registry, Emperor_Zero_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Turn_Component_Base_Fields[] =
	{
		{
			.name = "turn_activation",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Turn_Activation_Type",
			.offset = offsetof(Turn_Base_Component, turn_activation)
		},
		{
			.name = "effect_length",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Turn_Base_Component, effect_length)
		},
		{
			.name = "turns_until_triggered",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Turn_Base_Component, turns_until_triggered)
		},
	};

	 Reflection_Runtime_Struct Turn_Component_Base_Runtime_Struct =
	{
		.name = "Turn_Component_Base",
		.fields = Turn_Component_Base_Fields,
		.field_count = 3,
		.struct_size = sizeof(Turn_Base_Component)
	};

	reflection_registry_add_struct(reflection_registry, Turn_Component_Base_Runtime_Struct);



	Reflection_Runtime_Struct_Field Status_Changer_Component_Fields[] =
	{
		{
			.name = "status_change_types",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Status_Change_Types",
			.offset = offsetof(Status_Changer_Component, status_change_types)
		},
		{
			.name = "damage_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Element_Type",
			.offset = offsetof(Status_Changer_Component, damage_type)
		},
		{
			.name = "status_change_amount",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Status_Changer_Component, status_change_amount)
		},
	};

	 Reflection_Runtime_Struct Status_Changer_Component_Runtime_Struct =
	{
		.name = "Status_Changer_Component",
		.fields = Status_Changer_Component_Fields,
		.field_count = 3,
		.struct_size = sizeof(Status_Changer_Component)
	};

	reflection_registry_add_struct(reflection_registry, Status_Changer_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Save_Game_Fields[] =
	{
		{
			.name = "meta_data",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Save_Meta_Data",
			.offset = offsetof(Save_Game, meta_data)
		},
		{
			.name = "player_save_info",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Player_Save_Data",
			.offset = offsetof(Save_Game, player_save_info)
		},
		{
			.name = "unlocked_levels",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Save_Game, unlocked_levels)
		},
		{
			.name = "allowed_to_fusion",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Save_Game, allowed_to_fusion)
		},
	};

	 Reflection_Runtime_Struct Save_Game_Runtime_Struct =
	{
		.name = "Save_Game",
		.fields = Save_Game_Fields,
		.field_count = 4,
		.struct_size = sizeof(Save_Game)
	};

	reflection_registry_add_struct(reflection_registry, Save_Game_Runtime_Struct);


	Reflection_Runtime_Struct_Field Emperor_Madness_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Emperor_Madness_Component, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Emperor_Madness_Component_Runtime_Struct =
	{
		.name = "Emperor_Madness_Component",
		.fields = Emperor_Madness_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Emperor_Madness_Component)
	};

	reflection_registry_add_struct(reflection_registry, Emperor_Madness_Component_Runtime_Struct);



	Reflection_Runtime_Struct_Field Negation_Passive_Setter_Component_Fields[] =
	{
		{
			.name = "set_to_value",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Negation_Passive_Setter_Component, set_to_value)
		},
	};

	 Reflection_Runtime_Struct Negation_Passive_Setter_Component_Runtime_Struct =
	{
		.name = "Negation_Passive_Setter_Component",
		.fields = Negation_Passive_Setter_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Negation_Passive_Setter_Component)
	};

	reflection_registry_add_struct(reflection_registry, Negation_Passive_Setter_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Resistance_Stats_Component_Fields[] =
	{
		{
			.name = "Resistance",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Resistance_Type",
			.offset = offsetof(Resistance_Stats_Component, Resistance)
		},
	};

	 Reflection_Runtime_Struct Resistance_Stats_Component_Runtime_Struct =
	{
		.name = "Resistance_Stats_Component",
		.fields = Resistance_Stats_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Resistance_Stats_Component)
	};

	reflection_registry_add_struct(reflection_registry, Resistance_Stats_Component_Runtime_Struct);








	Reflection_Runtime_Struct_Field Command_Handler_Fields[] =
	{
		{
			.name = "current_command",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Command",
			.offset = offsetof(Command_Handler, current_command)
		},
	};

	 Reflection_Runtime_Struct Command_Handler_Runtime_Struct =
	{
		.name = "Command_Handler",
		.fields = Command_Handler_Fields,
		.field_count = 1,
		.struct_size = sizeof(Command_Handler)
	};

	reflection_registry_add_struct(reflection_registry, Command_Handler_Runtime_Struct);

	Reflection_Runtime_Struct_Field Drain_Battle_Heal_Damage_Component_Fields[] =
	{
		{
			.name = "must_be_dead",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Drain_Battle_Heal_Damage_Component, must_be_dead)
		},
		{
			.name = "should_heal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Drain_Battle_Heal_Damage_Component, should_heal)
		},
		{
			.name = "should_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Drain_Battle_Heal_Damage_Component, should_damage)
		},
	};

	 Reflection_Runtime_Struct Drain_Battle_Heal_Damage_Component_Runtime_Struct =
	{
		.name = "Drain_Battle_Heal_Damage_Component",
		.fields = Drain_Battle_Heal_Damage_Component_Fields,
		.field_count = 3,
		.struct_size = sizeof(Drain_Battle_Heal_Damage_Component)
	};

	reflection_registry_add_struct(reflection_registry, Drain_Battle_Heal_Damage_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Passive_Reverse_Component_Fields[] =
	{
		{
			.name = "reverse_status",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Reverse_Component, reverse_status)
		},
		{
			.name = "reverse_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Reverse_Component, reverse_damage)
		},
		{
			.name = "reverse_negation",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Reverse_Component, reverse_negation)
		},
	};

	 Reflection_Runtime_Struct Passive_Reverse_Component_Runtime_Struct =
	{
		.name = "Passive_Reverse_Component",
		.fields = Passive_Reverse_Component_Fields,
		.field_count = 3,
		.struct_size = sizeof(Passive_Reverse_Component)
	};

	reflection_registry_add_struct(reflection_registry, Passive_Reverse_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Passive_Anti_Pooling_Component_Fields[] =
	{
		{
			.name = "elements_to_check_for",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Anti_Pooling_Component, elements_to_check_for)
		},
		{
			.name = "pool_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Anti_Pooling_Component, pool_damage)
		},
		{
			.name = "pool_status",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Anti_Pooling_Component, pool_status)
		},
		{
			.name = "pool_negation",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Passive_Anti_Pooling_Component, pool_negation)
		},
	};

	 Reflection_Runtime_Struct Passive_Anti_Pooling_Component_Runtime_Struct =
	{
		.name = "Passive_Anti_Pooling_Component",
		.fields = Passive_Anti_Pooling_Component_Fields,
		.field_count = 4,
		.struct_size = sizeof(Passive_Anti_Pooling_Component)
	};

	reflection_registry_add_struct(reflection_registry, Passive_Anti_Pooling_Component_Runtime_Struct);





	Reflection_Runtime_Struct_Field Heal_Asphyxiation_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Heal_Asphyxiation_Component, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Heal_Asphyxiation_Component_Runtime_Struct =
	{
		.name = "Heal_Asphyxiation_Component",
		.fields = Heal_Asphyxiation_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Heal_Asphyxiation_Component)
	};

	reflection_registry_add_struct(reflection_registry, Heal_Asphyxiation_Component_Runtime_Struct);



	Reflection_Runtime_Struct_Field Save_Meta_Data_Fields[] =
	{
		{
			.name = "magic_number",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Save_Meta_Data, magic_number)
		},
		{
			.name = "version",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Save_Meta_Data, version)
		},
		{
			.name = "slot_number",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Save_Meta_Data, slot_number)
		},
		{
			.name = "missions_beaten",
			.type = REFLECTION_TYPE_U8,
			.type_name = "u8",
			.offset = offsetof(Save_Meta_Data, missions_beaten)
		},
		{
			.name = "game_time",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Save_Meta_Data, game_time)
		},
	};

	 Reflection_Runtime_Struct Save_Meta_Data_Runtime_Struct =
	{
		.name = "Save_Meta_Data",
		.fields = Save_Meta_Data_Fields,
		.field_count = 5,
		.struct_size = sizeof(Save_Meta_Data)
	};

	reflection_registry_add_struct(reflection_registry, Save_Meta_Data_Runtime_Struct);

	Reflection_Runtime_Struct_Field Unit_Fields[] =
	{
		{
			.name = "character_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Character_Type",
			.offset = offsetof(Unit, character_type)
		},
		{
			.name = "name",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Character_Name",
			.offset = offsetof(Unit, name)
		},
		{
			.name = "character_state",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Character_State",
			.offset = offsetof(Unit, character_state)
		},
		{
			.name = "inventory_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Inventory_Component",
			.offset = offsetof(Unit, inventory_component)
		},
		{
			.name = "battle_inventory_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Battle_Inventory_Component",
			.offset = offsetof(Unit, battle_inventory_component)
		},
		{
			.name = "action_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Action_Component",
			.offset = offsetof(Unit, action_component)
		},
		{
			.name = "health_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Health_Component",
			.offset = offsetof(Unit, health_component)
		},
		{
			.name = "mp_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "MP_Component",
			.offset = offsetof(Unit, mp_component)
		},
		{
			.name = "augment_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Augment_Component",
			.offset = offsetof(Unit, augment_component)
		},
		{
			.name = "resistance_stats_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Resistance_Stats_Component",
			.offset = offsetof(Unit, resistance_stats_component)
		},
		{
			.name = "status_stat_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Status_Stat_Component",
			.offset = offsetof(Unit, status_stat_component)
		},
		{
			.name = "conjure_death_list_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Conjure_List_Component",
			.offset = offsetof(Unit, conjure_death_list_component)
		},
		{
			.name = "reversal_list_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Reversal_List_Component",
			.offset = offsetof(Unit, reversal_list_component)
		},
		{
			.name = "charge_list_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Charge_List_Component",
			.offset = offsetof(Unit, charge_list_component)
		},
		{
			.name = "special_ability_flag_list_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Special_Ability_Flag_List_Component",
			.offset = offsetof(Unit, special_ability_flag_list_component)
		},
	};

	 Reflection_Runtime_Struct Unit_Runtime_Struct =
	{
		.name = "Unit",
		.fields = Unit_Fields,
		.field_count = 15,
		.struct_size = sizeof(Unit)
	};

	reflection_registry_add_struct(reflection_registry, Unit_Runtime_Struct);

	Reflection_Runtime_Struct_Field Augment_Redistribution_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Augment_Redistribution_Component, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Augment_Redistribution_Component_Runtime_Struct =
	{
		.name = "Augment_Redistribution_Component",
		.fields = Augment_Redistribution_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Augment_Redistribution_Component)
	};

	reflection_registry_add_struct(reflection_registry, Augment_Redistribution_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Augment_Setter_Component_Fields[] =
	{
		{
			.name = "augment_type_to_setter_value",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Augment_Setter_Component, augment_type_to_setter_value)
		},
		{
			.name = "special_condition",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Augment_Setter_Component, special_condition)
		},
		{
			.name = "must_be_below_certain_value",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Augment_Setter_Component, must_be_below_certain_value)
		},
	};

	 Reflection_Runtime_Struct Augment_Setter_Component_Runtime_Struct =
	{
		.name = "Augment_Setter_Component",
		.fields = Augment_Setter_Component_Fields,
		.field_count = 3,
		.struct_size = sizeof(Augment_Setter_Component)
	};

	reflection_registry_add_struct(reflection_registry, Augment_Setter_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Reversal_Component_Info_Fields[] =
	{
		{
			.name = "Ability",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Ability",
			.offset = offsetof(Reversal_Component_Info, Ability)
		},
		{
			.name = "ReversalComponent",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Reversal_Component",
			.offset = offsetof(Reversal_Component_Info, ReversalComponent)
		},
		{
			.name = "Caster",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Unit",
			.offset = offsetof(Reversal_Component_Info, Caster)
		},
		{
			.name = "ReversalTarget",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Unit",
			.offset = offsetof(Reversal_Component_Info, ReversalTarget)
		},
		{
			.name = "IsPermanent",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_Component_Info, IsPermanent)
		},
	};

	 Reflection_Runtime_Struct Reversal_Component_Info_Runtime_Struct =
	{
		.name = "Reversal_Component_Info",
		.fields = Reversal_Component_Info_Fields,
		.field_count = 5,
		.struct_size = sizeof(Reversal_Component_Info)
	};

	reflection_registry_add_struct(reflection_registry, Reversal_Component_Info_Runtime_Struct);

	Reflection_Runtime_Struct_Field Damage_Component_Fields[] =
	{
		{
			.name = "element",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Element_Type",
			.offset = offsetof(Damage_Component, element)
		},
		{
			.name = "damage",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Damage_Component, damage)
		},
	};

	 Reflection_Runtime_Struct Damage_Component_Runtime_Struct =
	{
		.name = "Damage_Component",
		.fields = Damage_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Damage_Component)
	};

	reflection_registry_add_struct(reflection_registry, Damage_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Ability_Component_Fields[] =
	{


		{
			.name = "type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Component_Type",
			.offset = offsetof(Ability_Component, type)
		},
		{
			.name = "heal",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Heal_Component",
			.offset = offsetof(Ability_Component, data.heal)
		},
		{
			.name = "damage",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Damage_Component",
			.offset = offsetof(Ability_Component, data.damage)
		},

	};

	 Reflection_Runtime_Struct Ability_Component_Runtime_Struct =
	{
		.name = "Ability_Component",
		.fields = Ability_Component_Fields,
		.field_count = 3,
		.struct_size = sizeof(Ability_Component)
	};

	reflection_registry_add_struct(reflection_registry, Ability_Component_Runtime_Struct);



	Reflection_Runtime_Struct_Field Status_Drain_Disfigured_Mass_Component_Fields[] =
	{
		{
			.name = "division_value_bonus",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Status_Drain_Disfigured_Mass_Component, division_value_bonus)
		},
		{
			.name = "affect_status",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Status_Drain_Disfigured_Mass_Component, affect_status)
		},
		{
			.name = "affect_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Status_Drain_Disfigured_Mass_Component, affect_damage)
		},
		{
			.name = "affect_negation",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Status_Drain_Disfigured_Mass_Component, affect_negation)
		},
		{
			.name = "deal_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Status_Drain_Disfigured_Mass_Component, deal_damage)
		},
	};

	 Reflection_Runtime_Struct Status_Drain_Disfigured_Mass_Component_Runtime_Struct =
	{
		.name = "Status_Drain_Disfigured_Mass_Component",
		.fields = Status_Drain_Disfigured_Mass_Component_Fields,
		.field_count = 5,
		.struct_size = sizeof(Status_Drain_Disfigured_Mass_Component)
	};

	reflection_registry_add_struct(reflection_registry, Status_Drain_Disfigured_Mass_Component_Runtime_Struct);



	Reflection_Runtime_Struct_Field Health_Component_Fields[] =
	{
		{
			.name = "current_health",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Health_Component, current_health)
		},
		{
			.name = "max_health",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Health_Component, max_health)
		},
		{
			.name = "max_health_limit",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Health_Component, max_health_limit)
		},
		{
			.name = "min_health_limit",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Health_Component, min_health_limit)
		},

	};

	 Reflection_Runtime_Struct Health_Component_Runtime_Struct =
	{
		.name = "Health_Component",
		.fields = Health_Component_Fields,
		.field_count = 4,
		.struct_size = sizeof(Health_Component)
	};

	reflection_registry_add_struct(reflection_registry, Health_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Damage_Negation_Component_Fields[] =
	{
		{
			.name = "damage_negation_change_amount",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Damage_Negation_Component, damage_negation_change_amount)
		},
	};

	 Reflection_Runtime_Struct Damage_Negation_Component_Runtime_Struct =
	{
		.name = "Damage_Negation_Component",
		.fields = Damage_Negation_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Damage_Negation_Component)
	};

	reflection_registry_add_struct(reflection_registry, Damage_Negation_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field MP_Heal_Component_Fields[] =
	{
		{
			.name = "heal_multiplier",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(MP_Heal_Component, heal_multiplier)
		},
	};

	 Reflection_Runtime_Struct MP_Heal_Component_Runtime_Struct =
	{
		.name = "MP_Heal_Component",
		.fields = MP_Heal_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(MP_Heal_Component)
	};

	reflection_registry_add_struct(reflection_registry, MP_Heal_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Action_Add_Component_Fields[] =
	{
		{
			.name = "action_amount_to_add",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Action_Add_Component, amount)
		},
	};

	 Reflection_Runtime_Struct Action_Add_Component_Runtime_Struct =
	{
		.name = "Action_Add_Component",
		.fields = Action_Add_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Action_Add_Component)
	};

	reflection_registry_add_struct(reflection_registry, Action_Add_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Resistance_Changer_Random_Component_Fields[] =
	{
		{
			.name = "random_amount",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Resistance_Changer_Random_Component, random_amount)
		},
		{
			.name = "resistance_to_change_to",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Resistance_Type",
			.offset = offsetof(Resistance_Changer_Random_Component, resistance_to_change_to)
		},
	};

	 Reflection_Runtime_Struct Resistance_Changer_Random_Component_Runtime_Struct =
	{
		.name = "Resistance_Changer_Random_Component",
		.fields = Resistance_Changer_Random_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Resistance_Changer_Random_Component)
	};

	reflection_registry_add_struct(reflection_registry, Resistance_Changer_Random_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field High_Low_Type_Augment_Component_Fields[] =
	{
		{
			.name = "augment_type_to_change_value",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(High_Low_Type_Augment_Component, augment_type_to_change_value)
		},
		{
			.name = "high_low",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "High_Low_Type",
			.offset = offsetof(High_Low_Type_Augment_Component, high_low)
		},
	};

	 Reflection_Runtime_Struct High_Low_Type_Augment_Component_Runtime_Struct =
	{
		.name = "High_Low_Type_Augment_Component",
		.fields = High_Low_Type_Augment_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(High_Low_Type_Augment_Component)
	};

	reflection_registry_add_struct(reflection_registry, High_Low_Type_Augment_Component_Runtime_Struct);



	Reflection_Runtime_Struct_Field Character_Flags_Component_Fields[] =
	{
		{
			.name = "death_animation_flag",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Character_Flags_Component, death_animation_flag)
		},
		{
			.name = "revive_animation_flag",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Character_Flags_Component, revive_animation_flag)
		},
	};

	 Reflection_Runtime_Struct Character_Flags_Component_Runtime_Struct =
	{
		.name = "Character_Flags_Component",
		.fields = Character_Flags_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Character_Flags_Component)
	};

	reflection_registry_add_struct(reflection_registry, Character_Flags_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Augment_Changer_Component_Fields[] =
	{
		{
			.name = "damage_type",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Element_Type",
			.offset = offsetof(Augment_Changer_Component, damage_type)
		},
		{
			.name = "augment_change_value",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Augment_Changer_Component, augment_change_value)
		},
	};

	 Reflection_Runtime_Struct Augment_Changer_Component_Runtime_Struct =
	{
		.name = "Augment_Changer_Component",
		.fields = Augment_Changer_Component_Fields,
		.field_count = 2,
		.struct_size = sizeof(Augment_Changer_Component)
	};

	reflection_registry_add_struct(reflection_registry, Augment_Changer_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Primavera_Light_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Primavera_Light_Component, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Primavera_Light_Component_Runtime_Struct =
	{
		.name = "Primavera_Light_Component",
		.fields = Primavera_Light_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Primavera_Light_Component)
	};

	reflection_registry_add_struct(reflection_registry, Primavera_Light_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Ability_Component_List_Fields[] =
	{
		{
			.name = "damage_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Damage_Component",
			.offset = offsetof(Ability_Component_List, damage_component)
		},
		{
			.name = "passive_reverse_component",
			.type = REFLECTION_TYPE_STRUCT,
			.type_name = "Passive_Reverse_Component",
			.offset = offsetof(Ability_Component_List, passive_reverse_component)
		},
	};

	 Reflection_Runtime_Struct Ability_Component_List_Runtime_Struct =
	{
		.name = "Ability_Component_List",
		.fields = Ability_Component_List_Fields,
		.field_count = 2,
		.struct_size = sizeof(Ability_Component_List)
	};

	reflection_registry_add_struct(reflection_registry, Ability_Component_List_Runtime_Struct);

	Reflection_Runtime_Struct_Field Special_Ability_Flag_List_Component_Fields[] =
	{
		{
			.name = "FusionMania",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, FusionMania)
		},
		{
			.name = "BlackSun",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, BlackSun)
		},
		{
			.name = "EmptySun",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, EmptySun)
		},
		{
			.name = "AbyssalSun",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, AbyssalSun)
		},
		{
			.name = "RampartTaunt",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, RampartTaunt)
		},
		{
			.name = "InfiniteMPFlag",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, InfiniteMPFlag)
		},
		{
			.name = "PermanentInfiniteMPFlag",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, PermanentInfiniteMPFlag)
		},
		{
			.name = "DesperateMagic",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, DesperateMagic)
		},
		{
			.name = "Mirage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, Mirage)
		},
		{
			.name = "MirageCount",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Ability_Flag_List_Component, MirageCount)
		},
		{
			.name = "DanceInTheDark",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, DanceInTheDark)
		},
		{
			.name = "NonExistence",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, NonExistence)
		},
		{
			.name = "TrueExistence",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_List_Component, TrueExistence)
		},
	};

	 Reflection_Runtime_Struct Special_Ability_Flag_List_Component_Runtime_Struct =
	{
		.name = "Special_Ability_Flag_List_Component",
		.fields = Special_Ability_Flag_List_Component_Fields,
		.field_count = 13,
		.struct_size = sizeof(Ability_Flag_List_Component)
	};

	reflection_registry_add_struct(reflection_registry, Special_Ability_Flag_List_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Remove_All_Ability_Component_Fields[] =
	{
		{
			.name = "ability_to_remove",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Ability_Remove_All_Component, ability_to_remove)
		},
	};

	 Reflection_Runtime_Struct Remove_All_Ability_Component_Runtime_Struct =
	{
		.name = "Remove_All_Ability_Component",
		.fields = Remove_All_Ability_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Ability_Remove_All_Component)
	};

	reflection_registry_add_struct(reflection_registry, Remove_All_Ability_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Kill_By_Condition_Component_Fields[] =
	{
		{
			.name = "use_kill_by_health_above_percent",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Kill_By_Condition_Component, use_kill_by_health_above_percent)
		},
		{
			.name = "health_percent_for_kill",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Kill_By_Condition_Component, health_percent_for_kill)
		},
		{
			.name = "use_kill_by_mana_above_percent",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Kill_By_Condition_Component, use_kill_by_mana_above_percent)
		},
		{
			.name = "mp_percent_for_kill",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Kill_By_Condition_Component, mp_percent_for_kill)
		},
	};

	 Reflection_Runtime_Struct Kill_By_Condition_Component_Runtime_Struct =
	{
		.name = "Kill_By_Condition_Component",
		.fields = Kill_By_Condition_Component_Fields,
		.field_count = 4,
		.struct_size = sizeof(Kill_By_Condition_Component)
	};

	reflection_registry_add_struct(reflection_registry, Kill_By_Condition_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Damage_Based_On_Passive_Component_Fields[] =
	{
		{
			.name = "use_status",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Damage_Based_On_Passive_Component, use_status)
		},
		{
			.name = "use_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Damage_Based_On_Passive_Component, use_damage)
		},
		{
			.name = "use_negation",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Damage_Based_On_Passive_Component, use_negation)
		},
		{
			.name = "damage_amount_per_point",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Damage_Based_On_Passive_Component, damage_amount_per_point)
		},
		{
			.name = "apply_damage_every_point_amount",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Damage_Based_On_Passive_Component, apply_damage_every_point_amount)
		},
	};

	 Reflection_Runtime_Struct Damage_Based_On_Passive_Component_Runtime_Struct =
	{
		.name = "Damage_Based_On_Passive_Component",
		.fields = Damage_Based_On_Passive_Component_Fields,
		.field_count = 5,
		.struct_size = sizeof(Damage_Based_On_Passive_Component)
	};

	reflection_registry_add_struct(reflection_registry, Damage_Based_On_Passive_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field MP_Component_Fields[] =
	{
		{
			.name = "current_mp",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(MP_Component, current_mp)
		},
		{
			.name = "max_mp",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(MP_Component, max_mp)
		},
		{
			.name = "MaxAllowedMP",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(MP_Component, MaxAllowedMP)
		},
	};

	 Reflection_Runtime_Struct MP_Component_Runtime_Struct =
	{
		.name = "MP_Component",
		.fields = MP_Component_Fields,
		.field_count = 3,
		.struct_size = sizeof(MP_Component)
	};

	reflection_registry_add_struct(reflection_registry, MP_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Reversal_List_Component_Fields[] =
	{
		{
			.name = "unit_who_hit_us",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Reversal_List_Component, unit_who_hit_us)
		},
		{
			.name = "DamageReversal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_List_Component, DamageReversal)
		},
		{
			.name = "HealReversal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_List_Component, heal)
		},
		{
			.name = "DrainReversal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_List_Component, drain)
		},
		{
			.name = "MPReversal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_List_Component, mp)
		},
		{
			.name = "AugmentReversal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_List_Component, AugmentReversal)
		},
		{
			.name = "NegationPassiveReversal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_List_Component, NegationPassiveReversal)
		},
		{
			.name = "DamagePassiveReversal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_List_Component, DamagePassiveReversal)
		},
		{
			.name = "ActionChangeReversal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_List_Component, ActionChangeReversal)
		},
		{
			.name = "AbilityChangeReversal",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Reversal_List_Component, AbilityChangeReversal)
		},
	};

	 Reflection_Runtime_Struct Reversal_List_Component_Runtime_Struct =
	{
		.name = "Reversal_List_Component",
		.fields = Reversal_List_Component_Fields,
		.field_count = 10,
		.struct_size = sizeof(Reversal_List_Component)
	};

	reflection_registry_add_struct(reflection_registry, Reversal_List_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Trade_Action_Changer_Component_Fields[] =
	{
		{
			.name = "lose_all",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Action_Trade_Component, lose_all)
		},

	};

	 Reflection_Runtime_Struct Trade_Action_Changer_Component_Runtime_Struct =
	{
		.name = "Trade_Action_Changer_Component",
		.fields = Trade_Action_Changer_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Action_Trade_Component)
	};

	reflection_registry_add_struct(reflection_registry, Trade_Action_Changer_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Heal_Based_On_Damage_Taken_Component_Fields[] =
	{
		{
			.name = "heal_percentage",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Heal_Based_On_Damage_Taken_Component, heal_percentage)
		},
	};

	 Reflection_Runtime_Struct Heal_Based_On_Damage_Taken_Component_Runtime_Struct =
	{
		.name = "Heal_Based_On_Damage_Taken_Component",
		.fields = Heal_Based_On_Damage_Taken_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Heal_Based_On_Damage_Taken_Component)
	};

	reflection_registry_add_struct(reflection_registry, Heal_Based_On_Damage_Taken_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Special_Ability_Flag_Setter_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Ability_Flag_Component, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Special_Ability_Flag_Setter_Component_Runtime_Struct =
	{
		.name = "Special_Ability_Flag_Setter_Component",
		.fields = Special_Ability_Flag_Setter_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Ability_Flag_Component)
	};

	reflection_registry_add_struct(reflection_registry, Special_Ability_Flag_Setter_Component_Runtime_Struct);




	Reflection_Runtime_Struct_Field Health_Pooling_Transfer_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Health_Pooling_Transfer, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Health_Pooling_Transfer_Runtime_Struct =
	{
		.name = "Health_Pooling_Transfer",
		.fields = Health_Pooling_Transfer_Fields,
		.field_count = 1,
		.struct_size = sizeof(Health_Pooling_Transfer)
	};

	reflection_registry_add_struct(reflection_registry, Health_Pooling_Transfer_Runtime_Struct);

	Reflection_Runtime_Struct_Field Level_Unlock_Rewards_Fields[] =
	{
		{
			.name = "unlock_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Level_Unlock_Rewards, unlock_count)
		},
		{
			.name = "ability_unlock_array_ids",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Level_Unlock_Rewards, ability_unlock_array_ids)
		},
	};

	 Reflection_Runtime_Struct Level_Unlock_Rewards_Runtime_Struct =
	{
		.name = "Level_Unlock_Rewards",
		.fields = Level_Unlock_Rewards_Fields,
		.field_count = 2,
		.struct_size = sizeof(Level_Unlock_Rewards)
	};

	reflection_registry_add_struct(reflection_registry, Level_Unlock_Rewards_Runtime_Struct);

	Reflection_Runtime_Struct_Field Dead_Unit_Damage_Component_Fields[] =
	{
		{
			.name = "unimplemented",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Dead_Unit_Damage_Component, unimplemented)
		},
	};

	 Reflection_Runtime_Struct Dead_Unit_Damage_Component_Runtime_Struct =
	{
		.name = "Dead_Unit_Damage_Component",
		.fields = Dead_Unit_Damage_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Dead_Unit_Damage_Component)
	};

	reflection_registry_add_struct(reflection_registry, Dead_Unit_Damage_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Conditional_Fat_Component_Fields[] =
	{
		{
			.name = "test_condition",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Conditional_Component, test_condition)
		},
		{
			.name = "caster_mp_at_zero",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Conditional_Component, caster_mp_at_zero)
		},
		{
			.name = "caster_has_positive_damage",
			.type = REFLECTION_TYPE_BOOL,
			.type_name = "bool",
			.offset = offsetof(Conditional_Component, caster_has_positive_damage)
		},
	};

	 Reflection_Runtime_Struct Conditional_Fat_Component_Runtime_Struct =
	{
		.name = "Conditional_Fat_Component",
		.fields = Conditional_Fat_Component_Fields,
		.field_count = 3,
		.struct_size = sizeof(Conditional_Component)
	};

	reflection_registry_add_struct(reflection_registry, Conditional_Fat_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Primavera_Flower_Component_Fields[] =
	{
		{
			.name = "heal_multiplier",
			.type = REFLECTION_TYPE_F32,
			.type_name = "float",
			.offset = offsetof(Primavera_Flower_Component, heal_multiplier)
		},
	};

	 Reflection_Runtime_Struct Primavera_Flower_Component_Runtime_Struct =
	{
		.name = "Primavera_Flower_Component",
		.fields = Primavera_Flower_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Primavera_Flower_Component)
	};

	reflection_registry_add_struct(reflection_registry, Primavera_Flower_Component_Runtime_Struct);

	Reflection_Runtime_Struct_Field Augment_Swapper_Component_Fields[] =
	{
		{
			.name = "swap_attempts",
			.type = REFLECTION_TYPE_S32,
			.type_name = "int",
			.offset = offsetof(Augment_Swapper_Component, swap_attempts)
		},
	};

	 Reflection_Runtime_Struct Augment_Swapper_Component_Runtime_Struct =
	{
		.name = "Augment_Swapper_Component",
		.fields = Augment_Swapper_Component_Fields,
		.field_count = 1,
		.struct_size = sizeof(Augment_Swapper_Component)
	};

	reflection_registry_add_struct(reflection_registry, Augment_Swapper_Component_Runtime_Struct);



	Reflection_Runtime_Struct_Field Player_Save_Data_Fields[] =
	{
		{
			.name = "unit_id",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Character_Name",
			.offset = offsetof(Player_Save_Data, unit_id)
		},
		{
			.name = "battle_list_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Player_Save_Data, battle_list_count)
		},
		{
			.name = "reserve_list_count",
			.type = REFLECTION_TYPE_U32,
			.type_name = "u32",
			.offset = offsetof(Player_Save_Data, reserve_list_count)
		},
		{
			.name = "ability_battle_list_save",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Player_Save_Data, ability_battle_list_save)
		},
		{
			.name = "ability_reserve_list",
			.type = REFLECTION_TYPE_ENUM,
			.type_name = "Ability_Name",
			.offset = offsetof(Player_Save_Data, ability_reserve_list)
		},
	};

	 Reflection_Runtime_Struct Player_Save_Data_Runtime_Struct =
	{
		.name = "Player_Save_Data",
		.fields = Player_Save_Data_Fields,
		.field_count = 5,
		.struct_size = sizeof(Player_Save_Data)
	};

	reflection_registry_add_struct(reflection_registry, Player_Save_Data_Runtime_Struct);

}
