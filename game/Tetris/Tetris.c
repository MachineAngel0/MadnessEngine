#include "Tetris.h"

#include "sprite.h"

Tetris_Game_State* tetris_init(Arena* arena, Frame_Arena* frame_arena)
{
    Tetris_Game_State* tetris_game_state = arena_alloc(arena, sizeof(Tetris_Game_State));
    tetris_game_state->tetris_state = Tetris_State_Start;
    tetris_clock_init(tetris_game_state, 5.0f, arena);
    tetris_grid_init(tetris_game_state, arena, GRID_COLUMN, GRID_ROW);

    //spawn block
    Tetromino_Type type_to_spawn = pick_new_tetromino_type();
    tetris_spawn_block(tetris_game_state, type_to_spawn);
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
            //color the edges
            if (i == 0 || i == tetris->tetris_grid->row - 1 || j == 0 || j == tetris->tetris_grid->column - 1
            )
            {
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

void tetris_update(Tetris_Game_State* tetris, float delta_time)
{
    tetris_update_clock(tetris, delta_time);
    if (tetris_has_clock_move_timer_elapsed(tetris))
    {
        bool spawn_new_block = tetris_move_block(tetris, Tetris_Direction_DOWN);

        if (!spawn_new_block)
        {
            //since a block got stopped
            //check if we lost the game
            //we lose if the current tetromino is on any part of the grey

            if (tetris->current_tetromino.grid_position.y <= 0)
            {
                //printf("you lost\n");
                //clear_vertex_info(vertex_dynamic_info); // dont clear this
                tetris->tetris_state == Tetris_State_Game_Over;
                return;
            }

            //printf("%d%d\n", game_state->current_tetromino.grid_position.x, game_state->current_tetromino.grid_position.y);
            tetris_update_grid(tetris);

            //set new tetromino
            //get a random type and spawn it in/ set the current tetromino data for it
            Tetromino_Type type_to_spawn = pick_new_tetromino_type();
            tetris_spawn_block(tetris, type_to_spawn);
        }
    }
}

void tetris_shutdown(Tetris_Game_State* tetris)
{
    //do nothing, it will deallocate when the arena clears itself
}

void tetris_generate_draw(Tetris_Game_State* tetris)
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

            vec3 sprite_color = tetris_color_look_up_table[tetris->tetris_grid->grid_color[i][j]];

            sprite_create((vec2){x, y}, (vec2){BLOCK_SCALE, BLOCK_SCALE}, sprite_color, (Texture_Handle){0},
                          SPRITE_PIPELINE_COLOR);
        }
    }

    // draw current tetromino piece that is in control by the player
    Tetromino cur_tetromino = tetris->current_tetromino;
    for (int i = 0; i < TETROMINO_SIZE; i++)
    {
        vec2 pos = {
            XOFFSET + ((cur_tetromino.tetromino_default_position[i].x + cur_tetromino.grid_position.x) * CELL_SIZE),
            YOFFSET + ((cur_tetromino.tetromino_default_position[i].y + cur_tetromino.grid_position.y) * CELL_SIZE),
        };
        vec2 size = {CELL_SIZE,CELL_SIZE};
        sprite_create(pos, size, tetris_color_look_up_table[cur_tetromino.type], (Texture_Handle){0},
                      SPRITE_PIPELINE_COLOR);
    }
}


void tetris_update_grid(Tetris_Game_State* tetris)
{
    TETRIS_COLOR Grid_Color = TETROMINO_COLOR_LOOKUP[tetris->current_tetromino.type];

    //update the grid with the position/color of the block
    for (int i = 0; i < TETROMINO_SIZE; i++)
    {
        Tetris_Grid_Position tetromino_default_position = tetris->current_tetromino.tetromino_default_position[i];
        tetris->tetris_grid->grid_color[(int)tetromino_default_position.y + (int)tetris->current_tetromino.grid_position
                .y]
            [(int)tetromino_default_position.x + (int)tetris->current_tetromino.grid_position.x]
            = Grid_Color;
    }

    int complete = 0; // the number of rows we want to shift the blocks down, everytime we have a completed row
    //iterate the rows from the bottom up
    for (int i = GRID_ROW; i > 0; i--)
    {
        bool clear_row = true;
        if (i == 0 || i == GRID_ROW - 1)
            continue;
        for (int j = 0; j < GRID_COLUMN; j++)
        {
            if (j == 0 || j == GRID_COLUMN - 1)
                continue;
            if (tetris->tetris_grid->grid_color[i][j] == GREY) continue;

            if (tetris->tetris_grid->grid_color[i][j] == WHITE)
            {
                clear_row = false;
            }
        }

        //if we clear the row we do that, otherwise, we want to move the blocks down based on the completed count
        if (clear_row)
        {
            complete += 1;
            //we want to clear the row
            for (int j = 0; j < GRID_COLUMN; j++)
            {
                if (i == 0 || i == GRID_ROW - 1 || j == 0 || j == GRID_COLUMN - 1)
                    continue;
                if (tetris->tetris_grid->grid_color[i][j] == GREY) continue;

                tetris->tetris_grid->grid_color[i][j] = WHITE;
            }
        }
        else
        {
            //get the row below it and set its color to the current color
            for (int j = 0; j < GRID_COLUMN;
                 j++
            )
            {
                if (i == 0 || i == GRID_ROW - 1 || j == 0 || j == GRID_COLUMN - 1)
                    continue;
                if (tetris->tetris_grid->grid_color[i][j] == GREY) continue;

                tetris->tetris_grid->grid_color[i + complete][j] = tetris->tetris_grid->grid_color[i][j];
            }
        }
    }
}

void tetris_update_clock(Tetris_Game_State* tetris, float delta_time)
{
    tetris->tetris_clock->accumulated_time -= 1.0f * delta_time;
}

bool tetris_has_clock_move_timer_elapsed(Tetris_Game_State* tetris)
{
    if (tetris->tetris_clock->accumulated_time <= 0)
    {
        tetris->tetris_clock->accumulated_time = tetris->tetris_clock->move_block_trigger_seconds;
        return true;
    }
    return false;
}

void tetris_spawn_block(Tetris_Game_State* tetris, Tetromino_Type tetromino_type)
{
    Tetris_Grid_Position* grid_pos = tetris->current_tetromino.tetromino_default_position;
    switch (tetromino_type)
    {
    case Tetromino_Type_I:
        I_Block(grid_pos, 0);
        break;
    case Tetromino_Type_O:
        O_Block(grid_pos);
        break;
    case Tetromino_Type_T:
        T_Block(grid_pos, 0);
        break;
    case Tetromino_Type_S:
        S_Block(grid_pos, 0);
        break;
    case Tetromino_Type_Z:
        Z_Block(grid_pos, 0);
        break;
    case Tetromino_Type_J:
        J_Block(grid_pos, 0);
        break;
    case Tetromino_Type_L:
        L_Block(grid_pos, 0);
        break;
    case Tetromino_Type_MAX:
        break;
    default: break;
    }

    tetris->current_tetromino.rotation_state = 0;
    tetris->current_tetromino.color = tetris_color_look_up_table[TETROMINO_COLOR_LOOKUP[tetromino_type]];
    tetris->current_tetromino.type = tetromino_type;

    tetris->current_tetromino.grid_position.x += SPAWN_XOFFSET; // give it a 5 offset
    tetris->current_tetromino.grid_position.y += SPAWN_YOFFSET; // give it a 5 offset
}

bool tetris_move_block(Tetris_Game_State* tetris, Tetris_Direction direction)
{
    //return false means we want to spawn in a new block
    switch (direction)
    {
    case Tetris_Direction_UP:
        tetris_rotate_block(tetris);
        break;
    case Tetris_Direction_DOWN:
        if (tetris_can_move_block(tetris, tetris->current_tetromino.grid_position,
                                  tetris->current_tetromino.tetromino_default_position,
                                  vec2{0, 1}))
        {
            tetris->current_tetromino.grid_position.y++;
        }
        else
        {
            return false;
        }
        break;
    case Tetris_Direction_RIGHT:
        if (tetris_can_move_block(tetris, tetris->current_tetromino.grid_position,
                                  tetris->current_tetromino.tetromino_default_position, vec2{1, 0}))
        {
            tetris->current_tetromino.grid_position.x++;
        }
        break;
    case Tetris_Direction_LEFT:
        if (tetris_can_move_block(tetris, tetris->current_tetromino.grid_position,
                                  tetris->current_tetromino.tetromino_default_position, vec2{-1, 0}))
        {
            tetris->current_tetromino.grid_position.x--;
        }
        else
        {
            return false;
        }
        break;
    }

    return true;
}

bool tetris_can_move_block(Tetris_Game_State* tetris, Tetris_Grid_Position tetromino_grid_position,
                           Tetris_Grid_Position* tetromino_grid_default_position, vec2 direction_vector)
{
    int x_grid = tetromino_grid_position.x + direction_vector.x;
    int y_grid = tetromino_grid_position.y + direction_vector.y;

    for (int i = 0; i < TETROMINO_SIZE; i++)
    {
        int x_check = tetromino_grid_default_position[i].x + x_grid;
        int y_check = tetromino_grid_default_position[i].y + y_grid;

        //check x direction
        if (x_check > GRID_COLUMN - 2)
        {
            //printf("NO X RIGHT\n");
            return false;
        }
        if (x_check < 1)
        {
            //printf("NO X LEFT\n");
            return false;
        }

        //check y direction
        if (y_check > GRID_ROW - 2)
        {
            //printf("NO Y BOTTOM\n");
            return false;
        }
        //check for a colored block and also if we are above the grids top row
        if (tetris->tetris_grid->grid_color[y_check][x_check] != WHITE && tetris->tetris_grid->grid_color[y_check][
                x_check] != GREY &&
            y_check > 0)
        {
            //printf("IS A COLORED BLOCK\n");

            return false;
        }
    }


    return true;
}

void tetris_rotate_block(Tetris_Game_State* tetris)
{
    //get a copy of the grid, which we want to check later to see if its a valid rotation
    Tetromino* cur_tetromino = &tetris->current_tetromino;


    int temp_rotation_state = cur_tetromino->rotation_state + 1;
    if (temp_rotation_state > 3) temp_rotation_state = 0;

    //get the new position for the check
    Tetris_Grid_Position new_tetromino_grid_position[TETROMINO_SIZE];

    switch (tetris->current_tetromino.type)
    {
    case Tetromino_Type_I:
        I_Block(new_tetromino_grid_position, cur_tetromino->rotation_state);
        break;
    case Tetromino_Type_O:
        O_Block(new_tetromino_grid_position);
        break;
    case Tetromino_Type_T:
        T_Block(new_tetromino_grid_position, cur_tetromino->rotation_state);
        break;
    case Tetromino_Type_S:
        S_Block(new_tetromino_grid_position, cur_tetromino->rotation_state);
        break;
    case Tetromino_Type_Z:
        Z_Block(new_tetromino_grid_position, cur_tetromino->rotation_state);
        break;
    case Tetromino_Type_J:
        J_Block(new_tetromino_grid_position, cur_tetromino->rotation_state);
        break;
    case Tetromino_Type_L:
        L_Block(new_tetromino_grid_position, cur_tetromino->rotation_state);
        break;
    case Tetromino_Type_MAX:
        break;
    }


    //check if we can rotate at all
    if (tetris_can_move_block(tetris, cur_tetromino->grid_position, new_tetromino_grid_position, {0, 0}))
    {
        //if we can rotate we set tetromino to temp, as it contains the new positions
        memcpy(cur_tetromino->tetromino_default_position, new_tetromino_grid_position,
               sizeof(Tetris_Grid_Position) * TETROMINO_SIZE);

        //set rotation number
        cur_tetromino->rotation_state = temp_rotation_state;
    }
}

Tetromino_Type pick_new_tetromino_type(void)
{
    //get a random tetromino type, rn its kinda bad as you'll end up getting a duplicates quite often
    srand((u64)time(NULL));
    return (Tetromino_Type)(rand() % (Tetromino_Type_MAX - 1));
}


void L_Block(Tetris_Grid_Position* default_position, int position)
{
    if (position == 0)
    {
        default_position[0] = (Tetris_Grid_Position){0, 0};
        default_position[1] = (Tetris_Grid_Position){0, 1};
        default_position[2] = (Tetris_Grid_Position){1, 1};
        default_position[3] = (Tetris_Grid_Position){2, 1};
        return;
    }
    if (position == 1)
    {
        default_position[0] = (Tetris_Grid_Position){1, 0};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){1, 2};
        default_position[3] = (Tetris_Grid_Position){2, 0};
        return;
    }
    if (position == 2)
    {
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){2, 1};
        default_position[3] = (Tetris_Grid_Position){2, 2};
        return;
    }
    if (position == 3)
    {
        default_position[0] = (Tetris_Grid_Position){0, 2};
        default_position[1] = (Tetris_Grid_Position){1, 0};
        default_position[2] = (Tetris_Grid_Position){1, 1};
        default_position[3] = (Tetris_Grid_Position){1, 2};
        return;
    };
    MASSERT(false);
}

void J_Block(Tetris_Grid_Position* default_position, int position)
{
    if (position == 0)
    {
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){2, 0};
        default_position[3] = (Tetris_Grid_Position){2, 1};
        return;
    }
    if (position == 1)
    {
        default_position[0] = (Tetris_Grid_Position){1, 0};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){1, 2};
        default_position[3] = (Tetris_Grid_Position){2, 2};
        return;
    }
    if (position == 2)
    {
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){0, 2};
        default_position[2] = (Tetris_Grid_Position){1, 1};
        default_position[3] = (Tetris_Grid_Position){2, 1};
        return;
    }
    if (position == 3)
    {
        default_position[0] = (Tetris_Grid_Position){0, 0};
        default_position[1] = (Tetris_Grid_Position){1, 0};
        default_position[2] = (Tetris_Grid_Position){1, 1};
        default_position[3] = (Tetris_Grid_Position){1, 2};
        return;
    }
    MASSERT(false);
}

void O_Block(Tetris_Grid_Position* default_position)
{
    default_position[0] = (Tetris_Grid_Position){0, 0};
    default_position[1] = (Tetris_Grid_Position){0, 1};
    default_position[2] = (Tetris_Grid_Position){1, 0};
    default_position[3] = (Tetris_Grid_Position){1, 1};
    return;
}

void S_Block(Tetris_Grid_Position* default_position, int position)
{
    if (position == 0)
    {
        default_position[0] = (Tetris_Grid_Position){0, 0};
        default_position[1] = (Tetris_Grid_Position){1, 0};
        default_position[2] = (Tetris_Grid_Position){1, 1};
        default_position[3] = (Tetris_Grid_Position){2, 1};
        return;
    }
    if (position == 1)
    {
        default_position[0] = (Tetris_Grid_Position){1, 1};
        default_position[1] = (Tetris_Grid_Position){1, 2};
        default_position[2] = (Tetris_Grid_Position){2, 0};
        default_position[3] = (Tetris_Grid_Position){2, 1};
        return;
    }
    if (position == 2)
    {
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){1, 2};
        default_position[3] = (Tetris_Grid_Position){2, 2};
        return;
    }
    if (position == 3)
    {
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){0, 2};
        default_position[2] = (Tetris_Grid_Position){1, 0};
        default_position[3] = (Tetris_Grid_Position){1, 1};
        return;
    }
    MASSERT(false);
}

void T_Block(Tetris_Grid_Position* default_position, int position)
{
    if (position == 0)
    {
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){1, 0};
        default_position[2] = (Tetris_Grid_Position){1, 1};
        default_position[3] = (Tetris_Grid_Position){2, 1};
        return;
    }
    if (position == 1)
    {
        default_position[0] = (Tetris_Grid_Position){1, 0};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){1, 2};
        default_position[3] = (Tetris_Grid_Position){2, 1};
        return;
    }
    if (position == 2)
    {
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){1, 2};
        default_position[3] = (Tetris_Grid_Position){2, 1};
        return;
    }
    if (position == 3)
    {
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){1, 0};
        default_position[2] = (Tetris_Grid_Position){1, 1};
        default_position[3] = (Tetris_Grid_Position){1, 2};
        return;
    };
    MASSERT(false);
}

void Z_Block(Tetris_Grid_Position* default_position, int position)
{
    if (position == 0)
    {
        default_position[0] = (Tetris_Grid_Position){0, 2};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){1, 2};
        default_position[3] = (Tetris_Grid_Position){2, 1};
        return;
    }
    if (position == 1)
    {
        default_position[0] = (Tetris_Grid_Position){0, 0};
        default_position[1] = (Tetris_Grid_Position){0, 1};
        default_position[2] = (Tetris_Grid_Position){1, 1};
        default_position[3] = (Tetris_Grid_Position){2, 2};
        return;
    }
    if (position == 2)
    {
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){1, 0};
        default_position[2] = (Tetris_Grid_Position){1, 1};
        default_position[3] = (Tetris_Grid_Position){2, 0};
        return;
    }
    if (position == 3)
    {
        default_position[0] = (Tetris_Grid_Position){1, 0};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){2, 1};
        default_position[3] = (Tetris_Grid_Position){2, 2};
        return;
    };

    MASSERT(false);
}

void I_Block(Tetris_Grid_Position* default_position, int position)
{
    if (position == 0)
    {
        // return Tetris_Grid_Position*{(Tetris_Grid_Position){0, 1}, (Tetris_Grid_Position){1, 1}, (Tetris_Grid_Position){2, 1}, (Tetris_Grid_Position){3, 1}};
        default_position[0] = (Tetris_Grid_Position){0, 1};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){2, 1};
        default_position[3] = (Tetris_Grid_Position){3, 1};
        return;
    }
    if (position == 1)
    {
        default_position[0] = (Tetris_Grid_Position){2, 0};
        default_position[1] = (Tetris_Grid_Position){2, 1};
        default_position[2] = (Tetris_Grid_Position){2, 2};
        default_position[3] = (Tetris_Grid_Position){2, 3};
        return;
    }
    if (position == 2)
    {
        default_position[0] = (Tetris_Grid_Position){0, 2};
        default_position[1] = (Tetris_Grid_Position){1, 2};
        default_position[2] = (Tetris_Grid_Position){2, 2};
        default_position[3] = (Tetris_Grid_Position){3, 2};
        return;
    }
    if (position == 3)
    {
        default_position[0] = (Tetris_Grid_Position){1, 0};
        default_position[1] = (Tetris_Grid_Position){1, 1};
        default_position[2] = (Tetris_Grid_Position){1, 2};
        default_position[3] = (Tetris_Grid_Position){1, 3};
        return;
    };

    MASSERT(false);
}
