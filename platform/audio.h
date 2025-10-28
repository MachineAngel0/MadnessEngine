#ifndef AUDIO_H
#define AUDIO_H

#include "../defines.h"
#include "../logger.h"
#include <dsound.h>
#include <stdbool.h>
#include <math.h>

#include "../mmath.h"


#if MPLATFORM_WINDOWS
#endif // MPLATFORM_WINDOWS
#if MPLATFORM_LINUX
#endif // MPLATFORM_LINUX

//when done move into the platform layer, leave this as the higher level api


typedef HRESULT WINAPI DSoundCreate(LPCGUID, LPDIRECTSOUND*, LPUNKNOWN);

#define DirectSoundCreate DSoundCreate

// DirectSound globals
LPDIRECTSOUND dsound = NULL;
LPDIRECTSOUNDBUFFER primary_buffer = NULL;
LPDIRECTSOUNDBUFFER secondary_buffer = NULL;


typedef struct sound_data
{
    uint32_t samples;
    int16_t* data;
} Sound_Data;



//IT WORKS
//TODO: REFACTOR INTO PLATFORM LAYER
bool audio_init_refactor_plz(HWND window, int32_t buffer_size, int32_t samples_per_second)
{
    WAVEFORMATEX wformat = {0};
    ZeroMemory(&wformat, sizeof(wformat));
    wformat.wFormatTag = WAVE_FORMAT_PCM;
    wformat.nChannels = 2;
    wformat.nSamplesPerSec = samples_per_second;
    wformat.wBitsPerSample = 16;
    wformat.nBlockAlign = (wformat.nChannels * wformat.wBitsPerSample) / 8;
    wformat.nAvgBytesPerSec = wformat.nSamplesPerSec * wformat.nBlockAlign;
    wformat.cbSize = wformat.nChannels * 2;


    HMODULE DirectSoundLib = LoadLibrary("dsound.dll");
    if (!DirectSoundLib)
    {
        WARN("Failed to load dsound.dll");
        return false;
    }
    DirectSoundCreate* dsound_func = (DSoundCreate *) GetProcAddress(DirectSoundLib, "DirectSoundCreate");

    // LPDIRECTSOUND dsound;
    if (!SUCCEEDED(dsound_func(0, &dsound, 0)))
    {
        WARN("Failed to init Direct Sound");
        return false;
    }
    if (!SUCCEEDED(IDirectSound_SetCooperativeLevel(dsound, window, DSSCL_PRIORITY)))
    {
        WARN("Failed to set cooperative level");
        return false;
    };

    DSBUFFERDESC buffer_desc = {0};
    ZeroMemory(&buffer_desc, sizeof(buffer_desc));
    buffer_desc.dwSize = sizeof(buffer_desc);
    buffer_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    // LPDIRECTSOUNDBUFFER primary_buffer = NULL;
    if (!SUCCEEDED(IDirectSound_CreateSoundBuffer(dsound, &buffer_desc, &primary_buffer, 0)))
    {
        WARN("FAILED TO CREATE PRIMARY BUFFER");
        return false;
    }
    //the macro is just shortening the lpvtbl part
    // dsound->lpVtbl->CreateSoundBuffer(dsound, &buffer_desc, &primary_buffer, 0);


    buffer_desc.dwBufferBytes = buffer_size;
    IDirectSoundBuffer_SetFormat(primary_buffer, &wformat);

    DSBUFFERDESC buffer_desc2 = {0};
    buffer_desc2.dwSize = sizeof(buffer_desc);
    buffer_desc2.dwFlags = 0;
    buffer_desc2.dwBufferBytes = buffer_size;
    buffer_desc2.lpwfxFormat = &wformat;

    // LPDIRECTSOUNDBUFFER secondary_buffer = NULL;
    if (!SUCCEEDED(IDirectSound_CreateSoundBuffer(dsound, &buffer_desc2, &secondary_buffer, 0)))
    {
        WARN("FAILED TO CREATE SECONDARY BUFFER");
        return false;
    };
    // secondary_buffer->lpVtbl->SetFormat(secondary_buffer, &wformat);
    IDirectSoundBuffer_SetFormat(secondary_buffer, &wformat);

    return true;
}

bool audio_play_square_wave_sound()
{
    //FILL IN SQUARE WAVE
    VOID* region1;
    DWORD region1_size;
    VOID* region2;
    DWORD region2_size;

    if (!SUCCEEDED(
        IDirectSoundBuffer_Lock(secondary_buffer, 0, 0, &region1, &region1_size, &region2, &region2_size,
            DSBLOCK_ENTIREBUFFER)))
    {
        WARN("Lock failed");
        return false;
    }

    int16_t* buffer = (int16_t *) region1;
    int16_t* buffer2 = (int16_t *) region2;

    int SAMPLES_PER_SECOND = 48000;

    int samples = region1_size / 4; // 4 bytes per stereo frame (16-bit * 2)
    int samples2 = region2_size / 4;

    int square_period = SAMPLES_PER_SECOND / 440; // 440 Hz tone

    for (int i = 0; i < samples; ++i)
    {
        int16_t sample = ((i / (square_period / 2)) % 2) ? 3000 : -3000;
        buffer[i * 2] = sample; // left
        buffer[i * 2 + 1] = sample; // right
    }

    if (region2)
    {
        for (int i = 0; i < samples2; ++i)
        {
            int16_t sample = ((i / (square_period / 2)) % 2) ? 3000 : -3000;
            buffer2[i * 2] = sample; // left
            buffer2[i * 2 + 1] = sample; // right
        }
    }

    IDirectSoundBuffer_Unlock(secondary_buffer, region1, region1_size, region2, region2_size);


    //plays the sound
    IDirectSoundBuffer_SetCurrentPosition(secondary_buffer, 0);
    IDirectSoundBuffer_Play(secondary_buffer, 0, 0, DSBPLAY_LOOPING);

    return true;
}



bool load_wav_file(const char* file_path, Sound_Data* sound_data)
{
    // [Master RIFF chunk]
    //    FileTypeBlocID  (4 bytes) : Identifier « RIFF »  (0x52, 0x49, 0x46, 0x46)
    //    FileSize        (4 bytes) : Overall file size minus 8 bytes
    //    FileFormatID    (4 bytes) : Format = « WAVE »  (0x57, 0x41, 0x56, 0x45)
    //
    // [Chunk describing the data format]
    //    FormatBlocID    (4 bytes) : Identifier « fmt␣(null value here) »  (0x66, 0x6D, 0x74, 0x20)
    //    BlocSize        (4 bytes) : Chunk size minus 8 bytes, which is 16 bytes here  (0x10)
    //    AudioFormat     (2 bytes) : Audio format (1: PCM integer, 3: IEEE 754 float)
    //                              0x0001 	WAVE_FORMAT_PCM 	PCM
    //                              0x0003 	WAVE_FORMAT_IEEE_FLOAT 	IEEE float
    //                              0x0006 	WAVE_FORMAT_ALAW 	8-bit ITU-T G.711 A-law
    //                              0x0007 	WAVE_FORMAT_MULAW 	8-bit ITU-T G.711 µ-law
    //                              0xFFFE 	WAVE_FORMAT_EXTENSIBLE 	Determined by SubFormat

    //    NbrChannels     (2 bytes) : Number of channels
    //    Frequency       (4 bytes) : Sample rate (in hertz)
    //    BytePerSec      (4 bytes) : Number of bytes to read per second (Frequency * BytePerBloc).
    //    BytePerBloc     (2 bytes) : Number of bytes per block (NbrChannels * BitsPerSample / 8).
    //    BitsPerSample   (2 bytes) : Number of bits per sample
    //
    // [Chunk containing the sampled data]
    //    DataBlocID      (4 bytes) : Identifier « data »  (0x64, 0x61, 0x74, 0x61)
    //    DataSize        (4 bytes) : SampledData size
    //    SampledData


    char wav_file_id[4]; // FileTypeBlocID
    char wav_size[4]; // FileSize
    char wav_id[4]; // FileFormatID


    char fmt[4]; // FormatBlocID
    uint32_t format_size; // BlocSize
    uint16_t audio_format;
    uint16_t nbr_channels;
    uint32_t frequency;
    uint32_t bytes_per_sec;
    uint16_t bytes_per_bloc;
    uint16_t bits_per_sample;

    char data_id[4]; // DataBlocID
    uint32_t data_size; // DataSize
    //we pass out the actual data

    //TODO: test file, replace with file path when done
    // FILE* wav_file = fopen(file_path, 'rb');
    FILE* wav_file = fopen("../platform/file_example_WAV_1MG.wav", "rb");
    if (!wav_file)
    {
        WARN("Failed to open wave file");
        fclose(wav_file);
        return false;
    }

    fread(wav_file_id, 1, 4, wav_file);
    INFO("WAV ID: %c, %c, %c, %c", wav_file_id[0], wav_file_id[1], wav_file_id[2], wav_file_id[3]);
    if (wav_file_id[0] != 'R' || wav_file_id[1] != 'I' || wav_file_id[2] != 'F' || wav_file_id[3] != 'F')
    {
        WARN("INVALID .WAV CK ID");
        fclose(wav_file);
        return false;
    }

    fread(wav_size, 4, 1, wav_file);

    fread(wav_id, 1, 4, wav_file);
    INFO("WAV ID: %c, %c, %c, %c", wav_id[0], wav_id[1], wav_id[2], wav_id[3]);
    if (wav_id[0] != 'W' || wav_id[1] != 'A' || wav_id[2] != 'V' || wav_id[3] != 'E')
    {
        WARN("INVALID .WAV ID");
        fclose(wav_file);
        return false;
    }


    fread(fmt, 1, 4, wav_file);
    INFO("FMT: %c, %c, %c", fmt[0], fmt[1], fmt[2]);
    if (fmt[0] != 'f' || fmt[1] != 'm' || fmt[2] != 't')
    {
        WARN("INVALID FMT ID");
        fclose(wav_file);
        return false;
    }

    fread(&format_size, 4, 1, wav_file);
    fread(&audio_format, 2, 1, wav_file);
    INFO("Format size: %d", audio_format);
    //for now we are only checking for pcm wav files
    if (audio_format != 1)
    {
        WARN("INVALID AUDIO FORMAT ID, not PCM");
        fclose(wav_file);
        return false;
    }

    fread(&nbr_channels, 2, 1, wav_file);
    fread(&frequency, 4, 1, wav_file);
    fread(&bytes_per_sec, 4, 1, wav_file);
    fread(&bytes_per_bloc, 2, 1, wav_file);
    fread(&bits_per_sample, 2, 1, wav_file);
    INFO("bytes_per_sec: %d", bytes_per_sec);


    fread(data_id, 1, 4, wav_file);
    INFO("DATA ID: %c, %c, %c, %c", data_id[0], data_id[1], data_id[2], data_id[3]);
    if (data_id[0] != 'd' || data_id[1] != 'a' || data_id[2] != 't' || data_id[3] != 'a')
    {
        WARN("INVALID DATA ID");
        fclose(wav_file);
        return false;
    }
    fread(&data_size, 4, 1, wav_file);
    INFO("DATA SIZE: %d", data_size);

    //TODO: ideally would want to return this data or fill inside a structure
    //TODO: replace with allocator
    // read in the data
    sound_data->data = malloc(data_size);
    if (!sound_data->data)
    {
        WARN("Failed to allocate memory for data");
        fclose(wav_file);
        return false;
    }
    if (fread(sound_data->data, 1, data_size, wav_file) != data_size)
    {
        WARN("Failed to read data size amount into data");
        fclose(wav_file);
        return false;
    }

    sound_data->samples = data_size/2;

    fclose(wav_file);
    return true;
}

bool audio_play_sound_testing(Sound_Data* sound_data)
{

    //FILL IN SQUARE WAVE
    VOID* region1;
    DWORD region1_size;
    VOID* region2;
    DWORD region2_size;

    if (!SUCCEEDED(
        IDirectSoundBuffer_Lock(secondary_buffer, 0, 0, &region1, &region1_size, &region2, &region2_size,
            DSBLOCK_ENTIREBUFFER)))
    {
        WARN("Lock failed");
        return false;
    }


    // so there is the possiblity that the sample si larger than the
    DWORD write_size1 = min_i(region1_size, sound_data->samples);
    DWORD write_size2 = min_i(region2_size, sound_data->samples);
    memcpy(region1, sound_data->data, write_size1);
    memcpy(region2, sound_data->data, write_size2);

    IDirectSoundBuffer_Unlock(secondary_buffer, region1, region1_size, region2, region2_size);


    //plays the sound
    IDirectSoundBuffer_SetCurrentPosition(secondary_buffer, 0);
    IDirectSoundBuffer_Play(secondary_buffer, 0, 0, 0);

    return true;
}




//this would be the API ideally
void audio_initialize();

void audio_shutdown();

void audio_play_sound();

void audio_stop_sound();


#endif //AUDIO_H
