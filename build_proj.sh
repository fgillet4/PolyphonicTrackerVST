#!/bin/bash

# Text colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
CLEAN_BUILD=false
INSTALL_PLUGIN=false
VERBOSE=false

# Get system-specific VST3 directory
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    VST3_DIR="$HOME/.vst3"
else
    # Windows (assuming running in Git Bash or similar)
    VST3_DIR="$PROGRAMFILES/Common Files/VST3"
fi

# Function to print error and exit
error_exit() {
    echo -e "${RED}Error: $1${NC}" >&2
    exit 1
}

# Function to print status
print_status() {
    echo -e "${YELLOW}$1${NC}"
}

# Function to print usage
print_usage() {
    echo -e "${BLUE}Usage: $0 [options]${NC}"
    echo "Options:"
    echo "  -t, --type <Debug|Release>    Build type (default: Release)"
    echo "  -c, --clean                   Clean build directory before building"
    echo "  -i, --install                 Install plugin to system VST3 directory"
    echo "  -v, --verbose                 Verbose output"
    echo "  -h, --help                    Show this help message"
    exit 0
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -t|--type)
            BUILD_TYPE="$2"
            if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; then
                error_exit "Build type must be either Debug or Release"
            fi
            shift 2
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -i|--install)
            INSTALL_PLUGIN=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            print_usage
            ;;
        *)
            error_exit "Unknown option: $1"
            ;;
    esac
done

# Check if we're in the project root directory
if [ ! -f "CMakeLists.txt" ]; then
    error_exit "Please run this script from the project root directory"
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    print_status "Creating build directory..."
    mkdir build
fi

# Clean if requested
if [ "$CLEAN_BUILD" = true ]; then
    print_status "Cleaning build directory..."
    cd build || error_exit "Failed to enter build directory"
    rm -rf *
    cd ..
fi

# Enter build directory
cd build || error_exit "Failed to enter build directory"

# Configure CMake
print_status "Configuring CMake for ${BUILD_TYPE} build..."
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"

if [ "$VERBOSE" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=ON"
fi

cmake $CMAKE_ARGS .. || error_exit "CMake configuration failed"

# Build the project
print_status "Building project..."
if [ "$VERBOSE" = true ]; then
    cmake --build . --config ${BUILD_TYPE} -v || error_exit "Build failed"
else
    cmake --build . --config ${BUILD_TYPE} || error_exit "Build failed"
fi

# Install plugin if requested
if [ "$INSTALL_PLUGIN" = true ]; then
    print_status "Installing plugin to system VST3 directory..."
    
    # Create VST3 directory if it doesn't exist
    mkdir -p "$VST3_DIR"
    
    # Find the built plugin
    if [[ "$OSTYPE" == "darwin"* ]]; then
        PLUGIN_PATH="VST3/Polyphonic Tracker.vst3"
    else
        PLUGIN_PATH="VST3/PolyphonicTracker.vst3"
    fi
    
    if [ ! -d "$PLUGIN_PATH" ]; then
        error_exit "Could not find built plugin"
    fi
    
    # Copy plugin to VST3 directory
    cp -r "$PLUGIN_PATH" "$VST3_DIR/" || error_exit "Failed to install plugin"
    echo -e "${GREEN}Plugin installed to: $VST3_DIR${NC}"
fi

# Print success message
echo -e "${GREEN}Build completed successfully!${NC}"

# Print plugin location
echo -e "${YELLOW}Plugin can be found in:${NC}"
echo -e "${GREEN}$(pwd)/VST3/${NC}"

# Print build type
echo -e "${YELLOW}Build type:${NC} ${GREEN}${BUILD_TYPE}${NC}"