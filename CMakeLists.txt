cmake_minimum_required(VERSION 3.31)
project(MadnessEngine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_UNITY_BUILD TRUE) # UNITY BUILD


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
include_directories(MadnessEngine ./core/serialization/)
include_directories(MadnessEngine ./dsa)
include_directories(MadnessEngine ./platform)

include_directories(MadnessEngine ./renderer)

include_directories(MadnessEngine ./lib)
include_directories(MadnessEngine ./z_assets)

add_executable(MadnessEngine
        main.c
        unity_build.c
)

add_library(MADNESSDSA SHARED
        unity_build.c
)


#libs
if (MSVC)
    target_link_libraries(MadnessEngine PRIVATE Xinput)
    target_link_libraries(MadnessEngine PRIVATE winmm)
endif ()
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_link_libraries(MadnessEngine PRIVATE Xinput)
    target_link_libraries(MadnessEngine PRIVATE winmm)
endif ()
if (UNIX)
endif ()


# Add all header directories to include paths
target_include_directories(MadnessEngine PUBLIC ${INCLUDE_DIRS})
target_link_libraries(MadnessEngine PRIVATE
        Vulkan::Vulkan
        #glfw
)

#target_link_libraries(MadnessEngine glfw ${GLFW_LIBRARIES} VULKAN::VULKAN)

#target_include_directories(MadnessEngine PUBLIC src vendor platform)

target_compile_definitions(MadnessEngine PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
)