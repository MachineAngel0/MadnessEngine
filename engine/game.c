#include "game.h"

#include "dummy.h"
#include "dummy2.h"
#include "logger.h"
#include "yo.h"

b8 game_initialize(game* game_inst) {
    KDEBUG("game_initialize() called!");

    printdummy();
    printdummy2();
    yo();
    return TRUE;
}

b8 game_update(game* game_inst, f32 delta_time) {
    return TRUE;
}

b8 game_render(game* game_inst, f32 delta_time) {
    return TRUE;
}

void game_on_resize(game* game_inst, u32 width, u32 height) {
}


