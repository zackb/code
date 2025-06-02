file(GLOB_RECURSE COREX_HEADERS include/corex/*.h include/corex/*.hpp)
file(GLOB_RECURSE COREX_SOURCES src/*.cpp)

# Find the SDL2 package
find_package(SDL2 CONFIG REQUIRED)

# Find the SDL2_image package
find_package(SDL2_image CONFIG REQUIRED)

# Find the SDL2_mixer package
find_package(SDL2_mixer CONFIG REQUIRED)

# Find the SDL2_ttf package
find_package(SDL2_ttf CONFIG REQUIRED)

add_library(corex STATIC
    ${COREX_HEADERS}
    ${COREX_SOURCES}
)

target_include_directories(corex PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
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

message(STATUS "SDL2_IMAGE_INCLUDE_DIRS=${SDL2_IMAGE_INCLUDE_DIRS}")

