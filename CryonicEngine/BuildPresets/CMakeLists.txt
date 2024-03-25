cmake_minimum_required(VERSION 3.15)

project(Game)

set(CMAKE_C_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)

add_library(raylib STATIC IMPORTED)
set_target_properties(raylib PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/Source/libraylib.a")

add_library(box2d STATIC IMPORTED)
set_target_properties(box2d PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/Source/box2d/libbox2d.a")

add_library(jolt STATIC IMPORTED)
set_target_properties(jolt PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/Source/Jolt/libJolt.a")

file(GLOB SOURCE_FILES
    Source/*.cpp
    Source/*.h
    Source/Scenes/*.cpp
    Source/Scenes/*.h
    Source/Components/*.cpp
    Source/Components/*.h
    Source/box2d/*.h
    Source/Jolt/*.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE -Werror=implicit-function-declaration)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib box2d jolt winmm)