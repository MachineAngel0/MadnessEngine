#pragma once
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
    char* data;
    u64 size;
}file_read_data;

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

bool filesystem_file_size(const char* file_path, u64* out_file_size)
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

// bool filesystem_exists(const char* path);
//
// bool filesystem_open(const char* path, file_modes mode, bool binary, file_handle* out_handle);
//
// void filesystem_close(file_handle* handle);
//
// bool filesystem_read_line(file_handle* handle, char** line_buf);
//
// bool filesystem_write_line(file_handle* handle, const char* text);
//
// bool filesystem_read(file_handle* handle, u64 data_size, void* out_data, u64* out_bytes_read);
//
// bool filesystem_read_all_bytes(file_handle* handle, u8** out_bytes, u64* out_bytes_read);
//
// bool filesystem_write(file_handle* handle, u64 data_size, const void* data, u64* out_bytes_written);
//
//
