#pragma once

#include "defines.h"

// // Holds a handle to a file.
// typedef struct file_handle {
//     // Opaque handle to internal file handle.
//     void* handle;
//     bool is_valid;
// } file_handle;
//
// typedef enum file_modes {
//     FILE_MODE_READ = 0x1,
//     FILE_MODE_WRITE = 0x2
// } file_modes;


typedef struct file_read_data
{
    u8* data;
    u64 size;
} file_read_data;

void file_read_data_free(file_read_data* file_data)
{
    free(file_data->data);
}


bool filesystem_open_and_return_bytes(const char* file_path, file_read_data* file_data)
{
    //TODO: use an arena
    FILE* fptr = fopen(file_path, "rb");
    if (!fptr)
    {
        WARN("FILE OPEN AND RETURN BYTES: FAILED TO OPEN FILE %s", file_path);
        return false;
    }

    // go to the end of the file and get the size
    fseek(fptr, 0, SEEK_END);
    file_data->size = ftell(fptr);
    file_data->data = malloc(file_data->size);
    rewind(fptr); // goes to the start of the file

    fread(file_data->data, 1, file_data->size, fptr);

    fclose(fptr);

    return true;
}

bool filesystem_open_and_get_file_size(const char* file_path, u64* out_file_size)
{
    FILE* fptr = fopen(file_path, "rb");
    if (!fptr)
    {
        WARN("FILE SIZE: FAILED TO OPEN FILE %s", file_path);
        return false;
    }

    // go to the end of the file and get the size
    fseek(fptr, 0, SEEK_END);
    *out_file_size = ftell(fptr);

    fclose(fptr);

    return true;
}


MINLINE void filesystem_go_to_start(FILE* fptr)
{
    fseek(fptr, 0, SEEK_SET);
}


size_t filesystem_file_size(FILE* fptr)
{
    MASSERT(fptr);
    fseek(fptr, 0, SEEK_END);
    const size_t size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    return size;
}


bool filesystem_create_file(const char* file_path)
{
    FILE* fptr = fopen(file_path, "r");

    if (fptr)
    {
        //file exists
        fclose(fptr);
        return true;
    }

    fptr = fopen(file_path, "w");
    if (fptr)
    {
        fclose(fptr);
        return true;
    }


    return false;
}


//PLATFORM SPECIFIC CODE

typedef struct File_Handle
{
    u32 handle;
} File_Handle;

//returns a file handle, 0 if an invalid file, but should be safe since it won't crash anything
File_Handle* filesystem_register_file(const char* file_path);
void filesystem_unregister_file_by_handle(File_Handle file_handle);
void filesystem_unregister_file_by_name(const char* file_path);

bool filesystem_has_file_changed(File_Handle* file_path);


bool filesystem_does_file_exists(const char* file_path);
bool filesystem_does_directory_exists(const char* directory_path);


bool filesystem_create_directory(const char* directory_path);
bool filesystem_create_file_platform(const char* file_path);

bool filesystem_scan_directory(const char* directory_path);
bool filesystem_is_directory_empty(const char* directory_path);



//NEW API

/*
typedef struct Madness_File
{
    File_Mode file_mode;
    FILE* fptr; // might need to be void* platform_specifics;
} Madness_File;
*/


// bool filesystem_open(const char* path, file_modes mode, bool binary, Madness_File* out_file);
// bool filesystem_close(Madness_File* file);
// bool filesystem_read(Madness_File* handle, u64 data_size, void* out_data, u64* out_bytes_read);
// bool filesystem_write(Madness_File* handle, u64 data_size, void* out_data, u64* out_bytes_read);
// bool filesystem_read_all_bytes(Madness_File* handle, u8** out_bytes, u64* out_bytes_read);
