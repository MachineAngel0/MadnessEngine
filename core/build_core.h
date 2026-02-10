//C- STD library
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//CORE - Top Level
#include "../core/defines.h"
#include "../core/unit_test.h"
#include "../core/color_asci.h"
#include "../core/platform/event.h"
#include "../core/platform/input.h"
#include "../core/logger.h"
#include "../core/maths/math_types.h"
#include "../core/misc_util.h"
#include "../core/maths/math_lib.h"
#include "../core/maths/transforms.h"
#include "../core/c_string.h"
#include "../core/id_gen.h"



//CORE - SERIALIZATION
#include "../core/serialization/network_serialization.h"
#include "../core/serialization/file_serialization.h"
#include "../core/asserts.h"

//CORE - PLATFORM
#include "../core/platform/audio.h"
#include "../core/platform/clock.h"
#include "../core/platform/gamepad.h"
#include "../core/platform/platform.h"
#include "../core/platform/filesystem.h"

//CORE - MEMORY
#include "../core/memory/memory_tracker.h"
#include "../core/memory/memory_system.h"


//CORE - DSA
#include "../core/dsa/arena.h"
#include "../core/dsa/arena_stack.h"
#include "../core/dsa/arena_freelist.h"
#include "../core/dsa/arena_pool.h"
#include "../core/dsa/array.h"
#include "../core/dsa/b_tree_red_black.h"
#include "../core/dsa/binary_tree.h"
#include "../core/dsa/darray.h"
#include "../core/dsa/dsa_utility.h"
#include "../core/dsa/hash_map.h"
#include "../core/dsa/hash_set.h"
#include "../core/dsa/hash_map_string.h"
#include "../core/dsa/heap.h"
#include "../core/dsa/linked_list.h"
#include "../core/dsa/queue.h"
#include "../core/dsa/stack.h"
#include "../core/dsa/str.h"
#include "../core/dsa/str_builder.h"
#include "../core/dsa/queue.h"
#include "../core/dsa/unions.h"



