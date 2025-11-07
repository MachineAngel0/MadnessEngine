#ifndef APP_TYPES_H
#define APP_TYPES_H
#include <stdbool.h>

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


    void (*renderer_initialize)(struct renderer* renderer_inst);
    void (*renderer_update)(struct renderer* renderer_inst);

    bool testing_switch;
    void* memory_reserve; // might move to app config
} game;



typedef struct editor {
    // The application configuration.
    app_config app_config;

    // Function pointer to game's update function.
    void (*update)(struct editor* editor_inst);
} editor;

typedef struct renderer {
    // The application configuration.
    app_config app_config;
    platform_state* plat_state;

    bool (*renderer_initialize)(struct renderer* renderer_inst);
    void (*renderer_run)(struct renderer* renderer_inst);
    void (*renderer_shutdown)(struct renderer* renderer_inst);

} renderer;

#endif