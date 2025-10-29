#include "platform.h"

// Windows platform layer.
#if MPLATFORM_WINDOWS

#include <dsound.h>
#include <math.h>

#include "logger.h"

#include <windows.h>
#include <windowsx.h>  // param input extraction
#include <stdlib.h>
#include <mmsystem.h>

#include "input.h"

typedef struct internal_state
{
    HINSTANCE h_instance;
    HWND hwnd;
} internal_state;

// Clock
static f64 clock_frequency;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

b8 platform_startup(platform_state* plat_state,
                    const char* application_name,
                    i32 x, i32 y,
                    i32 width, i32 height)
{
    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state* state = (internal_state *) plat_state->internal_state;

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
    clock_frequency = 1.0 / (f64) frequency.QuadPart;
    QueryPerformanceCounter(&start_time);


    //Audio
    // platform_audio_init(plat_state, 4000,4000);

    return TRUE;
}

void platform_shutdown(platform_state* plat_state)
{
    // Simply cold-cast to the known type.
    internal_state* state = (internal_state *) plat_state->internal_state;
    timeEndPeriod(1);; // Set system timer resolution to 1 ms

    if (state->hwnd)
    {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

b8 platform_pump_messages(platform_state* plat_state)
{
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return TRUE;
}

typedef HRESULT WINAPI DSoundCreate(LPCGUID, LPDIRECTSOUND*, LPUNKNOWN);
#define DirectSoundCreate DSoundCreate

bool platform_audio_init(platform_state* plat_state, int32_t buffer_size, int32_t samples_per_second)
{

    //get the state
    internal_state* state = (internal_state *) plat_state->internal_state;

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

    LPDIRECTSOUND dsound;
    if (!SUCCEEDED(dsound_func(0, &dsound, 0)))
    {
        WARN("Failed to init Direct Sound");
        return false;
    }
    dsound->lpVtbl->SetCooperativeLevel(dsound, state->hwnd, DSSCL_PRIORITY);

    DSBUFFERDESC buffer_desc = {0};
    ZeroMemory(&buffer_desc, sizeof(buffer_desc));
    buffer_desc.dwSize = sizeof(buffer_desc);
    buffer_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    LPDIRECTSOUNDBUFFER primary_buffer = NULL;
    if (!SUCCEEDED(IDirectSound_CreateSoundBuffer(dsound, &buffer_desc, &primary_buffer, 0)))
    {
        WARN("FAILED TO CREATE PRIMARY BUFFER");
        return false;
    }
    //the macro is just shortening the lpvtbl part
    // dsound->lpVtbl->CreateSoundBuffer(dsound, &buffer_desc, &primary_buffer, 0);


    buffer_desc.dwBufferBytes = buffer_size;
    primary_buffer->lpVtbl->SetFormat(primary_buffer, &wformat);
    IDirectSoundBuffer_SetFormat(primary_buffer, &wformat);

    DSBUFFERDESC buffer_desc2 = {};
    buffer_desc2.dwSize = sizeof(buffer_desc);
    buffer_desc2.dwFlags = 0;
    buffer_desc2.dwBufferBytes = buffer_size;
    buffer_desc2.lpwfxFormat = &wformat;

    LPDIRECTSOUNDBUFFER secondary_buffer = NULL;
    if (!SUCCEEDED(IDirectSound_CreateSoundBuffer(dsound, &buffer_desc, &secondary_buffer, 0)))
    {
        WARN("FAILED TO CREATE SECONDARY BUFFER");
        return false;
    };
    // secondary_buffer->lpVtbl->SetFormat(secondary_buffer, &wformat);
    IDirectSoundBuffer_SetFormat(secondary_buffer, &wformat);


    //
    // Fill the secondary buffer with a test tone
    //
    VOID* region1 = NULL;
    DWORD region1_size = 0;
    VOID* region2 = NULL;
    DWORD region2_size = 0;

    if (SUCCEEDED(secondary_buffer->lpVtbl->Lock(
        secondary_buffer, 0, buffer_size,
        &region1, &region1_size,
        &region2, &region2_size,
        0)))
    {
        int16_t *sample = (int16_t*)region1;
        int total_samples = region1_size / sizeof(int16_t);
        float tone_hz = 440.0f;
        float phase = 0.0f;
        float phase_inc = 3.14f*2 * tone_hz / (float)samples_per_second;

        for (int i = 0; i < total_samples; i += 2)
        {
            float sample_val = sinf(phase) * 3000.0f;
            sample[i + 0] = (int16_t)sample_val; // Left
            sample[i + 1] = (int16_t)sample_val; // Right
            phase += phase_inc;
            if (phase > 3.14f*2) phase -= 3.14f*2;
        }

        if (region2)
        {
            int16_t *sample2 = (int16_t*)region2;
            int total_samples2 = region2_size / sizeof(int16_t);
            phase = 0.0f;
            for (int i = 0; i < total_samples2; i += 2)
            {
                float sample_val = sinf(phase) * 3000.0f;
                sample2[i + 0] = (int16_t)sample_val;
                sample2[i + 1] = (int16_t)sample_val;
                phase += phase_inc;
                if (phase > 3.14f*2) phase -= 3.14f*2;
            }
        }

        secondary_buffer->lpVtbl->Unlock(
            secondary_buffer,
            region1, region1_size,
            region2, region2_size);
    }

    // Start playback looping
    secondary_buffer->lpVtbl->Play(secondary_buffer, 0, 0, DSBPLAY_LOOPING);



    return true;
}

bool platform_audio_shutdown(platform_state* plat_state)
{
    //TODO:
    return true;
}


void* platform_allocate(u64 size, b8 aligned)
{
    return malloc(size);
}

void platform_free(void* block, b8 aligned)
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
    return (f64) now_time.QuadPart * clock_frequency;
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
            // TODO: Fire an event for the application to quit.
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
        {
            // Get the updated size.
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;

            // TODO: Fire an event for window resize.
        }
        break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            // Key pressed/released
            b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            keys key = (u16)w_param;
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
            if (z_delta != 0) {
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
            //b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            // TODO: input processing.
        }
        break;
    }

    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

#endif // MPLATFORM_WINDOWS
