cmake_minimum_required(VERSION 3.16)
project(away LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# GLAD
add_library(glad STATIC
    external/glad/src/glad.c
)
target_include_directories(glad PUBLIC external/glad/include)

# Your app
add_executable(away
    src/main.cpp
)

target_include_directories(away PRIVATE
    external/glad/include
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(away
    glad
    ${SDL2_LIBRARIES}
    OpenGL::GL
)

# On Linux/macOS: link OpenGL
if(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(away OpenGL::GL)
endif()

# On Windows: may need opengl32 instead
if(WIN32)
    target_link_libraries(away opengl32)
endif()

