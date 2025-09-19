#pragma once

#include "defines.h"

// Disable assertions by commenting out the below line.
#define MASSERTIONS_ENABLED

#ifdef MASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

MAPI void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);

//__FILE__ gets the c file of the error, __LINE__ gets the line in the c file of the error
//#expr outputs for example 1==0 instead of null(which the value technically is)

#define MASSERT(expr)                                                \
    {                                                                \
        if (expr) {                                                  \
        } else {                                                     \
            report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                            \
        }                                                            \
    }

#define MASSERT_MSG(expr, message)                                        \
    {                                                                     \
        if (expr) {                                                       \
        } else {                                                          \
            report_assertion_failure(#expr, message, __FILE__, __LINE__); \
            debugBreak();                                                 \
        }                                                                 \
    }

#ifdef _DEBUG
#define MASSERT_DEBUG(expr)                                          \
    {                                                                \
        if (expr) {                                                  \
        } else {                                                     \
            report_assertion_failure(#expr, "", __FILE__, __LINE__); \
            debugBreak();                                            \
        }                                                            \
    }
#else
#define MASSERT_DEBUG(expr)  // Does nothing at all
#endif

#else
#define MASSERT(expr)               // Does nothing at all
#define MASSERT_MSG(expr, message)  // Does nothing at all
#define MASSERT_DEBUG(expr)         // Does nothing at all
#endif