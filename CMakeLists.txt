cmake_minimum_required(VERSION 3.31)
project(MadnessEngine C)

set(CMAKE_C_STANDARD 11) #i like alignas() so its staying as 11, 99 if I really want the portability
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

#generate a compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_UNITY_BUILD TRUE) # UNITY BUILD

set(MADNESS_BUILD_TARGETS
        MadnessEngine
        MadnessEngine_game
        MADNESSGAME
        MadnessEngine_renderer
        MADNESSRENDERER
        MadnessEngine_editor
)


# COMPILER FLAGS #
#Windows
if (MSVC)
    #    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /Wall /permissive- /Zi /fsanitize=address ")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /fsanitize=address")
    add_compile_options(/wd4255) # disables function prototype warnings
    add_compile_options(/wd4464) # path contains '..'
endif ()

#GCC/MINGW OR CLANG
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wvla -Wfloat-equal -Werror=vla ")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Waggregate-return -Wconversion") # additional compiler flags

    #LINUX
    #if(UNIX AND NOT CMAKE_C_COMPILER_ID STREQUAL "GNU") # for some reason gnu not working
    if (UNIX) # for some reason gnu not working
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
        set(CMAKE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    endif ()
endif ()


find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})

include_directories(MadnessEngine ./core/)
include_directories(MadnessEngine ./core/dsa/)
include_directories(MadnessEngine ./core/platform/)
include_directories(MadnessEngine ./core/serialization/)
include_directories(MadnessEngine ./editor/)
include_directories(MadnessEngine ./renderer/)
include_directories(MadnessEngine ./renderer/shaders)
include_directories(MadnessEngine ./game/)
include_directories(MadnessEngine ./lib/)
include_directories(MadnessEngine ./lib/cgltf-master)
include_directories(MadnessEngine ./z_assets/)


add_executable(MadnessEngine
        main.c
        unity_build_game.c
)

### GAME ###
add_executable(MadnessEngine_game
        unity_build_game.c
)
add_library(MADNESSGAME SHARED
        unity_build_game.c
)

### RENDERER ###

add_executable(MadnessEngine_renderer
        unity_build_renderer.c
)
add_library(MADNESSRENDERER SHARED
        unity_build_renderer.c
)

### EDITOR ###
add_executable(MadnessEngine_editor
        editor_entry.h
        unity_build_game.c
)

### CORE ###
add_executable(CORE_UNIT_TEST
        core/core_unit_test_main.c
        core/core_unity.c

)


#libs


if (WIN32)
    SET(MADNESS_WIN_LIBS
            Vulkan::Vulkan
            Xinput
            winmm
            xaudio2
            ole32
    )

    target_link_libraries(MadnessEngine PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(MadnessEngine_game PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(MADNESSGAME PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(MadnessEngine_renderer PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(MADNESSRENDERER PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(CORE_UNIT_TEST PRIVATE ${MADNESS_WIN_LIBS})
endif ()
if (UNIX)
    SET(MADNESS_UNIX_LIBS Vulkan::Vulkan)
endif ()


#target_link_libraries(MadnessEngine glfw ${GLFW_LIBRARIES} VULKAN::VULKAN)


#target_compile_definitions(MadnessEngine PRIVATE
#        $<$<CONFIG:Debug>:DEBUG_BUILD>
#        $<$<CONFIG:Release>:RELEASE_BUILD>
#)


#equivalent of above
foreach(tgt IN LISTS MADNESS_BUILD_TARGETS)
    target_compile_definitions(${tgt} PRIVATE
            $<$<CONFIG:Debug>:DEBUG_BUILD>
            $<$<CONFIG:Release>:RELEASE_BUILD>
    )
endforeach()


# pretty sure the above does not work, if you want to check if we are in debug, refer to the examples below
#ifndef NDEBUG
#FATAL("WE DEBUGGING");
#else
#FATAL("WE RELEASING");
#endif


#Here because ill get .dll missing errors
# Force static linking approach for MinGW and only MinGW
if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif ()
