cmake_minimum_required(VERSION 3.20)
project(HyprWad)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/ext/imgui)

file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(hyprwad 
    src/main.cpp 
    src/ui.cpp 
    src/input.cpp 
    ${IMGUI_SOURCES})

target_include_directories(hyprwad PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(hyprwad
    ${SDL2_LIBRARIES}
    OpenGL::GL
)

