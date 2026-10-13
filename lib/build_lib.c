#include "miniaudio_impl.c"

#include "stb_impl.c"

#include "msdf_impl.c"

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include <ft2build.h>

// #define CLAY_IMPLEMENTATION
// #include <clay.h>


//the include path is incorrect inside the ufbx.c file, so we use this macro which will redefine the location
#define UFBX_HEADER_PATH "../ufbx/ufbx.h"
#include "ufbx/ufbx.h"
#include "ufbx/ufbx.c"

#define GLFW_INCLUDE_VULKAN //SPEC: makes the GLFW header include the Vulkan vulkan/vulkan.h header in addition to any selected OpenGL or OpenGL ES header.
#include <glfw/glfw-master/include/GLFW/glfw3.h>


#include "tracy_lib/tracy/public/tracy/TracyC.h"
