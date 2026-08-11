#include "filesystem.h"

#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "platform.h"


Asset_List_Scan* asset_lists_generate(Memory_System* memory_system, u32 max_asset_count,
                                      const char* relative_asset_path)
{
    Asset_List_Scan* asset_list = memory_system_alloc(memory_system,
                                                      sizeof(Asset_List_Scan),
                                                      MEMORY_SUBSYSTEM_RESOURCE);
    asset_list->allocator = memory_system_allocator_create(memory_system,
                                                           (sizeof(String) * max_asset_count) + (256 /*string count*/
                                                               * max_asset_count),
                                                           MEMORY_SUBSYSTEM_RESOURCE);
    asset_list->strings = allocator_alloc(asset_list->allocator,
                                          sizeof(String) * max_asset_count);
    asset_list->max_count = max_asset_count;

    filesystem_get_assets_from_directory(relative_asset_path, asset_list);

    return asset_list;
}

bool asset_lists_free(Asset_List_Scan* asset_list_scan, Memory_System* memory_system)
{
    memory_system_allocator_free(memory_system, asset_list_scan->allocator, MEMORY_SUBSYSTEM_RESOURCE);
    memory_system_memory_free(memory_system, asset_list_scan, MEMORY_SUBSYSTEM_RESOURCE);
}


File_Watch_Handle filesystem_register_file(const char* file_path)
{
    return platform_register_file_watch(file_path);
}

bool filesystem_has_file_changed(File_Watch_Handle file_handle)
{
    return platform_has_filed_changed(file_handle);
}

bool filesystem_has_directory_changed(File_Watch_Handle file_handle)
{
    return platform_has_directory_changed(file_handle);
}

///////////////// FILE WATCHER /////////////////


bool filewatcher_init(Memory_System* memory_system)
{
    madness_file_watcher = memory_system_alloc(memory_system, sizeof(Madness_File_Watcher), MEMORY_SUBSYSTEM_MISC);
    madness_file_watcher->allocator = memory_system_allocator_create(memory_system, KB(64), MEMORY_SUBSYSTEM_MISC);

    MASSERT(madness_file_watcher);
    return true;
}

bool filewatcher_update(void)
{
    MASSERT(madness_file_watcher);
    for (int i = 0; i < madness_file_watcher->directory_handle_count; ++i)
    {
        platform_update_directory(madness_file_watcher->directory_handles[i]);

        File_Watch_Event event;

        while (platform_poll_directory_changes(
            madness_file_watcher->directory_handles[i],
            &event))
        {
            // Process event
            INFO("FILE EVENT: %s %d", event.path, event.action)
            //TODO: send out an event
        }
    }



    return true;

}



bool filewatcher_deinit(void)
{
    MASSERT(madness_file_watcher);
    return true;
}


void filewatcher_directory_register(const char* directory_path)
{
    MASSERT(madness_file_watcher);

    File_Watch_Handle* handle = &madness_file_watcher->directory_handles[madness_file_watcher->directory_handle_count++];
    *handle = platform_register_directory_watch(directory_path);

}

void filewatcher_directory_unregister(const char* directory_path)
{
    //TODO:
    MASSERT_FALSE();
}


bool filewatcher_poll(Madness_File_Watcher* watcher, File_Watch_Event event)
{

}
