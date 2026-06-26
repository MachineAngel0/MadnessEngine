#ifndef OVERFLOW_HANDLER_H
#define OVERFLOW_HANDLER_H

#include "game_structs.h"


Overflow_Handler* overflow_handler_init(Madness_Pulse_Game* game)
{
    Overflow_Handler* overflow_handler = allocator_alloc(&game->allocator, sizeof(overflow_handler));
    return overflow_handler;
}


void overflow_handler_use(Overflow_Handler* overflow_handler)
{

}




#endif //OVERFLOW_HANDLER_H
