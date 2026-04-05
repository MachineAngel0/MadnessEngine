#ifndef AUDIO_H
#define AUDIO_H
#include "miniaudio.h"


typedef struct Sound_Data
{
    uint32_t samples;
    uint32_t data_size;
    int16_t* data;
    // Audio_Type* audio_type; AUDIO TYPE - wav, mp3, etc..
} Sound_Data;

typedef struct Madness_Audio
{
    ma_engine engine;
}Madness_Audio;


Madness_Audio* audio_system_init(Memory_System* memory_system, Resource_System* resource_system);

bool audio_system_shutdown(Madness_Audio* madness_audio);


void audio_play_sound_effects(const char* file_path);

void audio_play_sound(const char* file_path);

void audio_stop_sound(const char* file_path);

//TODO: pass in an arena
bool wav_file_load(const char* file_path, Sound_Data* out_sound_data);


#endif //AUDIO_H
