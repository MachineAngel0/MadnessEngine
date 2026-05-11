#include "madness_pulse_game.h"


Madness_Pulse_Game* madness_pulse_game_init(Memory_System* memory_system, Madness_UI* madness_ui,
                                            Event_System* event_system, Input_System* input, Resource_System*
                                            resource_system)
{
    Madness_Pulse_Game* game = memory_system_alloc(memory_system, sizeof(Madness_Pulse_Game), MEMORY_SUBSYSTEM_GAME);
    game->game_state = Game_State_Enum_Main_Menu;

    u64 game_memory_size = GB(0.5);
    void* game_memory = memory_system_alloc(memory_system, game_memory_size, MEMORY_SUBSYSTEM_GAME);
    allocator_init(&game->allocator, game_memory, game_memory_size);


    game->madness_ui = madness_ui;
    game->event_system = event_system;
    game->resource_system = resource_system;
    game->input_system = input;


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
    if (input_key_released_unique(game->input_system, KEY_Q))
    {
        game->game_state = (game->game_state + 1) % (Game_State_Enum_MAX);
    }

    switch (game->game_state)
    {
    case Game_State_Enum_Main_Menu:
        madness_pulse_main_menu(game);
        break;
    case Game_State_Enum_Ability_Select:
        madness_pulse_ability_select(game);
        break;
    case Game_State_Enum_Level_Select:
        madness_pulse_level_select(game);
        break;
    case Game_State_Enum_Turn_Based:
        turn_update(game);
        break;
    case Game_State_Enum_MAX:
        break;
    }

    return true;
}

void madness_pulse_main_menu(Madness_Pulse_Game* game)
{
    //run the ui
    Madness_UI* ui = game->madness_ui;
    madness_ui_window_begin(ui, STRING("Main Menu"));


    if (madness_ui_button(ui, STRING("Load Game"), STRING("Load Game")))
    {
        //set new state for the load menu and load in menu metadata
        //TODO: for now just loading into the level select
        game->game_state = Game_State_Enum_Level_Select;
    }
    if (madness_ui_button(ui, STRING("Settings"), STRING("Settings")))
    {
    }
    if (madness_ui_button(ui, STRING("Quit"), STRING("Quit")))
    {
        Event_Data data = {0};
        event_fire(game->event_system, EVENT_APP_QUIT, STRING("Madness Pulse Game"), data);
    }
    madness_ui_window_end(ui);

}

void madness_pulse_level_select(Madness_Pulse_Game* game)
{
    //run the ui
    Madness_UI* ui = game->madness_ui;
    madness_ui_window_begin(ui, STRING("Level Select"));
    static scroll_box_state scroll_state;
    scroll_state.max_nodes_to_display = 10;
    madness_scroll_box_begin(game->madness_ui, STRING("level select"), &scroll_state);
    {
        for (int i = 0; i < 30; i++)
        {
            char buffer[100];
            sprintf(buffer, "Level %d", i);
            if (madness_ui_button(ui, STRING(buffer), STRING(buffer)))
            {
                //TODO: load the level based on the number
                DEBUG("Level Seleced: %d", i);
            }
        }
    }
    madness_scroll_box_end(game->madness_ui, STRING("level select"), &scroll_state);

    madness_ui_window_end(ui);

}


void madness_pulse_ability_select(Madness_Pulse_Game* game)
{
    Madness_UI* ui = game->madness_ui;
    madness_ui_window_begin(ui, STRING("Ability Select"));
    madness_ui_window_end(ui);
}
