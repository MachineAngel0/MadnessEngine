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

    //TODO:
    //u64* application_memory_requirement;

}app_config;


typedef struct application_base
{
    app_config app_config;
    platform_state* plat_state;
}application_base;







//TODO: everything below is old and should be replace


typedef struct game_app {
    // The application configuration.
    app_config app_config;
    platform_state* plat_state;

    // Function pointer to game's update function.
    void (*initialize)(struct game_app* game_inst);
    void (*update)(struct game_app* game_inst);


    void (*renderer_initialize)(struct renderer_app* renderer_inst);
    void (*renderer_update)(struct renderer_app* renderer_inst);
    void (*renderer_shutdown)(struct renderer_app* renderer_inst);

    bool testing_switch;
    void* memory_reserve; // might move to app config
} game_app;



typedef struct editor_app {
    // The application configuration.
    app_config app_config;
    platform_state* plat_state;

    // Function pointer to game's update function.
    void (*update)(struct editor_app* editor_inst);
} editor_app;



typedef struct renderer_app {
    // The application configuration.
    app_config app_config;
    platform_state* plat_state;

    bool (*renderer_initialize)(struct renderer_app* renderer_inst);
    void (*renderer_run)(struct renderer_app* renderer_inst, Clock* clock);
    void (*renderer_shutdown)(struct renderer_app* renderer_inst);

    void (*on_resize)(struct renderer_app* renderer_inst, u32 width, u32 height);

} renderer_app;

#endif