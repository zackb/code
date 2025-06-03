cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Corex)

# Use C++20 without any compiler specific extensions.
set(CMAKE_CXX_STANDARD 20 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use our cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# We don't use c++ modules, so avoid using unrecognised compiler-specific commands that cmake produces when they are enabled.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Use LTO for Release builds only.
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
add_subdirectory(corex)

add_subdirectory(meconium)

