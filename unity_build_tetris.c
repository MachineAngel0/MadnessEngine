

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


//Renderer

#include "renderer/build_renderer.h"
#include "renderer/build_renderer.c"



#include "game/Tetris/tetris_build.h"
#include "game/Tetris/tetris_build.c"


//APP and Entry Point
#include "game/Tetris/tetris_app.h"
#include "game/Tetris/tetris_app.c"


