cmake_minimum_required(VERSION 3.20)

project(Mimic)
set(CMAKE_CXX_STANDARD 20)

# Find raylib
find_package(raylib REQUIRED)

add_executable(mimic
    src/mcamera.cpp
    src/tex.cpp
    src/main.cpp
)

target_link_libraries(mimic raylib)

target_include_directories(mimic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install the executable
install(TARGETS mimic
        RUNTIME DESTINATION bin)

