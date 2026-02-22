#include "Tetris.h"

Tetris_Game_State* tetris_init(Arena* arena, Frame_Arena* frame_arena)
{
    Tetris_Game_State* tetris_game_state = arena_alloc(arena, sizeof(Tetris_Game_State));
    tetris_game_state->tetris_state = Tetris_State_Start;
    tetris_clock_init(tetris_game_state, MS_TO_SEC(5), arena);
    tetris_grid_init(tetris_game_state, arena, GRID_COLUMN, GRID_ROW);
    //spawn block???????
}

void tetris_clock_init(Tetris_Game_State* tetris, float block_move_speed_seconds, Arena* arena)
{
    tetris->tetris_clock = arena_alloc(arena, sizeof(Tetris_Clock));
    tetris->tetris_clock->accumulated_time = block_move_speed_seconds;
    tetris->tetris_clock->move_block_trigger_seconds = block_move_speed_seconds;
}

void tetris_grid_init(Tetris_Game_State* tetris, Arena* arena, int column, int row)
{
    tetris->tetris_grid = arena_alloc(arena, sizeof(Tetris_Grid));
    tetris->tetris_grid->column = column;
    tetris->tetris_grid->row = row;

    for (int i = 0; i < tetris->tetris_grid->row; i++)
    {
        for (int j = 0; j < tetris->tetris_grid->column; j++)
        {
            float x = XOFFSET
                + j * CELL_SIZE; // Start from -0.45 for 10 columns
            float y = YOFFSET
                + i * CELL_SIZE; // Start from -0.9 for 20 rows

            vec3 white{1.0f, 1.0f, 1.0f};
            vec3 grey{0.5, 0.5, 0.5};
            vec3 color = white;
            //color the edges
            if (i == 0 || i == tetris->tetris_grid->row - 1 || j == 0 || j == tetris->tetris_grid->column - 1
            )
            {
                color = grey;
                tetris->tetris_grid->grid_color[i][j] = GREY; //give the edge a custom color
            }
            else
            {
                //if not an edge, just color it white
                tetris->tetris_grid->grid_color[i][j] = WHITE;
            }
        }
    }
}

void tetris_update(Tetris_Game_State* tetris)
{
}

void tetris_shutdown(Tetris_Game_State* tetris)
{
}

void tetris_upload_draw(Tetris_Game_State* tetris)
{
    // we only need to generate the draw data for the grid layout, grid with the pieces and for the current tetromino
    for (int i = 0; i < tetris->tetris_grid->row; i++)
    {
        for (int j = 0; j < tetris->tetris_grid->column; j++)
        {
            float x = XOFFSET
                + j * CELL_SIZE; // Start from -0.45 for 10 columns
            float y = YOFFSET
                + i * CELL_SIZE; // Start from -0.9 for 20 rows

            vec3 white{1.0f, 1.0f, 1.0f};
            vec3 grey{0.5, 0.5, 0.5};
            vec3 color = white;
            //color the edges
            if (i == 0 || i == tetris->tetris_grid->row - 1 || j == 0 || j == tetris->tetris_grid->column - 1
            )
            {
                color = grey;
                tetris->tetris_grid->grid_color[i][j] = GREY; //give the edge a custom color
            }
            else
            {
                //if not an edge, just color it white
                tetris->tetris_grid->grid_color[i][j] = WHITE;
            }
        }
    }


}

void tetris_update_grid(Tetris_Game_State* tetris)
{
}

void tetris_update_clock(Tetris_Game_State* tetris, float delta_time)
{
    tetris->tetris_clock->accumulated_time -= 1.0f * delta_time;
}

void tetris_spawn_block()
{
}

void tetris_move_block()
{
}

void tetris_can_block()
{
}

void tetris_rotate_block()
{
}
