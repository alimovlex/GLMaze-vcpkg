cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

# Include custom Find modules (e.g. FindFreeImage)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

# Binary / project name
set(PROJECT_TARGETBIN "GLMaze")
project(${PROJECT_TARGETBIN} C CXX)

# Collect all .c and .h source files from src/
file(GLOB SRC
    "src/*.h"
    "src/*.c"
)

# ── Dependencies via vcpkg ────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)
find_package(SDL2   CONFIG REQUIRED)
find_package(GLEW   REQUIRED)

# FreeImage: vcpkg installs a CMake config; fall back to the module finder
find_package(freeimage CONFIG QUIET)
if(freeimage_FOUND)
    set(FREEIMAGE_TARGET freeimage::FreeImage)
    message(STATUS "FreeImage found via vcpkg config")
else()
    find_package(FreeImage REQUIRED)          # uses CMakeModules/FindFreeImage.cmake
    set(FREEIMAGE_TARGET ${FREEIMAGE_LIBRARY})
    message(STATUS "FreeImage found via module finder: ${FREEIMAGE_LIBRARY}")
endif()

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(${PROJECT_TARGETBIN} ${SRC})
set_target_properties(${PROJECT_TARGETBIN} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${PROJECT_TARGETBIN} PRIVATE
    ${GLEW_INCLUDE_DIRS}
)

if(NOT freeimage_FOUND)
    target_include_directories(${PROJECT_TARGETBIN} PRIVATE
        ${FREEIMAGE_INCLUDE_PATH}
    )
endif()

target_link_libraries(${PROJECT_TARGETBIN} PRIVATE
    OpenGL::GL
    SDL2::SDL2
    SDL2::SDL2main
    GLEW::GLEW
    ${FREEIMAGE_TARGET}
)

# Math library on Linux/Unix
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_TARGETBIN} PRIVATE m)
endif()

# ── Runtime output directory ───────────────────────────────────────────────────
if(UNIX AND NOT APPLE)
    set_target_properties(${PROJECT_TARGETBIN} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/GLMaze"
    )
endif()

# ── Copy resources next to executable after build ────────────────────────────
if(UNIX AND NOT APPLE)
    add_custom_command(TARGET ${PROJECT_TARGETBIN} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/res"
            "$<TARGET_FILE_DIR:${PROJECT_TARGETBIN}>"
        COMMENT "Copying res/ to output directory"
    )
endif()