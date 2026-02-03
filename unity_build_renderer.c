

//CORE
#include "core/core_unity.c"


//LIB
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "lib/stb_impl.c"

#include <ft2build.h>

#include <vulkan/vulkan.h>

#include "SPIRV-Reflect-main/spirv_reflect.h"
#include "SPIRV-Reflect-main/spirv_reflect.c"


//TODO: MAYBE, ALSO SHOULD GO INTO THE CORE_UNITY, ACCESSIBLE BY EVERYONE
// #include "cglm/cglm.h"


//Renderer
#include "renderer/renderer_unity.c"



// #include "lib/cgltf-master/cgltf.h"
// #include "lib/cgltf-master/cgltf_write.h"

//APP and Entry Point
#include "application.h"
#include "renderer_application.c"
#include "renderer_entry.h"
#include "renderer_entry.c"

