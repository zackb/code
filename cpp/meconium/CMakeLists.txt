cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Meconium)

set(CMAKE_CXX_STANDARD 17)

# Find the SDL2 package
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Specify the source files for your executable
set(MY_SRC main.cpp)

# Define the executable target
add_executable(meconium ${MY_SRC})

# Link the SDL2 libraries
target_link_libraries(meconium ${SDL2_LIBRARIES})

# Set the MacOSX bundle properties
set(CUR_TARGET meconium)
set_target_properties(${CUR_TARGET} PROPERTIES
    MACOSX_BUNDLE TRUE
    BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER net.jeedup.${CUR_TARGET}
    MACOSX_BUNDLE_BUNDLE_NAME ${CUR_TARGET}
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    # MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/customtemplate.plist.in
)

# Set the third-party frameworks and libraries to be bundled
set(MY_LIBRARIES_DIR "/opt/homebrew/opt/sdl2/lib")
set(MY_FRAMEWORKS_DIR "${CMAKE_BINARY_DIR}/${CUR_TARGET}.app/Contents/Frameworks")

# Create a "Frameworks" directory in the bundle
file(MAKE_DIRECTORY ${MY_FRAMEWORKS_DIR})

# Copy SDL2.dylib to the Frameworks folder in the app bundle
add_custom_command(TARGET ${CUR_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${MY_LIBRARIES_DIR}/libSDL2-2.0.0.dylib
        ${MY_FRAMEWORKS_DIR}/libSDL2-2.0.0.dylib
)

# Automatically adjust the library path using install_name_tool
add_custom_command(TARGET ${CUR_TARGET} POST_BUILD
    COMMAND install_name_tool -change
        /opt/homebrew/opt/sdl2/lib/libSDL2-2.0.0.dylib
        @executable_path/../Frameworks/libSDL2-2.0.0.dylib
        ${CMAKE_BINARY_DIR}/${CUR_TARGET}.app/Contents/MacOS/meconium
)
