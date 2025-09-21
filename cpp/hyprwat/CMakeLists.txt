cmake_minimum_required(VERSION 3.20)
project(HyprWat)
set(CMAKE_CXX_STANDARD 20)

# OpenGL
find_package(OpenGL REQUIRED)

# SDL2
find_package(SDL2 REQUIRED)

# Fontconfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(Fontconfig REQUIRED fontconfig)
link_directories(${Fontconfig_LIBRARY_DIRS})


set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/ext/imgui)

file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(hyprwat
    src/main.cpp 
    src/ui.cpp 
    src/input.cpp 
    src/signal.cpp 
    src/font.cpp 
    src/selection/selector.cpp
    src/hyprland/hyprland_ipc.cpp
    ${IMGUI_SOURCES})

include_directories(${Fontconfig_INCLUDE_DIRS})

target_include_directories(hyprwat PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(hyprwat
    ${SDL2_LIBRARIES}
    OpenGL::GL
    ${Fontconfig_LIBRARIES}
)

# Install the executable
install(TARGETS hyprwat
        RUNTIME DESTINATION bin)

