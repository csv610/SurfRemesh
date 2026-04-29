cmake_minimum_required(VERSION 3.16)
project(SurfRemesh LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(assimp QUIET)
if(NOT assimp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.4.2
    )
    FetchContent_MakeAvailable(assimp)
endif()

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/triangle.c)
    message(STATUS "Downloading Triangle library...")
    file(DOWNLOAD 
        https://people.math.sc.edu/Burkardt/c_src/triangle/triangle.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/triangle.c
        SHOW_PROGRESS
    )
    file(DOWNLOAD 
        https://people.math.sc.edu/Burkardt/c_src/triangle/triangle.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/triangle.h
        SHOW_PROGRESS
    )
endif()

add_library(triangle STATIC src/triangle.c)
target_compile_definitions(triangle PUBLIC TRILIBRARY)
target_include_directories(triangle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(triangle PRIVATE -w)

add_executable(surfremesh
    app/main.cpp
    src/SurfRemesh.cpp
    src/SurfRemesh.h
)
target_link_libraries(surfremesh PRIVATE triangle assimp::assimp)
target_include_directories(surfremesh PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(edge_stats
    app/edge_stats.cpp
    src/SurfRemesh.cpp
    src/SurfRemesh.h
)
target_link_libraries(edge_stats PRIVATE triangle assimp::assimp)
target_include_directories(edge_stats PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)