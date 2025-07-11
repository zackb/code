file(GLOB_RECURSE GAME_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE GAME_SOURCES src/*.cpp)

# Define the executable target
if (APPLE)
    set(EXECUTABLE_NAME "Meconium")
else()
    set(EXECUTABLE_NAME "meconium")
endif()


add_executable(${EXECUTABLE_NAME}
    ${GAME_HEADERS}
    ${GAME_SOURCES}
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    corex
)

# Set the MacOSX bundle properties (capitalize the app bundle name)
if (APPLE)
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
    set(ASSETS_DESTINATION "${CMAKE_BINARY_DIR}/meconium/${APP_BUNDLE_NAME}.app/Contents/Resources/assets")

    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DESTINATION}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/meconium/assets"
            "${ASSETS_DESTINATION}"
        COMMENT "Copying assets to app bundle Resources folder"
    )

    # Detect Homebrew prefix
    if(EXISTS "/opt/homebrew")
        set(HOMEBREW_PREFIX "/opt/homebrew")
    elseif(EXISTS "/usr/local")
        set(HOMEBREW_PREFIX "/usr/local")
    else()
        message(FATAL_ERROR "Homebrew not found in standard locations")
    endif()

    # Set the third-party frameworks and libraries to be bundled
    set(MY_SDL2_LIB_DIR "${HOMEBREW_PREFIX}/opt/sdl2/lib")
    set(MY_SDL2_IMAGE_LIB_DIR "${HOMEBREW_PREFIX}/opt/sdl2_image/lib")
    set(MY_SDL2_MIXER_LIB_DIR "${HOMEBREW_PREFIX}/opt/sdl2_mixer/lib")
    set(MY_SDL2_TTF_LIB_DIR "${HOMEBREW_PREFIX}/opt/sdl2_ttf/lib")

    set(MY_FRAMEWORKS_DIR "${CMAKE_BINARY_DIR}/meconium/${APP_BUNDLE_NAME}.app/Contents/Frameworks")

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

    # Copy SDL2_mixer.dylib to the Frameworks folder in the app bundle
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${MY_SDL2_MIXER_LIB_DIR}/libSDL2_mixer-2.0.0.dylib
            ${MY_FRAMEWORKS_DIR}/libSDL2_mixer-2.0.0.dylib
    )

    # Copy SDL2_ttf.dylib to the Frameworks folder in the app bundle
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${MY_SDL2_TTF_LIB_DIR}/libSDL2_ttf-2.0.0.dylib
            ${MY_FRAMEWORKS_DIR}/libSDL2_ttf-2.0.0.dylib
    )

    # Automatically adjust the library path using install_name_tool
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND install_name_tool -change
            ${MY_SDL2_LIB_DIR}/libSDL2-2.0.0.dylib
            @executable_path/../Frameworks/libSDL2-2.0.0.dylib
            ${CMAKE_BINARY_DIR}/meconium/${APP_BUNDLE_NAME}.app/Contents/MacOS/${EXECUTABLE_NAME}
    )

    # And for sdl2_image
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND install_name_tool -change
            ${MY_SDL2_IMAGE_LIB_DIR}/libSDL2_image-2.0.0.dylib
            @executable_path/../Frameworks/libSDL2_image-2.0.0.dylib
            ${CMAKE_BINARY_DIR}/meconium/${APP_BUNDLE_NAME}.app/Contents/MacOS/${EXECUTABLE_NAME}
    )

    # And for sdl2_mixer
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND install_name_tool -change
            ${MY_SDL2_MIXER_LIB_DIR}/libSDL2_mixer-2.0.0.dylib
            @executable_path/../Frameworks/libSDL2_mixer-2.0.0.dylib
            ${CMAKE_BINARY_DIR}/meconium/${APP_BUNDLE_NAME}.app/Contents/MacOS/${EXECUTABLE_NAME}
    )

    # And for sdl2_ttf
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND install_name_tool -change
            ${MY_SDL2_TTF_LIB_DIR}/libSDL2_ttf-2.0.0.dylib
            @executable_path/../Frameworks/libSDL2_ttf-2.0.0.dylib
            ${CMAKE_BINARY_DIR}/meconium/${APP_BUNDLE_NAME}.app/Contents/MacOS/${EXECUTABLE_NAME}
    )

elseif(UNIX)

    # Install the executable to /usr/local/bin by default
    install(TARGETS ${EXECUTABLE_NAME}
            RUNTIME DESTINATION bin)

    # Install assets directory to /usr/local/share/meconium/assets
    install(DIRECTORY assets/
            DESTINATION share/meconium/assets)

    # Install man pages (assumes you have man/meconium.6 or similar)
    install(FILES man/meconium.6
            DESTINATION share/man/man6)

    # Generate .deb .rpm and .tgz
    include(InstallRequiredSystemLibraries)

    set(CPACK_PACKAGE_NAME "meconium")
    set(CPACK_PACKAGE_VERSION "0.1.0")
    set(CPACK_PACKAGE_CONTACT "zack@bartel.com")
    set(CPACK_GENERATOR "DEB;RPM;TGZ")

    # Required for DEB and RPM
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Zack Bartel")
    set(CPACK_RPM_PACKAGE_RELEASE 1)

    # Remove the "-Linux" suffix
    set(CPACK_SYSTEM_NAME "linux")

    include(CPack)


endif()

# clean
add_custom_target(clean_bundle
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/Meconium.app
    COMMENT "Removing Meconium.app bundle"
)

# clean packages
add_custom_target(clean_package
    COMMAND ${CMAKE_COMMAND} -E rm -f ../../meconium-*.tar.gz
    COMMAND ${CMAKE_COMMAND} -E rm -f ../../meconium-*.deb
    COMMAND ${CMAKE_COMMAND} -E rm -f ../../meconium-*.rpm
    COMMAND ${CMAKE_COMMAND} -E rm -rf ../../_CPack_Packages
    COMMENT "Removing CPack-generated packages and temporary directories"
)


# Define full_clean to run all three: manual cleanup + regular clean + package clean
add_custom_target(full_clean
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build . --target clean_bundle
    COMMAND ${CMAKE_COMMAND} --build . --target clean_package
    COMMENT "Running full clean (build artifacts + .app bundle)"
)

