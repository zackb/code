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

add_executable(hottogo
    ${SRV_HEADERS}
    ${SRV_SOURCES}
)

target_include_directories(hottogo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(FetchContent)

# json-hpp
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3 # pick your preferred version
)

FetchContent_MakeAvailable(nlohmann_json)

# cpp-httplib
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.15.3 # or whatever tag or commit you prefer
)

FetchContent_MakeAvailable(httplib)

target_include_directories(hottogo PRIVATE ${httplib_SOURCE_DIR})

# CPR
# Force static CPR
set(CPR_BUILD_SHARED_LIBS OFF CACHE BOOL "Use static CPR" FORCE)
set(CPR_FORCE_USE_SYSTEM_CURL OFF CACHE BOOL "Use CPR's bundled curl" FORCE)

# Force static curl
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared libs" FORCE)
set(CURL_STATICLIB ON CACHE BOOL "Enable static curl" FORCE)
set(CURL_USE_OPENSSL ON CACHE BOOL "Use OpenSSL" FORCE)


FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG dd967cb48ea6bcbad9f1da5ada0db8ac0d532c06)
FetchContent_MakeAvailable(cpr)

target_link_libraries(hottogo PRIVATE cpr::cpr nlohmann_json::nlohmann_json)
