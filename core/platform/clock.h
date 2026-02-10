#ifndef CLOCK_H
#define CLOCK_H

#include "../core/defines.h"
#include "platform.h"

typedef struct Clock
{
    f64 start_time;
    f64 time_elapsed;
    f64 last_time;
    f64 delta_time;
} Clock;


void clock_update_frame_start(Clock* clock);

void clock_update_frame_end(Clock* clock);


void clock_start(Clock* clock);

void clock_stop(Clock* clock);

MINLINE float clock_delta_time_in_ms(Clock* clock);

MINLINE float clock_delta_time_to_fps(Clock* clock);

void clock_print_info(Clock* clock);



#endif // CLOCK_H
