cmake_minimum_required(VERSION 3.20)
project(Wandering CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.13.2
)
FetchContent_MakeAvailable(EnTT)

add_executable(wandering
    src/main.cpp
    src/game.cpp
    src/resources.cpp
    src/systems.cpp
    src/camera.cpp
    src/world.cpp
    src/debug.cpp
)

target_include_directories(wandering PRIVATE src)
target_link_libraries(wandering PRIVATE raylib nlohmann_json::nlohmann_json EnTT::EnTT)
