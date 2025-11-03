#ifndef APP_TYPES_H
#define APP_TYPES_H



typedef struct app_config
{
    // Window starting position x axis, if applicable.
    i16 start_pos_x;

    // Window starting position y axis, if applicable.
    i16 start_pos_y;

    // Window starting width, if applicable.
    i16 start_width;

    // Window starting height, if applicable.
    i16 start_height;

    // The application name used in windowing, if applicable.
    char* name;
}app_config;


typedef struct game {
    // The application configuration.
    app_config app_config;

    // Function pointer to game's update function.
    void (*initialize)(struct game* game_inst);
    void (*update)(struct game* game_inst);


    void (*renderer_initialize)();
    void (*renderer_update)();

    bool testing_switch;
} game;



typedef struct editor_fake {
    // The application configuration.
    app_config app_config;

    // Function pointer to game's update function.
    void (*update)(struct editor_fake* game_inst);
} editor_fake;

#endif