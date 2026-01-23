#ifndef ASSERTS_H
#define ASSERTS_H

// Disable assertions by commenting out the below line.
#define MASSERTIONS_ENABLED
#include "logger.h"

#ifdef MASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif




#include <stdint.h>


//NOTE: this is implemented inside the logger
void report_assertion_failure(const char* expression, const char* message, const char* file, int32_t line);
void report_unimplemented(const char* file, int32_t line);
void report_unreachable(const char* file, int32_t line);


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


#define UNIMPLEMENTED() report_unimplemented(__FILE__, __LINE__);
#define UNREACHABLE() report_unreachable(__FILE__, __LINE__)







#endif
