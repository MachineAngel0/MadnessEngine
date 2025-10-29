//using a unity/jumbo build



//CORE
#include "core/color_asci.h"
#include "core/defines.h"
#include "core/event.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/math_types.h"
#include "core/misc_util.h"
#include "core/mmath.h"
#include "core/unit_test.h"


//CORE - SERIALIZATION
#include "core/serialization/network_serialization.c"
#include "core/serialization/file_serialization.c"#include "core/asserts.h"

//CORE - PLATFORM
#include "core/platform/audio.h"
#include "core/platform/clock.h"
#include "core/platform/gamepad.h"
#include "core/platform/hot_reload.h"
#include "core/platform/platform.h"
#include "core/platform/platform_win32.c"
#include "core/platform/platform_linux.c"

//CORE - DSA
#include "core/dsa/arena.h"
#include "core/dsa/arena_freelist.h"
#include "core/dsa/arena_pool.h"
#include "core/dsa/array.h"
#include "core/dsa/b_tree_red_black.h"
#include "core/dsa/binary_tree.h"
#include "core/dsa/darray.h"
#include "core/dsa/dsa_utility.h"
#include "core/dsa/hash_map.h"
#include "core/dsa/hash_set.h"
#include "core/dsa/heap.h"
#include "core/dsa/linked_list_generic.h"
#include "core/dsa/queue.h"
#include "core/dsa/stack.h"
#include "core/dsa/str.c"
#include "core/dsa/queue.h"
#include "core/dsa/unions.h"


//RENDERER
// #include "renderer/Mesh.h"
// #include "renderer/Renderer.h"
// #include "renderer/Tetris.h"
// #include "renderer/text.h"
// #include "renderer/UI.h"
// #include "renderer/vk_buffer.h"
// #include "renderer/vk_command_buffer.h"
// #include "renderer/vk_descriptor.h"
// #include "renderer/vk_device.h"
// #include "renderer/vk_renderpass.h"
// #include "renderer/vk_vertex.h"


//GAME
#include "game/unit_component/actions.h"
#include "game/unit_component/health.h"


//LIB
#include "lib/stb_impl.c"


#include "app_types.h"
#include "application.c"
#include "application.h"
// #include "entry_editor.h"
// #include "entry_game.c"
#include "entry_game.h"

