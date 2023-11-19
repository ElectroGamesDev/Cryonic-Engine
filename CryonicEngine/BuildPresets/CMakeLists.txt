cmake_minimum_required(VERSION 3.15)

project(Game)

set(CMAKE_C_STANDARD 11)
set(RAYLIB_PATH "${CMAKE_SOURCE_DIR}/ThirdParty/raylib")

add_subdirectory(${RAYLIB_PATH}/src)

file(GLOB SOURCE_FILES
    Source/*.cpp
    Source/*.h
    Source/Scenes/*.cpp
    Source/Scenes/*.h
    Source/Components/*.cpp
    Source/Components/*.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE -Werror=implicit-function-declaration)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)