#ifndef TETRIS_H
#define TETRIS_H


//I need to be able to render multiple squares
//and put them at specific spots on the screen

#define GRID_COLUMN 12 //1 columns are the boundaries
#define GRID_ROW 22  //2 rows are the boundaries
#define BLOCK_SCALE 0.041f
#define CELL_SIZE 0.085f
#define XOFFSET -0.4f
#define YOFFSET -0.85f

#define SPAWN_XOFFSET 5.0f
#define SPAWN_YOFFSET -2.0f

//the max number of pieces a tetromino can be made out of
#define TETROMINO_SIZE 4

#include <stdbool.h>
#include <time.h>

#include "arena.h"
#include "maths/math_lib.h"
#include "maths/math_types.h"


typedef enum Tetris_Direction
{
    Tetris_Direction_UP,
    Tetris_Direction_DOWN,
    Tetris_Direction_RIGHT,
    Tetris_Direction_LEFT,
} Tetris_Direction;

typedef struct Tetris_Position
{
    float x;
    float y;
} Tetris_Position;

//were using this as there are no decimal values in grids
typedef vec2 Tetris_Grid_Position;

typedef enum Tetromino_Type
{
    Tetromino_Type_I,
    Tetromino_Type_O,
    Tetromino_Type_T,
    Tetromino_Type_S,
    Tetromino_Type_Z,
    Tetromino_Type_J,
    Tetromino_Type_L,
    Tetromino_Type_MAX,
} Tetromino_Type;


typedef enum TETRIS_COLOR
{
    TETRIS_COLOR_WHITE,
    TETRIS_COLOR_BLUE_LIGHT,
    TETRIS_COLOR_YELLOW,
    TETRIS_COLOR_PURPLE,
    TETRIS_COLOR_GREEN,
    TETRIS_COLOR_RED,
    TETRIS_COLOR_ORANGE,
    TETRIS_COLOR_BLUE_DARK,
    TETRIS_COLOR_GREY,
    TETRIS_COLOR_MAX,
} TETRIS_COLOR;


typedef struct Tetris_Grid
{
    int column;
    int row;
    //tetromino type represent the color of the grid
    TETRIS_COLOR grid_color[GRID_ROW][GRID_COLUMN];
} Tetris_Grid;


vec3 tetris_color_look_up_table[TETRIS_COLOR_MAX] = {
    [TETRIS_COLOR_WHITE] = {1.0f, 1.0f, 1.0f},
    [TETRIS_COLOR_BLUE_LIGHT] = {0.0f, 1.0f, 1.0f},
    [TETRIS_COLOR_YELLOW] = {1.0f, 1.0f, 0.0f},
    [TETRIS_COLOR_PURPLE] = {0.5f, 0.0f, 1.0f},
    [TETRIS_COLOR_GREEN] = {0.0f, 1.0f, 0.0f},
    [TETRIS_COLOR_RED] = {1.0f, 0.0f, 0.0f},
    [TETRIS_COLOR_ORANGE] = {1.0f, 0.5f, 0.0f},
    [TETRIS_COLOR_BLUE_DARK] = {0.0f, 0.0f, 1.0f},
    [TETRIS_COLOR_GREY] = {0.5f, 0.5f, 0.5f},
};


TETRIS_COLOR TETROMINO_COLOR_LOOKUP[] = {
    [Tetromino_Type_I] = TETRIS_COLOR_BLUE_LIGHT,
    [Tetromino_Type_O] = TETRIS_COLOR_YELLOW,
    [Tetromino_Type_T] = TETRIS_COLOR_PURPLE,
    [Tetromino_Type_S] = TETRIS_COLOR_GREEN,
    [Tetromino_Type_Z] = TETRIS_COLOR_RED,
    [Tetromino_Type_J] = TETRIS_COLOR_ORANGE,
    [Tetromino_Type_L] = TETRIS_COLOR_BLUE_DARK,
};


typedef struct Tetromino
{
    // the offset from (0,0)
    Tetris_Grid_Position grid_position;
    // The position of each unique block relative to (0,0)
    // since each tetromino has its pieces in different shapes,
    // we need to keep track of where each piece/block starts to get the proper offset on the grid
    Tetris_Grid_Position tetromino_default_position[TETROMINO_SIZE];

    Tetromino_Type type;
    vec3 color;
    int rotation_state;
} Tetromino;


typedef struct Tetris_Clock
{
    float accumulated_time; //adding time
    float move_block_trigger_seconds; //in seconds - when we want to move the block down // in seconds
} Tetris_Clock;

typedef enum Tetris_State
{
    Tetris_State_Start, // when we first boot up
    Tetris_State_Play, // while the game is playing
    Tetris_State_Game_Over, // when the player game overs, and we allow them to replay the game
} Tetris_State;

typedef struct Tetris_Game_State
{
    Tetris_State tetris_state;
    Tetris_Clock* tetris_clock;
    Tetris_Grid* tetris_grid;
    Tetromino current_tetromino;
} Tetris_Game_State;

//NEW API
//NOTE: the sprite system handles the drawing
Tetris_Game_State* tetris_init(Arena* arena, Frame_Arena* frame_arena);

void tetris_clock_init(Tetris_Game_State* tetris, float block_move_speed_seconds, Arena* arena);
void tetris_grid_init(Tetris_Game_State* tetris, Arena* arena, int column, int row);
void tetris_shutdown(Tetris_Game_State* tetris);

void tetris_update(Tetris_Game_State* tetris, float delta_time);
void tetris_update_grid(Tetris_Game_State* tetris);
void tetris_update_clock(Tetris_Game_State* tetris, float delta_time);

void tetris_generate_draw(Tetris_Game_State* tetris);

Tetromino_Type pick_new_tetromino_type(void);
bool tetris_has_clock_move_timer_elapsed(Tetris_Game_State* tetris);
void tetris_spawn_block(Tetris_Game_State* tetris, Tetromino_Type tetromino_type);
bool tetris_move_block(Tetris_Game_State* tetris, Tetris_Direction direction);
bool tetris_can_move_block(Tetris_Game_State* tetris, Tetris_Grid_Position tetromino_grid_position,
                           Tetris_Grid_Position* tetromino_grid_default_position, vec2 direction_vector);
void tetris_rotate_block(Tetris_Game_State* tetris);


void L_Block(Tetris_Grid_Position* default_position, int position);
void J_Block(Tetris_Grid_Position* default_position, int position);
void O_Block(Tetris_Grid_Position* default_position);
void S_Block(Tetris_Grid_Position* default_position, int position);
void T_Block(Tetris_Grid_Position* default_position, int position);
void Z_Block(Tetris_Grid_Position* default_position, int position);
void I_Block(Tetris_Grid_Position* default_position, int position);


#endif //TETRIS_H
