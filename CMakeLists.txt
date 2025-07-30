cmake_minimum_required(VERSION 3.31)
project(MadnessEngine C)

set(CMAKE_C_STANDARD 17)

set(CMAKE_UNITY_BUILD TRUE) # UNITY BUILD

# Set the root directory to search recursively
set(ROOT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Recursively find all directories
file(GLOB_RECURSE ALL_DIRS LIST_DIRECTORIES true "${ROOT_INCLUDE_DIR}/*")


add_executable(MadnessEngine

        engine/application.c
        engine/application.h
        engine/asserts.h
        engine/defines.h
        engine/logger.c
        engine/logger.h

        engine/platform.h
        engine/platform_linux.c
        engine/platform_win32.c

        engine/entry.c
        engine/entry.h
        engine/game.c
        engine/game.h
        engine/game_types.h

        engine/stb_image.h
        engine/stb_truetype.h
)

#TODO: how to make the engine a shared library
#second executable


# Filter only directories and add them
foreach (dir ${ALL_DIRS})
    if (IS_DIRECTORY "${dir}")
        target_include_directories(MadnessEngine PUBLIC ${dir})
        #target_include_directories(Game PUBLIC ${dir})
    endif ()
endforeach ()

#target_include_directories(MadnessEngine PUBLIC src vendor platform)

target_compile_definitions(MadnessEngine PRIVATE
        $<$<CONFIG:Debug>:DEBUG_BUILD>
        $<$<CONFIG:Release>:RELEASE_BUILD>
)