

//CORE
#include "core/build_core.h"
#include "core/build_core.c"

//VULKAN
#include <vulkan/vulkan.h>

#include "SPIRV-Reflect-main/spirv_reflect.h"
#include "SPIRV-Reflect-main/spirv_reflect.c"


//the include path is incorrect inside the ufbx.c file, so we use this macro which will redefine the location
#define UFBX_HEADER_PATH "../ufbx/ufbx.h"
#include "ufbx/ufbx.h"
#include "ufbx/ufbx.c"


//TODO: MAYBE, ALSO SHOULD GO INTO THE CORE_UNITY, ACCESSIBLE BY EVERYONE
// #include "cglm/cglm.h"


//Renderer
#include "renderer/build_renderer.h"
#include "renderer/build_renderer.c"



// #include "lib/cgltf-master/cgltf.h"
// #include "lib/cgltf-master/cgltf_write.h"

//APP and Entry Point
#include "renderer_dev.h"
#include "renderer_dev.c"


