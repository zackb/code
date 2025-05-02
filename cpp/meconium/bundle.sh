#!/bin/bash

mkdir -p Meconium.app/Contents/MacOS
mkdir -p Meconium.app/Contents/Frameworks
cp meconium Meconium.app/Contents/MacOS/
cp /opt/homebrew/opt/sdl2/lib/libSDL2-2.0.0.dylib Meconium.app/Contents/Frameworks/
install_name_tool -add_rpath @executable_path/../Frameworks Meconium.app/Contents/MacOS/meconium

