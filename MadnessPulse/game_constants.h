#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H



//abilities
#define MAX_ABILITY_COMPONENTS 4u

#define INSTA_KILL_DAMAGE 999999u

//unit

#define MAX_HEALTH_DEFAULT 1000u
#define CURRENT_HEALTH_DEFAULT 1000u

#define INVENTORY_MAX_BATTLE_LIST 20u
#define STARTING_ABILITY_COUNT 1u


#define OVERFLOW_MAX 100u
#define ABILITY_COUNT_MAX 999u



//game state
#define MAX_PLAYER_UNIT_COUNT 4u

#define MAX_SAVE_SLOTS 255u //size of a u8
#define SAVE_GAME_PATH "../save"
#define SAVE_GAME_MAGIC_NUMBER "mads" //u8[4]
#define SAVE_GAME_VERSION 1.0f






//ACTION INTERFACE:
typedef void (*action_execute)(struct Madness_Pulse_Game*);
typedef void (*action_skip)(struct Madness_Pulse_Game*);






















#endif //GAME_CONSTANTS_H
