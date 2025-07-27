cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(hottogo)

# Use C++20 without any compiler specific extensions.
set(CMAKE_CXX_STANDARD 20 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Use LTO for Release builds only.
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)


file(GLOB_RECURSE SRV_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE SRV_SOURCES src/*.cpp)

find_package(OpenSSL REQUIRED)

add_executable(hottogo
    ${SRV_HEADERS}
    ${SRV_SOURCES}
)

include(FetchContent)

# json-hpp
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(nlohmann_json)

# cpp-httplib
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.15.3
)

FetchContent_MakeAvailable(httplib)

# sqlite3
find_package(SQLite3 REQUIRED)

# ftxui
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v6.1.9
)

FetchContent_MakeAvailable(ftxui)

target_include_directories(hottogo PRIVATE 
    ${httplib_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(hottogo PRIVATE nlohmann_json::nlohmann_json OpenSSL::SSL OpenSSL::Crypto SQLite::SQLite3 ftxui::screen ftxui::dom ftxui::component)

