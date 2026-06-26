#ifndef SAVE_GAME_H
#define SAVE_GAME_H


#include <stdbool.h>

Ability_Name starting_abilities[] = {
    Ability_Name_Madness_Test,
    Ability_Name_DEBUG_1,
    Ability_Name_DEBUG_2,

    Ability_Name_Pass,

};
Ability_Name starting_reserve_inventory[] = {
    Ability_Name_INSANITY_Test,
    Ability_Name_DEBUG_3,
    Ability_Name_DEBUG_4,
};

_Static_assert(ARRAY_SIZE(starting_abilities) < INVENTORY_MAX_BATTLE_LIST);
_Static_assert(ARRAY_SIZE(starting_reserve_inventory) < INVENTORY_MAX_BATTLE_LIST);



// I want slots 255 max, I want to show the player all the slots available and they can choose to play on those slots
//the game should autosave, on a level complete and on ability selects, it's not like stm where you can fuck yourself over that badly

void save_game_setup(Madness_Pulse_Game* game)
{
    filesystem_create_directory(SAVE_GAME_PATH);

    //load up save games


    //scan directory for any save folders
    // "mp1.bin", "mp2.bin" ... etc
}

bool save_game_serialize(Save_Game* save_game, FILE* file)
{
    fwrite(&save_game->meta_data, 1, sizeof(Save_Meta_Data), file);

    for (u32 i = 0; i < MAX_PLAYER_UNIT_COUNT; i++)
    {
        save_game->player_save_info[i].unit_id;
        save_game->player_save_info[i].battle_list_count;
        save_game->player_save_info[i].reserve_list_count;

        fwrite(&save_game->player_save_info[i].unit_id, 1, sizeof(save_game->player_save_info[i].unit_id), file);
        fwrite(&save_game->player_save_info[i].battle_list_count, 1, sizeof(Ability_Name), file);
        fwrite(&save_game->player_save_info[i].reserve_list_count, 1, sizeof(Ability_Name), file);


        fwrite(&save_game->player_save_info[i].ability_battle_list_save, 1,
               sizeof(Ability_Name) * save_game->player_save_info[i].battle_list_count, file);
        fwrite(&save_game->player_save_info[i].ability_reserve_list, 1,
               sizeof(Ability_Name) * save_game->player_save_info[i].reserve_list_count, file);
    }

    fwrite(&save_game->unlocked_levels, 1, ARRAY_SIZE(save_game->unlocked_levels), file);
    fwrite(&save_game->allowed_to_fusion, 1, sizeof(save_game->allowed_to_fusion), file);

    return true;
}

bool save_game_deserialize(Madness_Pulse_Game* game, Save_Game* save_game, FILE* file)
{

    fread(&save_game->meta_data, 1, sizeof(Save_Meta_Data), file);

    for (u32 i = 0; i < MAX_PLAYER_UNIT_COUNT; i++)
    {
        save_game->player_save_info[i].unit_id;
        save_game->player_save_info[i].battle_list_count;
        save_game->player_save_info[i].reserve_list_count;

        fread(&save_game->player_save_info[i].unit_id, 1, sizeof(save_game->player_save_info[i].unit_id), file);
        fread(&save_game->player_save_info[i].battle_list_count, 1, sizeof(Ability_Name), file);
        fread(&save_game->player_save_info[i].reserve_list_count, 1, sizeof(Ability_Name), file);


        fread(&save_game->player_save_info[i].ability_battle_list_save, 1,
               sizeof(Ability_Name) * save_game->player_save_info[i].battle_list_count, file);
        fread(&save_game->player_save_info[i].ability_reserve_list, 1,
               sizeof(Ability_Name) * save_game->player_save_info[i].reserve_list_count, file);
    }

    fread(&save_game->unlocked_levels, 1, ARRAY_SIZE(save_game->unlocked_levels), file);
    fread(&save_game->allowed_to_fusion, 1, sizeof(save_game->allowed_to_fusion), file);

    return true;

}

bool save_game_load(Madness_Pulse_Game* game, const u32 selected_slot)
{
    if (!game->save_game)
    {
        game->save_game = allocator_alloc(&game->allocator, sizeof(Save_Game));
    }
    memset(game->save_game, 0, sizeof(Save_Game));

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/MPSAVE_%d.bin\0", SAVE_GAME_PATH, selected_slot);
    if (!filesystem_does_file_exists(filepath))
    {
        //this should never be reached
        MASSERT(false);
    }

    FILE* file = fopen(filepath, "rb");
    if (!file)
    {
        MASSERT(false);
    }
    save_game_deserialize(game, game->save_game, file);
    fclose(file);
    return true;

}



bool save_file_create(Madness_Pulse_Game* game)
{
    //TODO: WHEN YOU WAKE UP, loop over all the save files in the directory, then load the meta data

    bool found_free_slot = false;
    char filepath[1024];
    u32 save_slot_number = 0;
    for (; save_slot_number < MAX_SAVE_SLOTS; save_slot_number++)
    {
        // Create full path: "../saves/MPSAVE_0.bin"
        snprintf(filepath, sizeof(filepath), "%s/MPSAVE_%d.bin\0", SAVE_GAME_PATH, save_slot_number);

        if (filesystem_does_file_exists(filepath))
        {
            continue;
        }
        found_free_slot = true;
        break;
    }

    if (found_free_slot)
    {
        filesystem_create_file_platform(filepath);

        FILE* save_file = fopen(filepath, "wb");
        const Save_Meta_Data default_meta_data = {
            .magic_number = SAVE_GAME_MAGIC_NUMBER,
            .version = SAVE_GAME_VERSION,
            //used for the load game screen, so the player can see which save file they want
            .game_time = 0.f,
            .slot_number = save_slot_number,
            .missions_beaten = 0,
        };

        // MASSERT(bytes_written == sizeof(Save_Meta_Data));
        Player_Save_Data default_player_data = {
            .unit_id = Character_Name_Madness_Progenitor,
            .battle_list_count = ARRAY_SIZE(starting_abilities),
            .reserve_list_count = ARRAY_SIZE(starting_reserve_inventory),
            .ability_battle_list_save = starting_abilities,
            .ability_reserve_list = starting_reserve_inventory,
        };

        Save_Game save_game = {0};
        save_game.meta_data = default_meta_data;
        save_game.allowed_to_fusion = false;

        memset(save_game.unlocked_levels, 0, ARRAY_SIZE(save_game.unlocked_levels));
        save_game.unlocked_levels[Level_Name_Tutorial] = true;
        save_game.unlocked_levels[Level_Name_Worshipper] = true;

        default_player_data.unit_id = Character_Name_Madness_Progenitor;
        save_game.player_save_info[0] = default_player_data;

        default_player_data.unit_id = Character_Name_Madness_ButterFly;
        save_game.player_save_info[1] = default_player_data;

        default_player_data.unit_id = Character_Name_Madness_Wolf;
        save_game.player_save_info[2] = default_player_data;

        default_player_data.unit_id = Character_Name_Madness_Envoy;
        save_game.player_save_info[3] = default_player_data;

        save_game_serialize(&save_game, save_file);

        fclose(save_file);

        game->current_save_slot_number = save_slot_number;

        return true;
    }


    //cannot create a file
    return false;
}


bool save_file_load_all_meta_data(Madness_Pulse_Game* game)
{
    char filepath[1024];

    u8 found_files[MAX_SAVE_SLOTS];
    game->saves_found = 0;
    for (u32 save_slot_number = 0; save_slot_number < MAX_SAVE_SLOTS; save_slot_number++)
    {
        // Create full path: "../saves/MPSAVE_0.bin"
        snprintf(filepath, sizeof(filepath), "%s/MPSAVE_%d.bin\0", SAVE_GAME_PATH, save_slot_number);

        if (filesystem_does_file_exists(filepath))
        {
            found_files[game->saves_found++] = save_slot_number;
        }
    }


    for (u32 i = 0; i < game->saves_found; i++)
    {
        // Create full path: "../saves/MPSAVE_0.bin"
        u8 save_slot = found_files[i];
        snprintf(filepath, sizeof(filepath), "%s/MPSAVE_%d.bin\0", SAVE_GAME_PATH, save_slot);

        FILE* save_file = fopen(filepath, "rb");
        fread(&game->save_meta_data[i], 1, sizeof(Save_Meta_Data), save_file);
        fclose(save_file);
    }


    return true;
}


bool save_game_load_meta_data(Save_Meta_Data* out_meta_data, Allocator* arena)
{
    return false;
}

bool save_game_save_meta_data(Save_Meta_Data* meta_data_to_save, Allocator* arena, Frame_Allocator* frame)
{
    return false;
}


//
bool save_game_debug_build(Save_Game* out_save_game, Allocator* arena, Frame_Allocator* frame)
{
    return false;
}

bool save_game_demo_build(Save_Game* out_save_game, Allocator* arena, Frame_Allocator* frame)
{
    return false;
}


//called only during the fusion mania fight
void save_game_unlock_fusion(Save_Game* SaveGame)
{
}


//DEBUG
void save_game_debug_unlock_all_missions()
{
}

void save_game_debug_unlock_all_abilities()
{
}


#endif //SAVE_GAME_H
