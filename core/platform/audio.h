#ifndef AUDIO_H
#define AUDIO_H


#include "resource_types.h"
#include "miniaudio.h"

typedef struct Audio_Handle
{
    u32 id;
    u32 generation;
}Audio_Handle;

typedef struct Sound_Data
{
    uint32_t samples;
    uint32_t data_size;
    int16_t* data;
    // Audio_Type* audio_type; AUDIO TYPE - wav, mp3, etc..
} Sound_Data;



typedef struct Madness_Audio_Global_Settings
{
    float master_audio;
    // float particle_audio; // etc
}Madness_Audio_Global_Settings;


typedef struct Madness_Audio_System
{

    ma_resource_manager_config ma_rm_config;
    ma_resource_manager ma_rm;


    ma_engine_config ma_engine_config;
    ma_engine ma_engine;

    Madness_Audio_Global_Settings global_settings;



    ma_sound test_sound;
}Madness_Audio_System;



typedef struct Madness_Audio_Playback_Config
{
    bool looping;
}Madness_Audio_Playback_Config;

typedef struct Madness_Audio{

    //configs
    bool loop;
    // Madness_Audio_Playback_Options playback_config;

    //data
    ma_sound ma_sound;
    //file
    const char* file_name;
}Madness_Audio;


Madness_Audio_System* audio_system_init(Memory_System* memory_system, Asset_System* resource_system);

bool audio_system_shutdown(void);

bool audio_load(const char* file_path, ma_sound* sound);

bool audio_play_sound(ma_sound* sound, bool loop);
bool audio_stop_sound(ma_sound* sound, bool reset_to_start);

//plays and does not stop until the audio is finished
void audio_fire_and_forget(const char* file_path);


//TODO: pass in an arena
bool wav_file_load(const char* file_path, Sound_Data* out_sound_data);


#endif //AUDIO_H
