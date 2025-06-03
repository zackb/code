file(GLOB_RECURSE COREX_HEADERS include/corex/*.h include/corex/*.hpp)
file(GLOB_RECURSE COREX_SOURCES src/*.cpp)

# SDL2
find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
    find_package(SDL2 REQUIRED)  # fall back to FindSDL2.cmake
endif()

# SDL2_image
find_package(SDL2_image CONFIG QUIET)
if(NOT SDL2_image_FOUND)
    find_package(SDL2_image REQUIRED)
endif()

# SDL2_mixer
find_package(SDL2_mixer CONFIG QUIET)
if(NOT SDL2_mixer_FOUND)
    find_package(SDL2_mixer REQUIRED)
endif()

# SDL2_ttf
find_package(SDL2_ttf CONFIG QUIET)
if(NOT SDL2_ttf_FOUND)
    find_package(SDL2_ttf REQUIRED)
endif()

add_library(corex STATIC
    ${COREX_HEADERS}
    ${COREX_SOURCES}
)

target_include_directories(corex PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
)

target_include_directories(corex PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(corex PUBLIC
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
)
