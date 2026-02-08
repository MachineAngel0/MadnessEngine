#include "platform.h"

// Windows platform layer.
#if MPLATFORM_WINDOWS

#include <dsound.h>

#include "logger.h"

#include <windows.h>
#include <windowsx.h>  // param input extraction
#include <timeapi.h>
#include <stdlib.h>
#include <mmsystem.h>

#include "darray.h"
#include "input.h"

//renderer

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "vk_device.h"


typedef struct internal_state
{
    HINSTANCE h_instance;
    HWND hwnd;
    VkSurfaceKHR surface;
} internal_state;

// Clock
static f64 clock_frequency;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

bool platform_startup(platform_state* plat_state,
                      const char* application_name,
                      i32 x, i32 y,
                      i32 width, i32 height)
{
    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state* state = (internal_state*)plat_state->internal_state;

    state->h_instance = GetModuleHandleA(0);

    // Setup and register window class.
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS; // Get double-clicks
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // NULL; // Manage the cursor manually
    wc.hbrBackground = NULL; // Transparent
    wc.lpszClassName = "kohi_window_class";

    if (!RegisterClassA(&wc))
    {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // Create window
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = width;
    u32 client_height = height;

    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    // Obtain the size of the border.
    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    // In this case, the border rectangle is negative.
    window_x += border_rect.left;
    window_y += border_rect.top;

    // Grow by the size of the OS border.
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(
        window_ex_style, "kohi_window_class", application_name,
        window_style, window_x, window_y, window_width, window_height,
        0, 0, state->h_instance, 0);

    if (handle == 0)
    {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        FATAL("Window creation failed!");
        return FALSE;
    }
    else
    {
        state->hwnd = handle;
    }

    // Show the window
    b32 should_activate = 1; // TODO: if the window should not accept input, this should be false.
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
    // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
    ShowWindow(state->hwnd, show_window_command_flags);


    //set this so that ms sleep is much more accurate
    timeBeginPeriod(1); // Set system timer resolution to 1 ms

    // Clock setup
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0 / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);


    //Audio
    // platform_audio_init(plat_state, 4000,4000);

    return TRUE;
}

void platform_shutdown(platform_state* plat_state)
{
    // Simply cold-cast to the known type.
    internal_state* state = (internal_state*)plat_state->internal_state;
    timeEndPeriod(1);; // Set system timer resolution to 1 ms

    if (state->hwnd)
    {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

bool platform_pump_messages(platform_state* plat_state)
{
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return TRUE;
}


void* platform_allocate(u64 size, bool aligned)
{
    return malloc(size);
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

void* platform_set_memory(void* dest, i32 value, u64 size)
{
    return memset(dest, value, size);
}


f64 platform_get_absolute_time()
{
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (f64)now_time.QuadPart * clock_frequency;
}

void platform_sleep(u64 ms)
{
    Sleep(ms);
}

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg)
    {
    case WM_ERASEBKGND:
        // Notify the OS that erasing will be handled by the application to prevent flicker.
        return 1;
    case WM_CLOSE:
        event_context data = {};
        event_fire(EVENT_APP_QUIT, 0, data);
        return true;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        {
            RECT r;
            GetClientRect(hwnd, &r);
            u32 width = r.right - r.left;
            u32 height = r.bottom - r.top;
            // Fire the event. The application layer should pick this up, but not handle it
            // as it shouldn't be visible to other parts of the application.
            event_context context;
            context.data.u16[0] = (u16)width;
            context.data.u16[1] = (u16)height;
            event_fire(EVENT_APP_RESIZE, 0, context);
        }
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        {
            // Key pressed/released
            bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            keys key = (u16)w_param;

            //if alt
            if (w_param == VK_MENU)
            {
                //check if left or right alt key
                if (GetKeyState(VK_RMENU) & 0x8000)
                {
                    key = KEY_RALT;
                }
                else if (GetKeyState(VK_LMENU) & 0x8000)
                {
                    key = KEY_LALT;
                }
            }
            //shift key
            else if (w_param == VK_SHIFT)
            {
                if (GetKeyState(VK_RSHIFT) & 0x8000)
                {
                    key = KEY_RSHIFT;
                }
                else if (GetKeyState(VK_LSHIFT) & 0x8000)
                {
                    key = KEY_LSHIFT;
                }
            }
            //control key
            else if (w_param == VK_CONTROL)
            {
                if (GetKeyState(VK_RCONTROL) & 0x8000)
                {
                    key = KEY_RCONTROL;
                }
                else if (GetKeyState(VK_LCONTROL) & 0x8000)
                {
                    key = KEY_LCONTROL;
                }
            }

            input_process_key(key, pressed);
        }
        break;
    case WM_MOUSEMOVE:
        {
            // Mouse move
            i32 x_position = GET_X_LPARAM(l_param);
            i32 y_position = GET_Y_LPARAM(l_param);

            input_process_mouse_move(x_position, y_position);
        }
        break;
    case WM_MOUSEWHEEL:
        {
            i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0)
            {
                // Flatten the input to an OS-independent (-1, 1)
                z_delta = (z_delta < 0) ? -1 : 1;
                input_process_mouse_wheel(z_delta);
            }
        }
        break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        {
            bool pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            mouse_buttons mouse_button = MOUSE_BUTTON_MAX_BUTTONS;
            switch (msg)
            {
            case WM_LBUTTONUP:
            case WM_LBUTTONDOWN:
                mouse_button = MOUSE_BUTTON_LEFT;
                break;
            case WM_RBUTTONUP:
            case WM_RBUTTONDOWN:
                mouse_button = MOUSE_BUTTON_RIGHT;
                break;
            case WM_MBUTTONUP:
            case WM_MBUTTONDOWN:
                mouse_button = MOUSE_BUTTON_MIDDLE;
                break;
            default: break;
            }

            if (mouse_button != MOUSE_BUTTON_MAX_BUTTONS)
            {
                input_process_mouse_button(mouse_button, pressed);
            }
        }
        break;
    }

    return DefWindowProcA(hwnd, msg, w_param, l_param);
}


typedef struct windows_file_handle
{
    HMODULE dll_handle;
} windows_file_handle;

windows_file_handle file_handles[100];

char* platform_get_dynamic_library_extension(void)
{
    return ".dll";
}

char* platform_get_static_library_extension(void)
{
    return ".lib";
}

DLL_HANDLE platform_dll_load(const char* file_name, const char* function_name)
{
    //probably gonna have to have some sort of internal index for this
    windows_file_handle file_info = file_handles[0];

    if (file_info.dll_handle)
    {
        if (FreeLibrary(file_info.dll_handle) == 0)
        {
            WARN("FAILED TO UNLOAD DLL\n")
            WARN("%d", GetLastError());
        }
    }


    //TODO:
    // file_name + temp + dll
    const char* temp_dll_name = "temp_filename.dll";

    CopyFile(file_name, temp_dll_name, 0);
    // {
    // WARN("FAILED TO COPY DLL from %s to %s. Error: %d", dll_file_name, temp_dll_name, GetLastError());
    // return false;
    // }

    file_info.dll_handle = LoadLibraryA(temp_dll_name);
    //TODO: Temp code
    return (DLL_HANDLE){0, 0, 0};
}

bool platform_dll_unload(DLL_HANDLE handle)
{
    windows_file_handle file = file_handles[handle.handle];
    if (file.dll_handle)
    {
        if (FreeLibrary(file.dll_handle) == 0)
        {
            WARN("FAILED TO UNLOAD DLL\n")
            WARN("%d", GetLastError());
            return false;
        }
    }
    return true;
}

bool platform_dll_reload(DLL_HANDLE handle)
{
    platform_dll_unload(handle);
    platform_dll_load(handle.file_name, handle.function_name);
    return true;
}


void* platform_get_function_address(DLL_HANDLE handle, const char* function_name)
{
    //get whatever
    windows_file_handle file_handle = file_handles[handle.handle];
    return GetProcAddress(file_handle.dll_handle, function_name);
}

bool platform_file_copy(const char* source_file, char* new_file)
{
    CopyFile(source_file, new_file, 0);
    return true;
}


void platform_get_vulkan_extension_names(const char*** extension_name_array)
{
    darray_push(*extension_name_array, &"VK_KHR_win32_surface");
}

bool platform_create_vulkan_surface(platform_state* plat_state, vulkan_context* vulkan_context)
{
    DEBUG("Creating Vulkan WINDOWS PLATFORM surface...");

    // Simply cold-cast to the known type.
    internal_state* state = (internal_state*)plat_state->internal_state;

    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hinstance = state->h_instance;
    create_info.hwnd = state->hwnd;

    VkResult result = vkCreateWin32SurfaceKHR(vulkan_context->instance, &create_info,
                                              vulkan_context->allocator, &state->surface);
    if (result != VK_SUCCESS)
    {
        FATAL("Vulkan surface creation failed.");
        return false;
    }


    vulkan_context->surface = state->surface;
    if (vulkan_context->surface == VK_NULL_HANDLE)
    {
        FATAL("Vulkan surface handle is null!");
    }

    INFO("Vulkan surface handle has been aqcuired from the platform layer!");

    return TRUE;
}

#endif // MPLATFORM_WINDOWS
