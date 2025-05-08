cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Meconium)

# Use our cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


# Use C++20 without any compiler specific extensions.
set(CMAKE_CXX_STANDARD 20 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# We don't use modules, so avoid using unrecognised compiler-specific commands that cmake produces when they are enabled.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Use LTO for Release builds only.
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# Find the SDL2 package
find_package(SDL2 REQUIRED)

# Find the SDL2_image package
find_package(SDL2_image REQUIRED)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)


# Specify the source files for your executable
set(SOURCES
    src/main.cpp
    src/Meconium.cpp
    src/ResourceManager.cpp
        src/ecs/Animation.cpp
        src/ecs/AnimationSystem.cpp
        src/ecs/MovementSystem.cpp
        src/ecs/RenderSystem.cpp
        src/ecs/CollisionSystem.cpp
        src/TileMap.cpp
)

# Define the executable target (use lowercase for the binary name)
set(EXECUTABLE_NAME "meconium")

# Create the executable
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Add includes
target_include_directories(${EXECUTABLE_NAME} PRIVATE include)

# Link the SDL2 libraries
target_link_libraries(${EXECUTABLE_NAME}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
)

# Set the MacOSX bundle properties (capitalize the app bundle name)
set(APP_BUNDLE_NAME "Meconium")
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")  # universal build
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER net.jeedup.${APP_BUNDLE_NAME}
    MACOSX_BUNDLE_BUNDLE_NAME ${APP_BUNDLE_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    RESOURCE "${CMAKE_SOURCE_DIR}/assets"
    # MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/customtemplate.plist.in
)

# Copy assets into the bundle
set(ASSETS_DESTINATION "${CMAKE_BINARY_DIR}/${APP_BUNDLE_NAME}.app/Contents/Resources/assets")

add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DESTINATION}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "${ASSETS_DESTINATION}"
    COMMENT "Copying assets to app bundle Resources folder"
)

# Set the third-party frameworks and libraries to be bundled
# set(MY_SDL2_LIB_DIR "/usr/local/opt/sdl2/lib")
# set(MY_SDL2_IMAGE_LIB_DIR "/usr/local/opt/sdl2_image/lib")
set(MY_SDL2_LIB_DIR "/opt/homebrew/opt/sdl2/lib")
set(MY_SDL2_IMAGE_LIB_DIR "/opt/homebrew/opt/sdl2_image/lib")
set(MY_FRAMEWORKS_DIR "${CMAKE_BINARY_DIR}/${APP_BUNDLE_NAME}.app/Contents/Frameworks")

# Create a "Frameworks" directory in the bundle
file(MAKE_DIRECTORY ${MY_FRAMEWORKS_DIR})

# Copy SDL2.dylib to the Frameworks folder in the app bundle
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${MY_SDL2_LIB_DIR}/libSDL2-2.0.0.dylib
        ${MY_FRAMEWORKS_DIR}/libSDL2-2.0.0.dylib
)

# Copy SDL2_image.dylib to the Frameworks folder in the app bundle
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${MY_SDL2_IMAGE_LIB_DIR}/libSDL2_image-2.0.0.dylib
        ${MY_FRAMEWORKS_DIR}/libSDL2_image-2.0.0.dylib
)

# Automatically adjust the library path using install_name_tool
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND install_name_tool -change
        ${MY_SDL2_LIB_DIR}/libSDL2-2.0.0.dylib
        @executable_path/../Frameworks/libSDL2-2.0.0.dylib
        ${CMAKE_BINARY_DIR}/${APP_BUNDLE_NAME}.app/Contents/MacOS/${EXECUTABLE_NAME}
)

# And for sdl2_image
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND install_name_tool -change
        ${MY_SDL2_IMAGE_LIB_DIR}/libSDL2_image-2.0.0.dylib
        @executable_path/../Frameworks/libSDL2_image-2.0.0.dylib
        ${CMAKE_BINARY_DIR}/${APP_BUNDLE_NAME}.app/Contents/MacOS/${EXECUTABLE_NAME}
)

# clean
add_custom_target(clean_bundle
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/Meconium.app
    COMMENT "Removing Meconium.app bundle"
)

# Define full_clean to run both: manual cleanup + regular clean
add_custom_target(full_clean
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build . --target clean_bundle
    COMMENT "Running full clean (build artifacts + .app bundle)"
)

