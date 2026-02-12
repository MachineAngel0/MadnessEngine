#include "audio.h"

#include "defines.h"
#include "logger.h"



bool audio_system_init(void)
{
    //TODO:
    //just here as a place holder until i write my own audio platform independant system way later

    // https://github.com/mackron/miniaudio

    ma_result result;

    result = ma_engine_init(NULL, &engine);
    MASSERT_MSG(result == MA_SUCCESS, "ma_engine_init failed");

    //NOTE: these will play at the same time
    // ma_engine_play_sound(&engine, "..\\z_assets\\audio\\pcm1644s.wav", NULL);
    // ma_engine_play_sound(&engine, "..\\z_assets\\audio\\file_example_WAV_1MG.wav", NULL);

    return true;
}

bool audio_system_shutdown(void)
{
    ma_engine_uninit(&engine);
    return true;
}

void audio_play_sound_effects(const char* file_path)
{
    ma_engine_play_sound(&engine, file_path, NULL);
}

void audio_play_sound(const char* file_path)
{
    //audio that doesn't need sound effects
    audio_play_sound_effects(file_path);
}

void audio_stop_sound(const char* file_path)
{
    //NOTE: nothing for now
}


//TODO: pass in an arena
bool wav_file_load(const char* file_path, Sound_Data* out_sound_data)
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

    FILE* wav_file = fopen(file_path, "rb");
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
    out_sound_data->data = malloc(data_size);
    if (!out_sound_data->data)
    {
        WARN("Failed to allocate memory for data");
        fclose(wav_file);
        return false;
    }
    if (fread(out_sound_data->data, 1, data_size, wav_file) != data_size)
    {
        WARN("Failed to read data size amount into data");
        fclose(wav_file);
        return false;
    }

    out_sound_data->data_size = data_size;
    out_sound_data->samples = data_size / 2;

    fclose(wav_file);
    return true;
}


