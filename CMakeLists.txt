cmake_minimum_required(VERSION 3.31)
project(MadnessEngine C)

set(CMAKE_C_STANDARD 17)

set(CMAKE_UNITY_BUILD TRUE) # UNITY BUILD

# Set the root directory to search recursively
set(ROOT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Recursively find all .h files
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Extract the directories of those header files
set(INCLUDE_DIRS "")
foreach(header ${HEADER_FILES})
        get_filename_component(header_dir ${header} DIRECTORY)
        list(APPEND INCLUDE_DIRS ${header_dir})
endforeach()

# Remove duplicates
list(REMOVE_DUPLICATES HEADER_DIRS)

add_executable(MadnessEngine

        engine/main.c
        engine/src/core/asserts.h
        engine/src/core/logger.c
        engine/src/core/logger.h

        engine/defines.h
        engine/platform/platform.h

)

#TODO: how to make the engine a shared library
#second executable


# Add all header directories to include paths
target_include_directories(MadnessEngine PUBLIC ${INCLUDE_DIRS})

#target_include_directories(MadnessEngine PUBLIC src vendor platform)

target_compile_definitions(MadnessEngine PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
)