#ifndef PROFILER_H
#define PROFILER_H



// profiler.h
#ifdef TRACY_ENABLE

#include <tracy/TracyC.h>

#define PROFILE_FRAME() \
TracyCFrameMark

#define PROFILE_ZONE(name) \
TracyCZoneN(ctx_##name, #name, true)

#define PROFILE_ZONE_END(name) \
TracyCZoneEnd(ctx_##name)

//what tracy looks like without the extra wrapper
// TracyCZoneN(ctx, name, true)
// TracyCZoneEnd(ctx)



#else

#define PROFILE_FRAME()
#define PROFILE_ZONE(name)
#define PROFILE_ZONE_END(name)

#endif


#endif //PROFILER_H

