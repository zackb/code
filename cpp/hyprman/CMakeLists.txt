cmake_minimum_required(VERSION 3.20)

project(hyprman)
set(CMAKE_CXX_STANDARD 20)

Find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-4.0 REQUIRED IMPORTED_TARGET)

add_executable(hyprman
    src/main.cpp
    src/window.cpp
)

target_link_libraries(hyprman PkgConfig::GTKMM)

target_include_directories(hyprman PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# install executable
install(TARGETS hyprman
        RUNTIME DESTINATION bin)

