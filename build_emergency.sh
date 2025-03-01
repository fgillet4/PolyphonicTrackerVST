#!/bin/bash
# Emergency build script with all debugging enabled

echo "Building EMERGENCY version with all debug features enabled..."

# Define build directory
BUILD_DIR="build-mac-apple-silicon"

# First, create the build directory if it doesn't exist
mkdir -p $BUILD_DIR

# Configure CMake with debug symbols and compiler flags
cd $BUILD_DIR
cmake -DCMAKE_OSX_ARCHITECTURES=arm64 \
      -DCMAKE_SYSTEM_PROCESSOR=arm64 \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-DDEBUG_LAYOUT -DDEBUG_COMPONENTS" ..

# Build with all cores
cmake --build . -- -j$(sysctl -n hw.ncpu)

# Tell user where to find the plugin
echo ""
echo "Emergency build complete!"
echo "VST3 plugin is located at: $(pwd)/PolyphonicTrackerVST_artefacts/Debug/VST3/PolyphonicTrackerVST.vst3"
echo ""
echo "To install to system location, run:"
echo "cp -r $(pwd)/PolyphonicTrackerVST_artefacts/Debug/VST3/PolyphonicTrackerVST.vst3 ~/Library/Audio/Plug-Ins/VST3/"
echo ""

cd ..
chmod +x build_emergency.sh