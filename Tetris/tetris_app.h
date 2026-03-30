#pragma once

#include "../app_types.h"

extern void tetris_dev_set_function_pointers(Tetris_Application* tetris_application);

bool tetris_game_run(Tetris_Application* tetris_application);


void main(void)
{
    Tetris_Application tetris_app;
    tetris_dev_set_function_pointers(&tetris_app);
    tetris_game_run(&tetris_app);
}
