cmake_minimum_required(VERSION 3.16)
project(SurfRemesh LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include(CTest)

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

include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
)
FetchContent_MakeAvailable(catch2)

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

enable_testing()

add_executable(test_veclib
    tests/test_veclib.cpp
)
target_link_libraries(test_veclib PRIVATE Catch2::Catch2WithMain)
target_include_directories(test_veclib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${catch2_SOURCE_DIR}/extras
)
add_test(NAME veclib_tests COMMAND test_veclib)

add_executable(test_trilib
    tests/test_trilib.cpp
)
target_link_libraries(test_trilib PRIVATE triangle Catch2::Catch2WithMain)
target_include_directories(test_trilib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${catch2_SOURCE_DIR}/extras
)
add_test(NAME trilib_tests COMMAND test_trilib)

add_executable(test_edge
    tests/test_edge.cpp
    src/SurfRemesh.cpp
)
target_link_libraries(test_edge PRIVATE triangle assimp::assimp Catch2::Catch2WithMain)
target_include_directories(test_edge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${catch2_SOURCE_DIR}/extras
)
add_test(NAME edge_tests COMMAND test_edge)

add_executable(test_face
    tests/test_face.cpp
    src/SurfRemesh.cpp
)
target_link_libraries(test_face PRIVATE triangle assimp::assimp Catch2::Catch2WithMain)
target_include_directories(test_face PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${catch2_SOURCE_DIR}/extras
)
add_test(NAME face_tests COMMAND test_face)

add_executable(test_mesh
    tests/test_mesh.cpp
    src/SurfRemesh.cpp
)
target_link_libraries(test_mesh PRIVATE triangle assimp::assimp Catch2::Catch2WithMain)
target_include_directories(test_mesh PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${catch2_SOURCE_DIR}/extras
)
target_compile_definitions(test_mesh PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_test(NAME mesh_tests COMMAND test_mesh)