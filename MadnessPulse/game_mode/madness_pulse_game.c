#include "madness_pulse_game.h"

#include "save_game.h"
#include "ui_insanity.h"


Madness_Pulse_Game* madness_pulse_game_init(Memory_System* memory_system,
                                            Input_System* input,
                                            Asset_System* resource_system)
{
    Madness_Pulse_Game* game = memory_system_alloc(memory_system, sizeof(Madness_Pulse_Game), MEMORY_SUBSYSTEM_GAME);
    game->game_state = Game_State_Enum_Main_Menu;


    u64 game_persistent_memory_size = MB(16);
    u64 game_frame_memory_size = MB(16);
    u64 game_dynamic_memory_size = MB(16);

    game->allocator = memory_system_allocator_create(memory_system, game_persistent_memory_size, MEMORY_SUBSYSTEM_GAME,
                                                     "game persistent");
    game->frame_allocator = memory_system_allocator_create(memory_system, game_frame_memory_size, MEMORY_SUBSYSTEM_GAME,
                                                           "game frame");
    game->heap_allocator = memory_system_heap_allocator_create(memory_system, game_dynamic_memory_size,
                                                               MEMORY_SUBSYSTEM_GAME, "game dynamic");


    game->resource_system = resource_system;
    game->input_system = input;


    save_game_setup(game);


    return game;
}

bool madness_pulse_game_shutdown(Madness_Pulse_Game* game, Memory_System* memory_system)
{
    memory_system_memory_free(memory_system, &game->allocator, MEMORY_SUBSYSTEM_GAME);
    // memory_system_memory_free(memory_system, &game->frame_arena, MEMORY_SUBSYSTEM_GAME);

    memory_system_memory_free(memory_system, game, MEMORY_SUBSYSTEM_GAME);
    return true;
}

bool madness_pulse_game_update(Madness_Pulse_Game* game, float delta_time)
{
    PROFILE_ZONE(GAME)


    allocator_clear(game->frame_allocator);

    //TODO: DEBUG CODE
    if (input_key_released_unique(KEY_Q))
    {
        game->game_state = (game->game_state + 1) % (Game_State_Enum_MAX);
    }

    switch (game->game_state)
    {
    case Game_State_Enum_Main_Menu:
        //run the ui
        ImGui_Begin("Main Menu", 0, 0);
        {
            if (ImGui_Button("Load Game"))
            {
                if (platform_is_directory_empty(SAVE_GAME_PATH))
                {
                    //TODO: pop up and a sound, saying to press new game
                }
                else
                {
                    save_file_load_all_meta_data(game);
                    game->game_state = Game_State_Load_Save;
                }
            }
            if (ImGui_Button("New Game"))
            {
                //create a new save file that does not already exist

                if (!save_file_create(game))
                {
                    //TODO: failed pop up, all slots are used up
                    MASSERT(false);
                }

                //set and load save slot function

                game->game_state = Game_State_Enum_Turn_Based; // TODO: temp
            }
            if (ImGui_Button("Settings"))
            {
            }
            if (ImGui_Button("Quit"))
            {
                Event_Data data = {0};
                event_fire(EVENT_APP_QUIT, STRING("Madness Pulse Game MAIN MENU"), data);
            }
        }
        ImGui_End();


        break;
    case Game_State_Load_Save:
        // madness_pulse_load_save(game); // TODO:
        // game->game_state = Game_State_Enum_Turn_Based;
        ImGui_Begin("Load Game Save Slots", 0, 0);
        {
            for (u32 i = 0; i < game->saves_found; i++)
            {
                ImGui_PushIDInt(i);
                if (ImGui_Button(("Press To Load This Save")))
                {
                    //TODO: load up the intermediate screen
                    save_game_load(game, game->save_meta_data[i].slot_number);
                    if (!game->save_game) { MASSERT(false); }

                    game->game_state = Game_State_Enum_Turn_Based; // TODO: temp should be intermediate ui mode
                }
                ImGui_Text("Slot Number: %d", &game->save_meta_data[i].slot_number);
                ImGui_Text("Game Version: %d", &game->save_meta_data[i].version);
                ImGui_Text("Missions Beaten: %d", &game->save_meta_data[i].missions_beaten);

                ImGui_PopID();
            }
        }
        ImGui_End();

        break;
    case Game_State_New_Save:

        break;
    case Game_State_Enum_Ability_Select:
        madness_pulse_ability_select(game);
        break;
    case Game_State_Enum_Level_Select:
        madness_pulse_level_select(game);
        break;
    case Game_State_Enum_Turn_Based:
        if (!game->save_game)
        {
            //load available save
            save_game_load(game, 0);
        }
        turn_update(game);
        break;
    case Game_State_Settings:
        break;
    case Game_State_Enum_MAX:
        break;
    }


    PROFILE_ZONE_END(GAME)

    return true;
}


void madness_pulse_load_save(Madness_Pulse_Game* game)
{
    madness_ui_window_begin(STRING("Load Save"), 0);
    {
        madness_scroll_box_begin(STRING("Load Save"));
        {
            for (int i = 0; i < MAX_SAVE_SLOTS; i++)
            {
                char buffer[100];
                sprintf(buffer, "Save #%d", i);
                if (madness_ui_button(STRING(buffer)))
                {
                    //TODO: load the level based on the number
                    DEBUG("Save Selected: %d", i);

                    //create/load save data
                }
            }
        }
        madness_scroll_box_end();
    }
    madness_ui_window_end();
}


void madness_pulse_level_select(Madness_Pulse_Game* game)
{
    //run the ui
    madness_ui_window_begin(STRING("Level Select"), 0);
    {
        madness_scroll_box_begin(STRING("level select"));
        {
            for (int i = 0; i < Level_Name_MAX; i++)
            {
                char buffer[100];
                sprintf(buffer, "Level %d", i);
                if (madness_ui_button(STRING(buffer)))
                {
                    //TODO: load the level based on the number
                    DEBUG("Level Seleced: %d", i);
                }
            }
        }
        madness_scroll_box_end();
    }
    madness_ui_window_end();
}


void madness_pulse_ability_select(Madness_Pulse_Game* game)
{
    madness_ui_window_begin(STRING("Ability Inventory Select"), 0);
    {
    }
    madness_ui_window_end();
}
