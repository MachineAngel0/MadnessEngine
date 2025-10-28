#include <stdio.h>
#include "platform/audio.h"
#include "platform/clock.h"
#include "core/serialization/network_serialization.h"
#include "core/serialization/file_serialization.h"
#include "core/logger.h"
#include "platform/platform.h"



int main(void)
{

    log_test();
    serialization_test();

    pack_unpack_test();


    // hash_map_test();
    // hash_set_test();

    platform_state platform;

    platform_startup(&platform, "Madness Engine",
                     300, 300,
                     300, 300);

    Clock clock;
    clock_start(&clock);
    // Sound_Data data;
    // load_wav_file("", &data);
    // audio_play_square_wave_sound();

    while (true)
    {

        platform_pump_messages(&platform);

        clock_update_frame_start(&clock);


        // printf("%f s\n", clock.delta_time);
        printf("%f ms\n", clock_delta_time_in_ms(&clock));
        printf("%f fps\n", clock_delta_time_to_fps(&clock));


        clock_update_frame_end(&clock);
    }

    // internal_state* i = (internal_state *) platform.internal_state;
    // TODO: so the buffer size if pretty important that its large enough,
    // but realistically i probably want to stream it in, which is a tomorrow problem
    // audio_init_refactor_plz(i->hwnd, 4410000, 44100);

    // pretty sure theres probably a bug in the code
    // audio_play_square_wave_sound();
    // Sound_Data data;
    // load_wav_file("", &data);
    // audio_play_sound_testing(&data);


    // while (true)
    // {

    // }

    /*
        reload_dll("libMADNESSDSA.dll", "libMADNESSDSA_.dll");


        gamepad_init();

        gamepad_poll();

        log_test();

        array_test();

        //    darray_test();

        // stack_testing();

        queue_testing();

        string_test();

        linked_list_test();

        binary_tree_test();

        event_test();
        */

    return 0;
}
