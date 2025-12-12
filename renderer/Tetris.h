
#ifndef TETRIS_H
#define TETRIS_H
#include <cstdio>
#include <random>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

#include "clock.h"
#include "mesh.h"
#include "UI.h"
#include "vk_vertex.h"


//I need to be able to render multiple squares
//and put them at specific spots on the screen




constexpr int GRID_COLUMN = 12; //1 columns are the boundaries
constexpr int GRID_ROW = 22;  //2 rows are the boundaries
constexpr float BLOCK_SCALE = 0.041f;
constexpr float CELL_SIZE = 0.085f;
constexpr float XOFFSET = -0.4f;
constexpr float YOFFSET = -0.85f;

constexpr float SPAWN_XOFFSET = 5;
constexpr float SPAWN_YOFFSET = -2;


enum Direction
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
};

struct Position
{
    float x;
    float y;
};

//were using this as there are no decimal values in grids
struct Grid_Position
{
    int x;
    int y;
};

enum Tetromino_Type
{
    I,
    O,
    T,
    S,
    Z,
    J,
    L,
    COUNT,
};


enum COLOR: int8_t
{
    WHITE,
    BLUE_LIGHT,
    YELLOW,
    PURPLE,
    GREEN,
    RED,
    ORANGE,
    BLUE_DARK,
    GREY,
};


struct Tetris_Grid
{
    int column;
    int row;
    //tetromino type represent the color of the grid
    COLOR grid_color [GRID_ROW][GRID_COLUMN];
};


static std::unordered_map<COLOR, glm::vec3> color_look_up_table{
    {WHITE,glm::vec3{1.0, 1.0, 1.0}},
    {BLUE_LIGHT,glm::vec3{0.0, 1.0, 1.0}},
    {YELLOW, glm::vec3{1.0,1.0,0.0}},
    {PURPLE, glm::vec3{0.5,0.0,1.0}},
    {GREEN,glm::vec3{0.0,1.0,0.0}},
    {RED, glm::vec3{1.0,0.0,0.0}},
    {ORANGE,glm::vec3{1.0,0.5,0.0}},
    {BLUE_DARK,glm::vec3{0.0,0.0,1.0}},
    {GREY,glm::vec3{0.5, 0.5, 0.5}},
};

static std::unordered_map<Tetromino_Type, COLOR> TETROMINO_COLOR_LOOKUP{
    {I, BLUE_LIGHT},
    {O, YELLOW},
    {T, PURPLE},
    {S, GREEN},
    {Z, RED},
    {J, ORANGE},
    {L, BLUE_DARK},
};


inline glm::vec3 get_color(COLOR color)
{
    return color_look_up_table[color];
}
struct Tetromino
{
    //this is a vector for offsetting each group of vertices but this is very much not ideal,
    //it would be better to just store the offset into the vertices data
    std::vector<int> id;
    Tetromino_Type type;
    glm::vec3 color;
    Grid_Position grid_position; // the offset from (0,0)
    std::vector<Grid_Position> tetromino_default_position; // the position of each block relative to (0,0)
    int rotation_state = 0; //3 max

};





inline Tetris_Grid create_grid(VERTEX_DYNAMIC_INFO& vertex_info, int column, int row)
{
    Tetris_Grid new_grid{};
    new_grid.column = column;
    new_grid.row = row;

    for (int i = 0; i < GRID_ROW; i++)
    {
        for (int j = 0; j < GRID_COLUMN; j++)
        {

            float x = XOFFSET + j * CELL_SIZE;  // Start from -0.45 for 10 columns
            float y = YOFFSET + i * CELL_SIZE;   // Start from -0.9 for 20 rows

            glm::vec3 white{1.0f, 1.0f, 1.0f};
            glm::vec3 grey{0.5, 0.5, 0.5};
            glm::vec3 color = white;
            //color the edges
            if (i == 0 || i == GRID_ROW-1|| j == 0 || j == GRID_COLUMN-1)
            {
                color = grey;
                new_grid.grid_color[i][j] = GREY; //give the edge a custom color
            }
            else
            {
                new_grid.grid_color[i][j] = WHITE;
            }


            add_quad(glm::vec2{x, y}, color_look_up_table[new_grid.grid_color[i][j]],  BLOCK_SCALE, vertex_info);
        }
    }

    return new_grid;
}

inline void refresh_grid(Tetris_Grid& tetris_grid, Tetromino current_tetromino, VERTEX_DYNAMIC_INFO& vertex_info)
{
    //reset our vertex buffer data
    clear_vertex_info(vertex_info);

    COLOR Grid_Color = TETROMINO_COLOR_LOOKUP[current_tetromino.type];

    //update the grid with the position/color of the block
    for (auto tetromino_default_position : current_tetromino.tetromino_default_position)
    {
        tetris_grid.grid_color
        [tetromino_default_position.y + current_tetromino.grid_position.y][tetromino_default_position.x + current_tetromino.grid_position.x]
        = Grid_Color;
    }

    int complete = 0; // the number of rows we want to shift the blocks down, everytime we have a completed row
    //iterate the rows from the bottom up
    for (int i = GRID_ROW; i > 0; i--)
    {
        bool clear_row = true;
        if (i == 0 || i == GRID_ROW-1) continue;
        for (int j = 0; j < GRID_COLUMN; j++)
        {
            if (j == 0 || j == GRID_COLUMN-1) continue;
            if (tetris_grid.grid_color[i][j] == GREY) continue;

            if (tetris_grid.grid_color[i][j] == WHITE)
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
                if (i == 0 || i == GRID_ROW-1|| j == 0 || j == GRID_COLUMN-1) continue;
                if (tetris_grid.grid_color[i][j] == GREY) continue;

                tetris_grid.grid_color[i][j] = WHITE;

            }
        }
        else
        {
            //get the row below it and set its color to the current color
            for (int j = 0; j < GRID_COLUMN; j++)
            {
                if (i == 0 || i == GRID_ROW-1|| j == 0 || j == GRID_COLUMN-1) continue;
                if (tetris_grid.grid_color[i][j] == GREY) continue;

                tetris_grid.grid_color[i+complete][j] = tetris_grid.grid_color[i][j];
            }
        }
    }


    /*
    if (row_to_clear.contains(i))
    {
        if (tetris_grid.grid_color[i][j] != GREY)
        {
            tetris_grid.grid_color[i][j] = WHITE;
        }
    }*/

    for (int i = 0; i < GRID_ROW; i++)
    {
        for (int j = 0; j < GRID_COLUMN; j++)
        {

            float x = XOFFSET + j * CELL_SIZE;  // Start from -0.45 for 10 columns
            float y = YOFFSET + i * CELL_SIZE;   // Start from -0.9 for 20 rows

            add_quad(glm::vec2{x, y}, color_look_up_table[tetris_grid.grid_color[i][j]], BLOCK_SCALE, vertex_info);
        }
    }


}

inline std::vector<Grid_Position> L_Block(int position)
{
    if (position == 0)
    {
        return {Grid_Position(0, 0), Grid_Position(0, 1), Grid_Position(1, 1), Grid_Position(2, 1)};
    }
    if (position == 1)
    {
        return {Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(1, 2), Grid_Position(2, 0)};
    }
    if (position == 2)
    {
        return {Grid_Position(0, 1), Grid_Position(1, 1), Grid_Position(2, 1), Grid_Position(2, 2)};
    }
    if (position == 3)
    {
        return {Grid_Position(0, 2), Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(1, 2)};
    };
    return {Grid_Position(0, 0)};

}

inline std::vector<Grid_Position> J_Block(int position)
{
    if (position == 0)
    {
        return {Grid_Position(0, 1), Grid_Position(1, 1), Grid_Position(2, 0), Grid_Position(2, 1)};
    }
    if (position == 1)
    {
        return {Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(1, 2), Grid_Position(2, 2)};
    }
    if (position == 2)
    {
        return {Grid_Position(0, 1), Grid_Position(0, 2), Grid_Position(1, 1), Grid_Position(2, 1)};
    }
    if (position == 3)
    {
        return {Grid_Position(0, 0), Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(1, 2)};
    };
    return {Grid_Position(0, 0)};


}
inline std::vector<Grid_Position> O_Block()
{
    return {Grid_Position(0, 0), Grid_Position(0, 1), Grid_Position(1, 0), Grid_Position(1, 1)};
}

inline std::vector<Grid_Position> S_Block(int position)
{
    if (position == 0)
    {
        return {Grid_Position(0, 0), Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(2, 1)};
    }
    if (position == 1)
    {
        return {Grid_Position(1, 1), Grid_Position(1, 2), Grid_Position(2, 0), Grid_Position(2, 1)};
    } if (position == 2)
    {
        return {Grid_Position(0, 1), Grid_Position(1, 1), Grid_Position(1, 2), Grid_Position(2, 2)};
    };
    if (position == 3)
    {
        return {Grid_Position(0, 1), Grid_Position(0, 2), Grid_Position(1, 0), Grid_Position(1, 1)};
    }
    return {Grid_Position(0, 0)};

}


inline std::vector<Grid_Position> T_Block(int position)
{
    if (position == 0)
    {
        return {Grid_Position(0, 1), Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(2, 1)};
    }
    if (position == 1)
    {
        return {Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(1, 2), Grid_Position(2, 1)};
    }
    if (position == 2)
    {
        return{Grid_Position(0, 1), Grid_Position(1, 1), Grid_Position(1, 2), Grid_Position(2, 1)};
    }
    if (position == 3)
    {
        return {Grid_Position(0, 1), Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(1, 2)};
    };
    return {Grid_Position(0, 0)};

}
inline std::vector<Grid_Position> Z_Block(int position)
{
    if (position == 0)
    {
        return{Grid_Position(0, 2), Grid_Position(1, 1), Grid_Position(1, 2), Grid_Position(2, 1)};
    }
    if (position == 1)
    {
        return {Grid_Position(0, 0), Grid_Position(0, 1), Grid_Position(1, 1), Grid_Position(1, 2)};
    }
    if (position == 2)
    {
        return{Grid_Position(0, 1), Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(2, 0)};
    }
    if (position == 3)
    {
        return{Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(2, 1), Grid_Position(2, 2)};
    };

    return {Grid_Position(0, 0)};
}

inline std::vector<Grid_Position> I_Block(int position)
{
    if (position == 0)
    {
        return{Grid_Position(0, 1), Grid_Position(1, 1), Grid_Position(2, 1), Grid_Position(3, 1)};
    }
    if (position == 1)
    {
        return{Grid_Position(2, 0), Grid_Position(2, 1), Grid_Position(2, 2), Grid_Position(2, 3)};
    }
    if (position == 2)
    {
        return{Grid_Position(0, 2), Grid_Position(1, 2), Grid_Position(2, 2), Grid_Position(3, 2)};
    }
    if (position == 3)
    {
        return {Grid_Position(1, 0), Grid_Position(1, 1), Grid_Position(1, 2), Grid_Position(1, 3)}  ;
    };

    return {Grid_Position(0, 0)};
}


inline void spawn_block(Tetromino& Tetromino, glm::vec3 color, VERTEX_DYNAMIC_INFO& vertex_info)
{
    Tetromino.grid_position.x += SPAWN_XOFFSET; // give it a 5 offset
    Tetromino.grid_position.y += SPAWN_YOFFSET; // give it a 5 offset
    for (auto default_position : Tetromino.tetromino_default_position)
    {
        //Tetromino.id.emplace_back(add_quad(glm::vec2{XOFFSET + (SPAWN_CENTER_OFFSET + (CELL_SIZE*grid_position.x)), YOFFSET + (CELL_SIZE*grid_position.y)}, color, BLOCK_SCALE, vertex_info));
        Tetromino.id.emplace_back(add_quad(glm::vec2{XOFFSET + ((default_position.x + Tetromino.grid_position.x) * CELL_SIZE), YOFFSET + (CELL_SIZE * (default_position.y + Tetromino.grid_position.y))}, color, BLOCK_SCALE, vertex_info));
    }
}

inline Tetromino create_new_tetromino(VERTEX_DYNAMIC_INFO& vertex_info, Tetromino_Type tetromino_type)
{
    Tetromino new_block{};
    switch (tetromino_type)
    {
        case I:
            new_block.tetromino_default_position = I_Block(0);
            break;
        case O:
            new_block.tetromino_default_position = O_Block();
            break;
        case T:
            new_block.tetromino_default_position = T_Block(0);
            break;
        case S:
            new_block.tetromino_default_position = S_Block(0);
            break;
        case Z:
            new_block.tetromino_default_position = Z_Block(0);
            break;
        case J:
            new_block.tetromino_default_position = J_Block(0);
            break;
        case L:
            new_block.tetromino_default_position = L_Block(0);
            break;
        default: break;
    }

     new_block.color =  color_look_up_table[TETROMINO_COLOR_LOOKUP[tetromino_type]];
    new_block.type = tetromino_type;
    spawn_block(new_block, new_block.color, vertex_info);

    return new_block;
}


inline void tetromino_update(Tetromino& tetromino, VERTEX_DYNAMIC_INFO& vertex_info)
{

    for (int i = 0; i < tetromino.id.size(); i++)
    {

        Grid_Position position = tetromino.grid_position;
        Grid_Position default_position = tetromino.tetromino_default_position[i];

        int id = tetromino.id[i] * 4;

        vertex_info.dynamic_vertices[id].pos.x =  XOFFSET + ((position.x + default_position.x) * CELL_SIZE) - BLOCK_SCALE;
        vertex_info.dynamic_vertices[id].pos.y = YOFFSET + ((position.y+ default_position.y) * CELL_SIZE) - BLOCK_SCALE;

        vertex_info.dynamic_vertices[id+1].pos.x =  XOFFSET + ((position.x + default_position.x) * CELL_SIZE) + BLOCK_SCALE;
        vertex_info.dynamic_vertices[id+1].pos.y = YOFFSET + ((position.y+ default_position.y) * CELL_SIZE) - BLOCK_SCALE;

        vertex_info.dynamic_vertices[id+2].pos.x =  XOFFSET + ((position.x + default_position.x) * CELL_SIZE)+ BLOCK_SCALE;
        vertex_info.dynamic_vertices[id+2].pos.y = YOFFSET + ((position.y+ default_position.y) * CELL_SIZE) + BLOCK_SCALE;

        vertex_info.dynamic_vertices[id+3].pos.x =  XOFFSET + ((position.x + default_position.x) * CELL_SIZE) - BLOCK_SCALE;
        vertex_info.dynamic_vertices[id+3].pos.y = YOFFSET + ((position.y+ default_position.y) * CELL_SIZE) + BLOCK_SCALE;



        /*
        vertex_info.dynamic_vertices[id].pos.x =  XOFFSET + (position.x * CELL_SIZE) - BLOCK_SCALE;
        vertex_info.dynamic_vertices[id].pos.y = YOFFSET + (position.y * CELL_SIZE) - BLOCK_SCALE;

        vertex_info.dynamic_vertices[id+1].pos.x =  XOFFSET + (position.x * CELL_SIZE) + BLOCK_SCALE;
        vertex_info.dynamic_vertices[id+1].pos.y = YOFFSET + (position.y * CELL_SIZE) - BLOCK_SCALE;

        vertex_info.dynamic_vertices[id+2].pos.x =  XOFFSET + (position.x * CELL_SIZE) + BLOCK_SCALE;
        vertex_info.dynamic_vertices[id+2].pos.y = YOFFSET + (position.y * CELL_SIZE) + BLOCK_SCALE;

        vertex_info.dynamic_vertices[id+3].pos.x =  XOFFSET + (position.x * CELL_SIZE) - BLOCK_SCALE;
        vertex_info.dynamic_vertices[id+3].pos.y = YOFFSET + (position.y * CELL_SIZE) + BLOCK_SCALE;*/

    }

    vertex_info.vertex_buffer_should_update = true;

}

inline bool can_move(Tetris_Grid& tetris_grid, Tetromino& tetromino, glm::vec2 direction_vector)
{

    int x_grid = tetromino.grid_position.x + direction_vector.x;
    int y_grid = tetromino.grid_position.y + direction_vector.y;


    for (auto tetromino_default_position : tetromino.tetromino_default_position)
    {
        int x_check = tetromino_default_position.x + x_grid;
        int y_check = tetromino_default_position.y + y_grid;

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
        if (tetris_grid.grid_color[y_check][x_check] != WHITE && tetris_grid.grid_color[y_check][x_check] != GREY && y_check > 0 )
        {
            //printf("IS A COLORED BLOCK\n");

            return false;
        }

    }


    return true;
}

inline void rotate_block(Tetris_Grid tetris_grid, Tetromino& tetromino, VERTEX_DYNAMIC_INFO& vertex_info)
{
    //testing using Z
    int temp_rotation_state = tetromino.rotation_state + 1;
    if (temp_rotation_state > 3) temp_rotation_state = 0;

    std::vector<Grid_Position> rotation;

    switch (tetromino.type)
    {
        case I:
            rotation = I_Block(tetromino.rotation_state);
            break;
        case O:
            rotation = O_Block();
            break;
        case T:
            rotation = T_Block(tetromino.rotation_state);
            break;
        case S:
            rotation = S_Block(tetromino.rotation_state);
            break;
        case Z:
            rotation = Z_Block(tetromino.rotation_state);
            break;
        case J:
            rotation = J_Block(tetromino.rotation_state);
            break;
        case L:
            rotation = L_Block(tetromino.rotation_state);
            break;
        case COUNT:
            //INVALID
            break;
    }

    //check if we can rotate at all
    Tetromino temp_tetromino = tetromino;

    //set new positions for temp, we want to use it to check, if the rotation will collide with something
    for (int i = 0; i < temp_tetromino.tetromino_default_position.size(); i++)
    {
        temp_tetromino.tetromino_default_position[i] = rotation[i];
    }

    if (can_move(tetris_grid, temp_tetromino,{0,0}))
    {
        //if we can rotate we set tetromino to temp, as it contains the new positions
        tetromino.tetromino_default_position = temp_tetromino.tetromino_default_position;
        //set rotation number
        tetromino.rotation_state = temp_rotation_state;
    }


}



//return false means we want to spawn in a new block
inline bool move_block(Tetris_Grid& tetris_grid, Tetromino& tetromino, Direction direction, VERTEX_DYNAMIC_INFO& vertex_info)
{

    switch (direction)
    {
        case UP:
            //rotate_block function handles the can move check
            rotate_block(tetris_grid, tetromino, vertex_info);
            break;
        case DOWN:
            if (can_move(tetris_grid, tetromino, glm::vec2{0,1}))
            {
                tetromino.grid_position.y++;
            }
            else
            {
                return false;
            }
            break;
        case RIGHT:
            if (can_move(tetris_grid, tetromino, glm::vec2{1,0}))
            {
                tetromino.grid_position.x++;
            };
            break;
        case LEFT:
            if (can_move(tetris_grid, tetromino, glm::vec2{-1,0}))
            {
                tetromino.grid_position.x--;
            };
            break;
    }

    tetromino_update(tetromino, vertex_info);
    return true;
}


inline void update_grid_representation(Tetris_Grid& tetris_grid, Tetromino& current_tetromino, VERTEX_DYNAMIC_INFO& vertex_info)
{
    current_tetromino.grid_position.x = 0;
}



struct Tetris_Clock
{
    float accumulated_time; //adding time
    float move_block_trigger = SECONDS(5.0f); //when we want to move the block down // in seconds
};

inline void tetris_clock_init(Tetris_Clock& tetris_clock)
{
    tetris_clock.accumulated_time =  tetris_clock.move_block_trigger;
};
inline void tetris_clock_update(Tetris_Clock& tetris_clock, float delta_time)
{
    tetris_clock.accumulated_time -= 1.0f * delta_time;
    //printf("%f\n", tetris_clock.accumulated_time);
};
inline bool should_move_block_time_trigger(Tetris_Clock& tetris_clock)
{
    if (tetris_clock.accumulated_time <= 0)
    {
        tetris_clock.accumulated_time = tetris_clock.move_block_trigger;
        return true;
    }
    return false;
};

enum class Tetris_Game_States
{
    Start, // when we first boot up
    Play, // while the game is playing
    Game_Over, // when the player game overs, and we allow them to replay the game
};

struct Game_State
{

    Tetris_Game_States game_state = Tetris_Game_States::Start;
    Tetris_Clock tetris_clock;
    Tetris_Grid tetris_grid;
    Tetromino current_tetromino;
    float move_timer = 5.0;
    float move_trigger = 0.0;

    std::vector<Game_State*> start_game;
    std::vector<Game_State*> init_play_game; // transition state into play game
    std::vector<Game_State*> play_game;
    std::vector<Game_State*> game_over;

};





inline std::mt19937& rng() {
    static std::mt19937 gen(
        static_cast<unsigned int>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        )
    );
    return gen;
}


inline Tetromino pick_new_tetromino(VERTEX_DYNAMIC_INFO& vertex_info)
{
    //get a random tetromino type, rn its kinda bad as you'll end up getting a duplicates quite often
    std::uniform_int_distribution dist(0, Tetromino_Type::COUNT - 1);
    auto type = static_cast<Tetromino_Type>(dist(rng()));
    return create_new_tetromino(vertex_info, type);
}




inline void update_game(Game_State* game_state, VERTEX_DYNAMIC_INFO& vertex_dynamic_info, float dt)
{



    tetris_clock_update(game_state->tetris_clock, dt);
    if (should_move_block_time_trigger(game_state->tetris_clock))
    {

        bool spawn_new_block = move_block(game_state->tetris_grid, game_state->current_tetromino, DOWN, vertex_dynamic_info);

        if (!spawn_new_block)
        {
            //since a block got stopped
            //check if we lost the game
            //we lose if the current tetromino is on any part of the grey
            if (game_state->current_tetromino.grid_position.y <= 0)
            {
                //printf("you lost\n");
                //clear_vertex_info(vertex_dynamic_info); // dont clear this
                game_state->play_game.clear();
                game_state->game_over.emplace_back(game_state);
                /*
                game_state->tetris_grid = create_grid(vertex_dynamic_info, GRID_COLUMN, GRID_ROW);
                //game_state->current_block = create_block(vertex_info, O);
                game_state->current_tetromino = pick_new_tetromino(vertex_dynamic_info);
                tetris_clock_init(game_state->tetris_clock);*/
            }
            else
            {
                //printf("%d%d\n", game_state->current_tetromino.grid_position.x, game_state->current_tetromino.grid_position.y);
                refresh_grid(game_state->tetris_grid, game_state->current_tetromino, vertex_dynamic_info);

                //set new tetromino
                game_state->current_tetromino = pick_new_tetromino(vertex_dynamic_info);
            }
        }
    }
};



inline Game_State* init_game_state()
{
    Game_State* temp_game_state = static_cast<Game_State *>(malloc(sizeof(Game_State)));

    //we want to begin in start game mode
    temp_game_state->start_game.emplace_back(temp_game_state);



    return temp_game_state;
}


inline void init_play_state(Game_State* game_state, VERTEX_DYNAMIC_INFO& vertex_info)
{
    game_state->tetris_grid = create_grid(vertex_info, GRID_COLUMN, GRID_ROW);
    //game_state->current_block = create_block(vertex_info, O);
    game_state->current_tetromino = pick_new_tetromino(vertex_info);

    Tetris_Clock tetris_clock;
    game_state->tetris_clock = tetris_clock;
    tetris_clock_init(game_state->tetris_clock);

    vertex_info.vertex_buffer_should_update = true;
}



inline void game_update_DOD(Game_State* game_state, UI_STATE* ui_state, VERTEX_DYNAMIC_INFO& vertex_dynamic_info, float dt)
{

    for (auto& game : game_state->start_game)
    {
        //Our start screen
        if (do_button_new_text(ui_state, UIID{0, 0}, glm::vec2{30,40}, glm::vec2{50,25}, "Start Game", {10.0,8.0}, glm::vec3{1.0,0.0,0.0}, glm::vec3{0.0,0.0,1.0}, glm::vec3{0.0,1.0,0.0}))
        {
            game_state->init_play_game.push_back(game);
            game_state->start_game.clear();
        }


    };
    for (auto& game : game_state->init_play_game)
    {
        init_play_state(game, vertex_dynamic_info);
        game_state->play_game.push_back(game);
    }
    game_state->init_play_game.clear();

    for (auto& game : game_state->play_game)
    {
        update_game(game, vertex_dynamic_info, dt);
    }
    for (auto& game : game_state->game_over)
    {
        //Game Over
        if (do_button_new_text(ui_state, UIID{0, 0}, glm::vec2{30,40}, glm::vec2{50,25}, "Start Over", {10.0,8.0}, glm::vec3{1.0,0.0,0.0}, glm::vec3{0.0,0.0,1.0}, glm::vec3{0.0,1.0,0.0}))
        {
            game_state->init_play_game.push_back(game);
            game_state->game_over.clear();
        }
    }



}


#endif //TETRIS_H
