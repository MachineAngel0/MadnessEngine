#ifndef SAVE_GAME_H
#define SAVE_GAME_H
#include <stdbool.h>

#include "arena.h"
#include "game_structs.h"

/* Save data related things*/


typedef struct Game_Settings
{
    //game specific settings
    bool unimplemented;
} Game_Settings;



#define MAX_GAME_SAVE_SLOTS 255 //size of a u8



typedef struct Save_Meta_Data
{
    //used for the load game screen, so the player can see which save file they want

    u8 slot_number; //255 game save is more than enough
    float game_time;
    int missions_beaten;
    String save_name;
} Save_Meta_Data;


typedef struct Player_Save_Data
{
    u32 unit_id;

    u32 reserve_count;
    u32 battle_list_count;

    u32* ability_battle_list_save;
    u32* ability_reserve_list;
} Player_Save_Data;

typedef struct Save_Game
{
    Player_Save_Data PlayerSaveInfo[MAX_PLAYER_COUNT];

    //TODO: set the values needed to false
    // stores whether a level is unlocked or not,
    // they need to be the same name as the level
    bool unlocked_levels[Mission_Name_MAX];

    //TODO: make fusion available during the fusion fight, and save the setting permanently from there
    bool allowed_to_fusion = false;


} Save_Game;

//NOTE: THESE FUNCTION ARE NEW AND UNIMPLEMENTED
bool save_game_new_save(Save_Game* out_new_save, Arena* arena, Frame_Arena* frame);


bool save_game_load_meta_data(Save_Meta_Data* out_meta_data, Arena* arena);
bool save_game_save_meta_data(Save_Meta_Data* meta_data_to_save, Arena* arena, Frame_Arena* frame);

bool save_game_save(Save_Game* save_game, Arena* arena, Frame_Arena* frame);
bool save_game_load(Save_Game* out_save_game, Arena* arena, Frame_Arena* frame);

//
bool save_game_debug_build(Save_Game* out_save_game,Arena* arena, Frame_Arena* frame);
bool save_game_demo_build(Save_Game* out_save_game,Arena* arena, Frame_Arena* frame);


//called only during the fusion mania fight
void save_game_unlock_fusion(Save_Game* SaveGame);


//DEBUG
void save_game_debug_unlock_all_missions();
void save_game_debug_unlock_all_abilities();



#endif //SAVE_GAME_H
