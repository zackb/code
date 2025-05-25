cmake_minimum_required(VERSION 3.15)
project(Excelsive)

set(CMAKE_CXX_STANDARD 20)

# Set path to libxl include and lib dirs
set(LIBXL_INCLUDE_DIR "/home/zack/Downloads/libxl-4.6.0/include_cpp")
set(LIBXL_LIBRARY_DIR "/home/zack/Downloads/libxl-4.6.0/lib-aarch64")

include_directories(${LIBXL_INCLUDE_DIR})
link_directories(${LIBXL_LIBRARY_DIR})

add_executable(Excelsive main.cpp)

target_link_libraries(Excelsive libxl.so)

