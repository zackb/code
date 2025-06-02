file(GLOB_RECURSE GAME_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE GAME_SOURCES src/*.cpp)

#TODO: Remove SDL completely from here/game
# Find the SDL2 package
find_package(SDL2 REQUIRED)

# Find the SDL2_image package
find_package(SDL2_image REQUIRED)

# Find the SDL2_mixer package
find_package(SDL2_mixer REQUIRED)

# Find the SDL2_ttf package
find_package(SDL2_ttf REQUIRED)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)




add_executable(meconium
    ${GAME_HEADERS}
    ${GAME_SOURCES}
)

target_include_directories(meconium PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(meconium PRIVATE corex)

