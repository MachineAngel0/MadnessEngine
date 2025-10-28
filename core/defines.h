#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>

// Unsigned int types.
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Signed int types.
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Floating point types
typedef float f32;
typedef double f64;

// Boolean types
typedef int b32;
typedef char b8; // there is bool but idk if i want to use it


// Properly define static assertions.
#if defined(__clang__) || defined(__gcc__) || defined(__GNUC__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// Ensure all types are of the correct size.
STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

// useful for C89
#define TRUE 1
#define FALSE 0

#define BITFLAG(x) (1 << (x))

#define KB(x) = (1024 * x) // kilobytes
#define MB(x) = (1024 * KB(x)) // megabytes
#define GB(x) = (1024 * MB(x)) //gigabytes
// #define TB(x) = (x * GB(x)) // terabyte, should never need this


// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define MPLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#define KPLATFORM_LINUX 1
#if defined(__ANDROID__)
#define KPLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
// Catch anything not caught by the above.
#define KPLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#define KPLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#define KPLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#define KPLATFORM_IOS 1
#define KPLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define KPLATFORM_IOS 1
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform!"
#endif

#ifdef MEXPORT
// Exports
#ifdef _MSC_VER
#define MAPI __declspec(dllexport)
#else
#define MAPI __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define MAPI __declspec(dllimport)
#else
#define MAPI
#endif
#endif


//forcing inline

#if defined(_MSC_VER)
#define MINLINE __forceinline
#define MNOINLINE __declspec(noinline)
#else
#define MINLINE static inline
#define MNOINLINE
#endif



//TODO: the defer macro

#define macro_var(name) concat(name, __LINE__)
#define defer(start, end) for ( \
    int macro_var(_i_) = (start, 0); \
    !macro_var(_i_); \
    (macro_var(_i_) += 1), end)\


// defer example
// #define gui(gui_begin, gui_end())
// gui{...}

// file = file_open()
// scope(file_close()) {...}


#endif //DEFINES_H
