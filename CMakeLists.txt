cmake_minimum_required(VERSION 3.28)

project(clay_raylib_testing)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

# Header files
include_directories(
    ${PROJECT_SOURCE_DIR}/external/clay
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/themes
)

# Source files
file(GLOB_RECURSE SOURCES
    src/*cpp
)

add_executable(${PROJECT_NAME} ${SOURCES}
    src/main.cpp
    external/clay/clay.h
)

target_link_libraries(${PROJECT_NAME}
    raylib
)

# Linux dependencies
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME}
        m  # math
        pthread
        dl
        rt
        X11
    )
endif()