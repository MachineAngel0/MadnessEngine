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

//NOTE: these are defined in the platform layer
// #define GLFW_EXPOSE_NATIVE_COCOA // mac os
// #define GLFW_EXPOSE_NATIVE_WIN32
// #define GLFW_EXPOSE_NATIVE_X11
// #define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_INCLUDE_VULKAN //SPEC: makes the GLFW header include the Vulkan vulkan/vulkan.h header in addition to any selected OpenGL or OpenGL ES header.
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "tracy_lib/tracy-0.14.1/public/tracy/TracyC.h"

//imgui
#include "dcimgui.h"

// #include "cimnodes-master/cimnodes.h"

// #include "dcimgui_impl_glfw.h"
// #include "dcimgui_impl_win32.h"
// #define VK_USE_PLATFORM_WIN32_KHR
// #include "dcimgui_impl_vulkan.h"
