#ifndef PLATFORM_H
#define PLATFORM_H


#include "../core/defines.h"

typedef struct platform_state
{
    void* internal_state;
} platform_state;


MAPI b8 platform_startup(
    platform_state* plat_state,
    const char* application_name,
    i32 x, i32 y,
    i32 width, i32 height);


MAPI void platform_shutdown(platform_state* plat_state);


MAPI b8 platform_pump_messages(platform_state* plat_state);


//AUDIO
bool platform_audio_init(platform_state* plat_state, int32_t buffer_size, int32_t samples_per_second);
bool platform_audio_shutdown(platform_state* plat_state);


void* platform_allocate(u64 size, b8 aligned);


void platform_free(void* block, b8 aligned);


void* platform_zero_memory(void* block, u64 size);


void* platform_copy_memory(void* dest, const void* source, u64 size);


void* platform_set_memory(void* dest, i32 value, u64 size);

f64 platform_get_absolute_time();


// Sleep on the thread for the provided ms. This blocks the main thread.
// Should only be used for giving time back to the OS for unused update power.
// Therefore it is not exported.
void platform_sleep(u64 ms);

#endif //PLATFORM_H
