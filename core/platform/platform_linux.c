#include "platform.h"
#include "input.h"

// Linux platform layer.
#if MPLATFORM_LINUX


#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <sys/time.h>
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>  // nanosleep
#else
#include <unistd.h>  // usleep
#endif

#include <sys/random.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "darray.h"
#include "input.h"
#include "event.h"
#include "c_string.h"
#include "logger.h"


#include <sys/sendfile.h>
#include <dlfcn.h>

// #include "platform/platform_linux_x11.c"
// #include "platform/platform_linux_wayland.c"

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

typedef enum Linux_Backend
{
    LINUX_BACKEND_X11,
    LINUX_BACKEND_WAYLAND
} Linux_Backend;

typedef struct Linux_Internal_State
{
    //if i decide to use my own backends
    // void* backend_internal_state;
    Linux_Backend backend;
    GLFWwindow* main_window;

    GLFWwindow* window[MAX_MADNESS_WINDOW_COUNT];
    bool in_use[MAX_MADNESS_WINDOW_COUNT];
    u8 count;
} Linux_Internal_State;


void glfw_frambuffer_window_resize(GLFWwindow* window, int width, int height)
{
    Linux_Internal_State* state = (Linux_Internal_State*)glfwGetWindowUserPointer(window);

    // Fire the event. The application layer should pick this up, but not handle it
    // as it shouldn't be visible to other parts of the application.
    Event_Data context;
    context.data.event_data_window_resize.width = (u16)width;
    context.data.event_data_window_resize.height = (u16)height;
    event_fire(EVENT_APP_RESIZE, STRING("glfw linux platform"), context);
}

void glfw_window_resize(GLFWwindow* window, int width, int height)
{
    Linux_Internal_State* state = (Linux_Internal_State*)glfwGetWindowUserPointer(window);

    // Fire the event. The application layer should pick this up, but not handle it
    // as it shouldn't be visible to other parts of the application.
    Event_Data context;
    context.data.event_data_window_resize.width = (u16)width;
    context.data.event_data_window_resize.height = (u16)height;
    event_fire(EVENT_APP_RESIZE, STRING("glfw linux platform"), context);
}

void glfw_window_close(GLFWwindow* window)
{
    Event_Data context = {0};
    event_fire(EVENT_APP_QUIT, STRING("glfw linux platform"), context);
}

bool platform_startup(
    Platform_State* plat_state,
    Platform_Window_Config platform_config)
{
    // Create the internal state.
    plat_state->internal_state = malloc(sizeof(Linux_Internal_State));
    memset(plat_state->internal_state, 0, (sizeof(Linux_Internal_State)));
    Linux_Internal_State* state = (Linux_Internal_State*)plat_state->internal_state;

    //get the backend type
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session && strcmp(session, "wayland") == 0)
    {
        INFO("USING WAYLAND")
        state->backend = LINUX_BACKEND_WAYLAND;
    }
    else if (session && strcmp(session, "x11") == 0)
    {
        INFO("USING x11")
        state->backend = LINUX_BACKEND_X11;
    }
    else
    {
        MASSERT_MSG_FALSE("UNDEFINED LINUX BACKEND TYPE");
        return false;
    }

    //init glfw
    if (!glfwInit())
    {
        MASSERT_MSG_FALSE("LINUX BACKEND: glfwInit failed");
        return false;
    }

    if (!glfwVulkanSupported())
    {
        MASSERT_MSG_FALSE("LINUX BACKEND: Vulkan loader not found");
        return false;
    }

    // default window behaviours
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    // create hidden, show after setup (no white flash)
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    // only matters for fullscreen
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
    // not always-on-top
    glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

    // HiDPI
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE); // window size scales with the monitor's content scale
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE); // framebuffer is native resolution, not upscaled

    // identity: lets the desktop match your .desktop file / icon / taskbar grouping
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "madness_engine");
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "madness_engine");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "madness_engine");

    //position
    glfwWindowHint(GLFW_POSITION_X, platform_config.start_pos_x);
    glfwWindowHint(GLFW_POSITION_Y, platform_config.start_pos_y);

    // no GL context, Vulkan owns the surface
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    state->main_window = glfwCreateWindow(platform_config.start_width, platform_config.start_height,
                                          platform_config.name,
                                          NULL,
                                          NULL);

    if (!state->main_window)
    {
        MASSERT_MSG_FALSE("LINUX BACKEND: GLFW window creation failed");
        return false;
    }


    // after glfwCreateWindow succeeds
    glfwSetWindowUserPointer(state->main_window, state);
    glfwSetFramebufferSizeCallback(state->main_window, glfw_frambuffer_window_resize);
    glfwSetWindowSizeCallback(state->main_window, glfw_frambuffer_window_resize);
    glfwSetWindowCloseCallback(state->main_window, glfw_window_close);

    return true;
}


void platform_shutdown(Platform_State* plat_state)
{
    // Create the internal state.
    Linux_Internal_State* state = (Linux_Internal_State*)plat_state->internal_state;
    glfwDestroyWindow(state->main_window);
}

bool platform_pump_messages(Platform_State* plat_state)
{
    /* Poll for and process events */
    glfwPollEvents();
}


void platform_get_vulkan_extension_names(const char*** extension_name_array)
{
    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);

    for (u32 i = 0; i < count; i++)
    {
        //we want to make sure we are not pusshing,
        //VK_KHR_SURFACE_EXTENSION_NAME/ VK_KHR_surface twice
        //since we do a push before hand for it
        if (strcmp(extensions[i], VK_KHR_SURFACE_EXTENSION_NAME) == 0)
        {
            continue;
        }


        darray_push(*extension_name_array, extensions[i]);
    }

    // darray_push(*extension_name_array, &VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
    // darray_push(*extension_name_array, &"VK_KHR_xcb_surface"); // VK_KHR_xlib_surface?
}

// Surface creation for Vulkan, uses the main window for now
bool platform_create_vulkan_surface(Platform_State* plat_state, Renderer* renderer)
{
    DEBUG("Creating Vulkan LINUX PLATFORM surface...");
    Linux_Internal_State* state = (Linux_Internal_State*)plat_state->internal_state;

    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(renderer->instance, state->main_window, NULL, &surface);
    if (result != VK_SUCCESS)
    {
        // Window surface creation failed
        FATAL("Linux vulkan surface creation failed.");
        return false;
    }


    renderer->surface = surface;
    if (renderer->surface == VK_NULL_HANDLE)
    {
        FATAL("Vulkan surface handle is null!");
        return false;
    }

    INFO("Vulkan surface handle has been aqcuired from the platform layer!");

    return true;
}

bool platform_create_window(Platform_State* platform_state, Platform_Window_Config window_config,
                            Madness_Window* out_window_handle)
{
    Linux_Internal_State* state = (Linux_Internal_State*)platform_state->internal_state;

    // default window behaviours
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    // create hidden, show after setup (no white flash)
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    // only matters for fullscreen
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
    // not always-on-top
    glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

    // HiDPI
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE); // window size scales with the monitor's content scale
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE); // framebuffer is native resolution, not upscaled

    // identity: lets the desktop match your .desktop file / icon / taskbar grouping
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "madness_engine");
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "madness_engine");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "madness_engine");

    //position
    glfwWindowHint(GLFW_POSITION_X, window_config.start_pos_x);
    glfwWindowHint(GLFW_POSITION_Y, window_config.start_pos_y);

    // no GL context, Vulkan owns the surface
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    state->window[state->count] = glfwCreateWindow(window_config.start_width, window_config.start_height,
                                                   window_config.name,
                                                   NULL,
                                                   NULL);

    if (!state->main_window)
    {
        MASSERT_MSG_FALSE("LINUX BACKEND: GLFW window creation failed");
        *out_window_handle = (Madness_Window){0};
    }

    *out_window_handle = (Madness_Window){state->count};
    state->count++;
}


bool platform_audio_init(Platform_State* plat_state, int32_t buffer_size, int32_t samples_per_second)
{
}

bool platform_audio_shutdown(Platform_State* plat_state)
{
}

void* platform_reserve_memory(u64 size, bool aligned)
{
}

bool platform_load_dynamic_library_from_handle(DLL_HANDLE handle, const char* file_name)
{
}

File_Watch_Handle platform_register_directory_watch(const char* directory_name)
{
}

bool platform_has_directory_changed(File_Watch_Handle directory_watch_handle)
{
}

bool platform_open_file_dialogue(char* out_path, char* start_file_absolute_path)
{
}

bool platform_update_directory(File_Watch_Handle directory_watch_handle)
{
}

bool platform_poll_directory_changes(File_Watch_Handle directory_watch_handle, File_Watch_Event* out_event)
{
}

bool platform_is_directory_empty(const char* directory_path)
{
}

bool platform_get_assets_from_directory(const char* directory_path, Asset_List_Scan* asset_list_scan)
{
}

bool platform_file_open(Madness_File_Platform* file, const char* path, File_Modes file_modes, Allocator* allocator)
{
}

bool platform_file_close(Madness_File_Platform* file)
{
}

bool platform_file_read(Madness_File_Platform* file)
{
}

bool platform_file_write(Madness_File_Platform* file)
{
}

bool platform_file_read_async()
{
}

bool platform_file_write_async()
{
}

void platform_get_cursor_pos(int* out_x, int* out_y)
{
}

void platform_set_cursor_type(Platform_Cursor_Type cursor_type)
{
    //does nothing rn
    return;
}

void platform_windows_resize(Platform_State* platform_state, int width, int height)
{
}

void platform_copy_to_clipboard(const char* c_string)
{
}

void* platform_allocate(u64 size, bool aligned)
{
    //mmap(); //TODO:
    void* mem_block = malloc(size);
    MASSERT(mem_block);
    return mem_block;


    /*void* ptr = mmap(
        NULL,
        size,
        PROT_READ | PROT_WRITE, // Committed immediately for access
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );*/
}

void platform_free(void* block)
{
    free(block);
}

void* platform_zero_memory(void* block, u64 size)
{
    return memset(block, 0, size);
}

void* platform_copy_memory(void* dest, const void* source, u64 size)
{
    return memcpy(dest, source, size);
}

void* platform_set_memory(void* dest, s32 value, u64 size)
{
    return memset(dest, value, size);
}

f64 platform_get_absolute_time(void)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
}

void platform_sleep(u64 ms)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
#else
    if (ms >= 1000)
    {
        sleep(ms / 1000);
    }
    usleep((ms % 1000) * 1000);
#endif
}

char* platform_get_dynamic_library_extension(void)
{
    return ".so";
}

char* platform_get_static_library_extension(void)
{
    return ".a";
}

typedef struct linux_file_handle
{
    void* file_handle;
    const char* file_name; //doesnt seem super needed
} linux_file_handle;

linux_file_handle file_handles[100];
static u32 file_handle_count = 1;


DLL_HANDLE platform_load_dynamic_library(const char* file_name)
{
    DLL_HANDLE out_handle = {file_handle_count, file_name};

    //probably gonna have to have some sort of internal index for this
    linux_file_handle* file_info = &file_handles[file_handle_count];
    file_info->file_name = file_name;
    file_handle_count++;

    const char* dll_extension_name = platform_get_dynamic_library_extension();
    const char* final_file_name = c_string_concat(file_name, dll_extension_name, NULL);

    // Load the library
    file_info->file_handle = dlopen(final_file_name, RTLD_NOW);
    if (!file_info->file_handle)
    {
        const char* err = dlerror();
        fprintf(stderr, "dlopen failed: %s\n", err);
    }

    out_handle.file_name = file_info->file_name;

    return out_handle;
}

bool platform_unload_dynamic_library(DLL_HANDLE handle)
{
    linux_file_handle* file = &file_handles[handle.handle];
    if (file->file_handle)
    {
        dlclose(file->file_handle);
        return true;
    }

    WARN("LINUX: UNLOAD DYNAMIC LIBRARY, not a valid file handle")
    return false;
}

bool platform_reload_dynamic_library(DLL_HANDLE handle)
{
    platform_unload_dynamic_library(handle);
    platform_load_dynamic_library(handle.file_name);
}

void* platform_get_function_address(DLL_HANDLE handle, const char* function_name)
{
    linux_file_handle* file = &file_handles[handle.handle];
    void* out_data = dlsym(file->file_handle, function_name);
    const char* err = dlerror();
    if (err)
    {
        fprintf(stderr, "dlsym failed: %s\n", err);
    }

    return out_data;
}

File_Watch_Handle platform_register_file_watch(const char* file_name)
{
    UNIMPLEMENTED();
    return (File_Watch_Handle){.handle = 0};
}

bool platform_has_filed_changed(File_Watch_Handle file_watch_handle)
{
    UNIMPLEMENTED();
    false;
}

bool platform_file_copy(const char* source_file, char* new_file)
{
    //mostly just a windows thing
    /*
    //TODO: these are suppose to be file pointers, meaning i have to do a file open on them
    size_t offset = 0;
    size_t file_copy_byte_size;
    ssize_t bytes_copied = sendfile(source_file, new_file, offset,file_copy_byte_size);
    perror ("sendfile");
    // if(errno == ????)

    */
}

bool platform_create_file(const char* file_path)
{
    // O_WRONLY writing only
    // O_CREAT creates it, O_EXCL forces it to fail if it already exists
    // 0644 sets default read/write permissions for the file
    int fd = open(file_path, O_WRONLY | O_CREAT | O_EXCL, 0644);

    if (fd == -1)
    {
        if (errno == EEXIST)
        {
            INFO("PLATFORM CREATE FILE: File already exists. No action taken.\n");
            return true;
        }
        else
        {
            M_ERROR("PLATFORM CREATE FILE: Error opening file");
            return false;
        }
    }

    close(fd);
    return true;
}

bool platform_create_directory(const char* directory_path)
{
    // 0777 grants read, write, and execute permissions to everyone (modified by umask)
    if (mkdir(directory_path, 0777) == -1)
    {
        if (errno == EEXIST)
        {
            INFO("PLATFORM CREATE DIRECTORY: '%s' already exists.", directory_path);
            return true;
        }
        else
        {
            // Something else went wrong
            // https://linux.die.net/man/2/mkdir for error codes
            // perror("Failed to create directory");
            M_ERROR("PLATFORM CREATE DIRECTORY: Error creating directory");
            return false;
        }
    }

    return true;
}

bool platform_does_file_exist(const char* file_path)
{
    // you can use access() or stat()
    struct stat buffer;

    if (stat(file_path, &buffer) != 0)
    {
        DEBUG("PLATFORM FILE EXISTS: [%s] does not exist.", file_path)
        return false;
    }
    if (S_ISDIR(buffer.st_mode))
    {
        DEBUG("PLATFORM FILE EXISTS: [%s] path is a directory not a file.", file_path)
        return false;
    }
    if (S_ISREG(buffer.st_mode))
    {
        return true;
    }

    FATAL("PLATFORM FILE EXISTS: unintended code reached");
    return false;
}

bool platform_does_directory_exist(const char* path)
{
    // you can use access() or stat()
    struct stat buffer;

    if (stat(path, &buffer) != 0)
    {
        DEBUG("PLATFORM DIRECTORY EXISTS: [%s] does not exist.", path)
        return false;
    }
    if (S_ISDIR(buffer.st_mode))
    {
        return true;
    }
    if (S_ISREG(buffer.st_mode))
    {
        DEBUG("PLATFORM DIRECTORY EXISTS: [%s] path is a file not a diretory.", path)
        return false;
    }

    FATAL("PLATFORM FILE EXISTS: unintended code reached");
    return false;
}


void platform_get_window_size(s32* width, s32* height)
{
}

void platform_get_window_pos(s32* x, s32* y)
{
}


void platform_set_cursor_pos(int x, int y)
{
}

void platform_generate_uuid(u64* high, u64* low)
{
    u8 bytes[16];

    ssize_t result = getrandom(bytes, sizeof(bytes), 0);

    if (result != sizeof(bytes))
    {
        high = 0;
        low = 0;
        MASSERT(false);
        return;
    }

    memcpy(high, bytes, sizeof(u64));
    memcpy(low, bytes + 8, sizeof(u64));
}


#endif
