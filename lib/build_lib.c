#include "miniaudio_impl.c"

#include "stb_impl.c"

#include "msdf_impl.c"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <ft2build.h>

#define CLAY_IMPLEMENTATION
#include <clay.h>


//the include path is incorrect inside the ufbx.c file, so we use this macro which will redefine the location
#define UFBX_HEADER_PATH "../ufbx/ufbx.h"
#include "ufbx/ufbx.h"
#include "ufbx/ufbx.c"

