
//CORE - DSA
#include "../core/dsa/arena.c"
#include "../core/dsa/arena_stack.c"
#include "../core/dsa/arena_freelist.c"
#include "../core/dsa/arena_pool.c"
#include "../core/dsa/array.c"
#include "../core/dsa/b_tree_red_black.c"
#include "../core/dsa/binary_tree.c"
#include "../core/dsa/darray.c"
#include "../core/dsa/hash_map.c"
#include "../core/dsa/hash_set.c"
#include "../core/dsa/hash_map_string.c"
#include "../core/dsa/heap.c"
#include "../core/dsa/linked_list.c"
#include "../core/dsa/queue.c"
#include "../core/dsa/stack.c"
#include "../core/dsa/str.c"
#include "../core/dsa/str_builder.c"


//MATHS
#include "../core/maths/transforms.c"
#include "../core/c_string.c"



//CORE - MEMORY
#include "../core/memory/memory_tracker.c"
#include "../core/memory/memory_system.c"




//CORE - PLATFORM
#include "../core/platform/event.c"
#include "../core/platform/input.c"
#include "../core/platform/audio.c"
#include "../core/platform/clock.c"
#include "../core/platform/filesystem.c"
#if MPLATFORM_WINDOWS
#include "../core/platform/platform_win32.c"
#include "../core/platform/gamepad_win32.c"
#elif MPLATFORM_LINUX
#include "../core/platform/platform_linux.c"
#include "../core/platform/gamepad_linux.c"
#endif



//CORE - SERIALIZATION
#include "../core/serialization/network_serialization.c"
#include "../core/serialization/file_serialization.c"

#include "unit_test.c"
