#include "../core/platform/clock.h"

void clock_update_frame_start(Clock* clock)
{
    if (clock->start_time != 0)
    {
        clock->time_elapsed = platform_get_absolute_time() - clock->start_time;
        clock->delta_time = clock->time_elapsed - clock->last_time;
    }
}

void clock_update_frame_end(Clock* clock)
{
    f64 frame_end_time = platform_get_absolute_time() - clock->start_time;
    // f64 remaining_time = clock->time_elapsed - frame_end_time;
    f64 remaining_time = frame_end_time - clock->time_elapsed;
    DEBUG("FRAME END TIME %f, REMAINING TIME %f, delta time: %f", frame_end_time, remaining_time, clock->delta_time)

    //todo: either have this set manually or get the screen refresh rate
        f64 frame_time = 1.0f / 120.0f; // just testing 60fps for now
    if (remaining_time <= frame_time)
    {
        // printf("sleeping\n");
        u64 sleep_ms = (u64)(1000.f * remaining_time);
        if (sleep_ms > 0)
        {
            platform_sleep(sleep_ms);
        }
        // else
        // {
        // printf("sleep but no sleep\n");
        // }
        //NOTE: this also helps but i kinda dont like it here, i dont want to just be stalling for no reasom
        // while ((platform_get_absolute_time() - clock->start_time - clock->time_elapsed) < frame_time) {}
    }
    // else
    // {
    // printf("no sleep\n");
    // }

    // if (remaining_time > 0)
    // {
    //     u64 remaining_ms = (remaining_time* 1000);
    //     if (cap_frames)
    //     {
    //         platform_sleep(remaining_ms - 1);
    //     }
    // }

    clock->last_time = clock->time_elapsed;
}


void clock_start(Clock* clock)
{
    clock->start_time = platform_get_absolute_time();
    clock->time_elapsed = 0;
}

void clock_stop(Clock* clock)
{
    clock->start_time = 0;
}

MINLINE float clock_delta_time_in_ms(Clock* clock)
{
    return clock->delta_time * 1000.0f;
}

MINLINE float clock_delta_time_to_fps(Clock* clock)
{
    return 1.0f / clock->delta_time;
}

void clock_print_info(Clock* clock)
{
    printf("%f s\n", clock->delta_time);
    printf("%f ms\n", clock_delta_time_in_ms(clock));
    printf("%f fps\n", clock_delta_time_to_fps(clock));
}
