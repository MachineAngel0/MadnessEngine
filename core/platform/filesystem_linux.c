#include "filesystem.h"


#if MPLATFORM_LINUX

File_Handle* filesystem_register_file(const char* file_path)
{

}

void filesystem_unregister_file_by_handle(File_Handle file_handle)
{
}

void filesystem_unregister_file_by_name(const char* file_path)
{
}

bool filesystem_has_file_changed(File_Handle* file_path)
{
}


bool filesystem_does_file_exists(const char* file_path)
{
}

bool filesystem_does_directory_exists(const char* directory_path)
{
}


bool filesystem_create_directory(const char* directory_path)
{
}

bool filesystem_create_file_platform(const char* file_path)
{
}

bool filesystem_scan_directory(const char* directory_path)
{
}

bool filesystem_is_directory_empty(const char* directory_path)
{
}


#endif
