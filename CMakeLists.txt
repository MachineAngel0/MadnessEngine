cmake_minimum_required(VERSION 3.31)
project(MadnessEngine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_UNITY_BUILD TRUE) # UNITY BUILD

set(MADNESS_BUILD_TARGETS
        MadnessEngine
        MadnessEngine_game
        MadnessEngine_renderer
        MADNESSRENDERER
)


# COMPILER FLAGS #
#Windows
if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /Wall /permissive- /Zi /fsanitize=address ")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /fsanitize=address")
endif ()

#GCC/MINGW OR CLANG
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wvla -Wfloat-equal ")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Waggregate-return -Wconversion") # additional compiler flags

    #LINUX
    #if(UNIX AND NOT CMAKE_C_COMPILER_ID STREQUAL "GNU") # for some reason gnu not working
    if (UNIX) # for some reason gnu not working
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
        set(CMAKE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    endif ()
endif ()


find_package(Vulkan REQUIRED)

include_directories(MadnessEngine ./core)
include_directories(MadnessEngine ./core/dsa)
include_directories(MadnessEngine ./core/platform)
include_directories(MadnessEngine ./core/serialization/)

include_directories(MadnessEngine ./renderer)
include_directories(MadnessEngine ./renderer/shaders)

include_directories(MadnessEngine ./game)

include_directories(MadnessEngine ./lib)
include_directories(MadnessEngine ./z_assets)


add_executable(MadnessEngine
        main.c
        unity_build.c
)

### GAME ###
add_executable(MadnessEngine_game
        entry_game.h
        unity_build.c
)
add_library(MADNESSGAME SHARED
        entry_game.h
        unity_build.c
)

### RENDERER ###

add_executable(MadnessEngine_renderer
        renderer/renderer_unity.c
        core/core_unity.c

)
add_library(MADNESSRENDERER SHARED
        renderer/renderer_unity.c
        core/core_unity.c

)

### EDITOR ###
add_executable(MadnessEngine_editor
        entry_editor.h
        unity_build.c
)

### CORE ###
add_executable(CORE_UNIT_TEST
        core/core_unit_test_main.c
        core/core_unity.c
)









#libs


if (WIN32)
    SET(MADNESS_WIN_LIBS Xinput winmm)

    target_link_libraries(MadnessEngine PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(MadnessEngine_game PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(MADNESSGAME PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(MadnessEngine_renderer PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(MADNESSRENDERER PRIVATE ${MADNESS_WIN_LIBS})
    target_link_libraries(CORE_UNIT_TEST PRIVATE ${MADNESS_WIN_LIBS})
endif ()
if (UNIX)
    SET(MADNESS_UNIX_LIBS Xinput winmm)


endif ()


# Add all header directories to include paths
target_link_libraries(MadnessEngine PRIVATE
        Vulkan::Vulkan
)
target_link_libraries(MadnessEngine_game PRIVATE
        Vulkan::Vulkan
)
target_link_libraries(MADNESSGAME PRIVATE
        Vulkan::Vulkan
)

target_link_libraries(MadnessEngine_renderer PRIVATE
        Vulkan::Vulkan
        #glfw
)
target_link_libraries(MADNESSRENDERER PRIVATE
        Vulkan::Vulkan
        #glfw
)


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



#Here because ill get .dll missing errors
# Force static linking approach for MinGW
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")