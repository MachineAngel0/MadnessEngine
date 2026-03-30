#ifndef MADNESS_PULSE_GAME_H
#define MADDNESS_PULSE_GAME_H

#include "../app_types.h"


// Externally-defined function to create a game. make sure that the c compiler finds this function somewhere
extern void madness_pulse_create_fpn(struct Madness_Pulse_Application* madness_pulse_app);

int main()
{
    Madness_Pulse_Application* madness_pulse_app;
    madness_pulse_create_fpn(&madness_pulse_app);
    madness_pulse_run(&madness_pulse_app);

    return 0;
}




#endif //MADNESS_PULSE_GAME_H