cmake_minimum_required(VERSION 3.15)
project(hyprwat CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Wayland QUIET COMPONENTS Client Egl)
if(NOT Wayland_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND REQUIRED IMPORTED_TARGET wayland-client wayland-egl)
endif()

find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)

# ImGui sources
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../ext/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Wayland protocol sources
set(WAYLAND_PROTOCOLS
    wlr-layer-shell-unstable-v1-client-protocol.c
    xdg-shell-client-protocol.c
)

# Main executable
add_executable(hyprwat
    main.cpp
    ${IMGUI_SOURCES}
    ${WAYLAND_PROTOCOLS}
)

# Include directories
target_include_directories(hyprwat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

if(Wayland_FOUND)
    target_link_libraries(hyprwat PRIVATE
        Wayland::Client
        Wayland::Egl
    )
else()
    target_link_libraries(hyprwat PRIVATE
        PkgConfig::WAYLAND
    )
endif()

target_link_libraries(hyprwat PRIVATE
    OpenGL::OpenGL
    OpenGL::EGL
    ${CMAKE_DL_LIBS}
    m
    pthread
)

# Optional: Set output directory
set_target_properties(hyprwat PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
