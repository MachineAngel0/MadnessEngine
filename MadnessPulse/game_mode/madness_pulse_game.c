#include "madness_pulse_game.h"

#include "save_game.h"


Madness_Pulse_Game* madness_pulse_game_init(Memory_System* memory_system, Madness_UI* madness_ui,
                                            Event_System* event_system, Input_System* input, Resource_System*
                                            resource_system)
{
    Madness_Pulse_Game* game = memory_system_alloc(memory_system, sizeof(Madness_Pulse_Game), MEMORY_SUBSYSTEM_GAME);
    game->game_state = Game_State_Enum_Main_Menu;


    u64 game_memory_size = MB(64);
    void* game_memory = memory_system_alloc(memory_system, game_memory_size, MEMORY_SUBSYSTEM_GAME);
    void* game_frame_memory = memory_system_alloc(memory_system, game_memory_size, MEMORY_SUBSYSTEM_GAME);
    void* game_free_list_memory = memory_system_alloc(memory_system, game_memory_size, MEMORY_SUBSYSTEM_GAME);
    allocator_init(&game->allocator, game_memory, game_memory_size);
    allocator_init(&game->frame_allocator, game_frame_memory, game_memory_size);
    allocator_heap_init(&game->heap_allocator, game_free_list_memory, game_memory_size);

    game->madness_ui = madness_ui;
    game->event_system = event_system;
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
    allocator_clear(&game->frame_allocator);

    //TODO: DEBUG CODE
    if (input_key_released_unique(game->input_system, KEY_Q))
    {
        game->game_state = (game->game_state + 1) % (Game_State_Enum_MAX);
    }

    switch (game->game_state)
    {
    case Game_State_Enum_Main_Menu:
        //run the ui
        Madness_UI* ui = game->madness_ui;
        madness_ui_window_begin(ui, STRING("Main Menu"));

        if (madness_ui_button(ui, STRING("Load Game")))
        {
            if (filesystem_is_directory_empty(SAVE_GAME_PATH))
            {
                //TODO: pop up and a sound, saying to press new game
            }
            else
            {
                save_file_load_all_meta_data(game);
                game->game_state = Game_State_Load_Save;
            }
        }
        if (madness_ui_button(ui, STRING("New Game")))
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
        if (madness_ui_button(ui, STRING("Settings")))
        {
        }
        if (madness_ui_button(ui, STRING("Quit")))
        {
            Event_Data data = {0};
            event_fire(game->event_system, EVENT_APP_QUIT, STRING("Madness Pulse Game MAIN MENU"), data);
        }
        madness_ui_window_end(ui);
        break;
    case Game_State_Load_Save:
        // madness_pulse_load_save(game); // TODO:
        // game->game_state = Game_State_Enum_Turn_Based;
        madness_ui_window_begin(game->madness_ui, STRING("Load Game Save Slots"));
        {
            for (u32 i = 0; i < game->saves_found; i++)
            {
                if (madness_ui_button(game->madness_ui, STRING("Press To Load This Save")))
                {
                    //TODO: load up the intermediate screen
                    save_game_load(game, game->save_meta_data[i].slot_number);
                    if (!game->save_game) { MASSERT(false); }

                    game->game_state = Game_State_Enum_Turn_Based; // TODO: temp should be intermediate ui mode
                }
                madness_ui_u8(game->madness_ui, STRING("Slot Number:"), &game->save_meta_data[i].slot_number, 0);
                madness_ui_float(game->madness_ui, STRING("Game Version:"), &game->save_meta_data[i].version, 0);
                madness_ui_u8(game->madness_ui, STRING("Missions Beaten:"), &game->save_meta_data[i].missions_beaten,
                              0);
                madness_ui_padding(game->madness_ui, "");
            }
        }
        madness_ui_window_end(game->madness_ui);

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

    return true;
}


void madness_pulse_load_save(Madness_Pulse_Game* game)
{
    Madness_UI* ui = game->madness_ui;
    madness_ui_window_begin(ui, STRING("Load Save"));
    {
        madness_scroll_box_begin(ui, STRING("Load Save"));
        {
            for (int i = 0; i < MAX_SAVE_SLOTS; i++)
            {
                char buffer[100];
                sprintf(buffer, "Save #%d", i);
                if (madness_ui_button(ui, STRING(buffer)))
                {
                    //TODO: load the level based on the number
                    DEBUG("Save Selected: %d", i);

                    //create/load save data
                }
            }
        }
        madness_scroll_box_end(ui);
    }
    madness_ui_window_end(ui);
}


void madness_pulse_level_select(Madness_Pulse_Game* game)
{
    //run the ui
    madness_ui_window_begin(game->madness_ui, STRING("Level Select"));
    {
        madness_scroll_box_begin(game->madness_ui, STRING("level select"));
        {
            for (int i = 0; i < Level_Name_MAX; i++)
            {
                char buffer[100];
                sprintf(buffer, "Level %d", i);
                if (madness_ui_button(game->madness_ui, STRING(buffer)))
                {
                    //TODO: load the level based on the number
                    DEBUG("Level Seleced: %d", i);
                }
            }
        }
        madness_scroll_box_end(game->madness_ui);
    }
    madness_ui_window_end(game->madness_ui);
}


void madness_pulse_ability_select(Madness_Pulse_Game* game)
{
    Madness_UI* ui = game->madness_ui;
    madness_ui_window_begin(ui, STRING("Ability Inventory Select"));
    {
    }
    madness_ui_window_end(ui);
}
