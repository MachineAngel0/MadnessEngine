//using a unity/jumbo build

//ENGINE
#include "core/serialization/network_serialization.c"
#include "core/serialization/file_serialization.c"#include "core/asserts.h"
#include "core/color_asci.h"
#include "core/defines.h"
#include "core/event.h"
#include "core/input.h"
#include "core/logger.h"
#include "core/math_types.h"
#include "core/misc_util.h"
#include "core/mmath.h"
#include "core/unit_test.h"


//PLATFORM
#include "platform/audio.h"
#include "platform/clock.h"
#include "platform/gamepad.h"
#include "platform/hot_reload.h"
#include "platform/platform.h"
#include "platform/platform_win32.c"
#include "platform/platform_linux.c"

//DSA
#include "dsa/arena.h"
#include "dsa/arena_freelist.h"
#include "dsa/arena_pool.h"
#include "dsa/array.h"
#include "dsa/b_tree_red_black.h"
#include "dsa/binary_tree.h"
#include "dsa/darray.h"
#include "dsa/dsa_utility.h"
#include "dsa/hash_map.h"
#include "dsa/hash_set.h"
#include "dsa/heap.h"
#include "dsa/linked_list_generic.h"
#include "dsa/queue.h"
#include "dsa/stack.h"
#include "dsa/str.c"
#include "dsa/queue.h"
#include "dsa/unions.h"


//RENDERER
#include "renderer/Mesh.h"
#include "renderer/Renderer.h"
#include "renderer/Tetris.h"
#include "renderer/text.h"
#include "renderer/UI.h"
#include "renderer/vk_buffer.h"
#include "renderer/vk_command_buffer.h"
#include "renderer/vk_descriptor.h"
#include "renderer/vk_device.h"
#include "renderer/vk_renderpass.h"
#include "renderer/vk_vertex.h"


//LIB
#include "lib/stb_impl.c"
