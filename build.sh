#!/bin/bash

# Text colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
CLEAN_BUILD=false
INSTALL_PLUGIN=false
VERBOSE=false
PLATFORM=""

# Function to print status
print_status() {
    echo -e "${YELLOW}$1${NC}"
}

# Function to print error and exit
error_exit() {
    echo -e "${RED}Error: $1${NC}" >&2
    exit 1
}

# Function to print platform selection menu
print_platform_menu() {
    echo -e "${BLUE}Available Platform Targets:${NC}"
    echo -e "${CYAN}Apple Silicon Mac${NC}"
    echo "  1) arm64 - Native M1/M2 build"
    echo "  2) arm64 - Universal Binary (arm64 + x86_64)"
    echo
    echo -e "${CYAN}Intel Mac${NC}"
    echo "  3) x86_64 - Native Intel build"
    echo "  4) x86_64 - Universal Binary (arm64 + x86_64)"
    echo
    echo -e "${CYAN}Windows${NC}"
    echo "  5) x86_64 - 64-bit Windows (requires MinGW or Visual Studio)"
    echo "  6) x86 - 32-bit Windows (requires MinGW or Visual Studio)"
    echo
    echo -e "${CYAN}Linux${NC}"
    echo "  7) x86_64 - 64-bit Linux"
    echo "  8) arm64 - ARM64 Linux"
    echo
    echo -e "${YELLOW}Enter platform number (1-8):${NC}"
}

# Function to set platform-specific configuration
configure_platform() {
    case $1 in
        1)  # Apple Silicon Native
            PLATFORM="apple-silicon"
            CMAKE_ARGS+=" -DCMAKE_OSX_ARCHITECTURES=arm64"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_PROCESSOR=arm64"
            ;;
        2)  # Apple Universal Binary
            PLATFORM="apple-universal"
            CMAKE_ARGS+=" -DCMAKE_OSX_ARCHITECTURES=arm64;x86_64"
            CMAKE_ARGS+=" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13"
            ;;
        3)  # Intel Mac Native
            PLATFORM="intel-mac"
            CMAKE_ARGS+=" -DCMAKE_OSX_ARCHITECTURES=x86_64"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_PROCESSOR=x86_64"
            ;;
        4)  # Intel Mac Universal
            PLATFORM="intel-universal"
            CMAKE_ARGS+=" -DCMAKE_OSX_ARCHITECTURES=arm64;x86_64"
            CMAKE_ARGS+=" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13"
            ;;
        5)  # Windows 64-bit
            PLATFORM="windows-x64"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_NAME=Windows"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_PROCESSOR=AMD64"
            ;;
        6)  # Windows 32-bit
            PLATFORM="windows-x86"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_NAME=Windows"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_PROCESSOR=x86"
            ;;
        7)  # Linux x86_64
            PLATFORM="linux-x64"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_NAME=Linux"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_PROCESSOR=x86_64"
            ;;
        8)  # Linux ARM64
            PLATFORM="linux-arm64"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_NAME=Linux"
            CMAKE_ARGS+=" -DCMAKE_SYSTEM_PROCESSOR=aarch64"
            ;;
        *)
            error_exit "Invalid platform selection"
            ;;
    esac
}

# Set platform-specific output paths
set_output_paths() {
    case $PLATFORM in
        apple-silicon|apple-universal|intel-mac|intel-universal)
            VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
            AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"
            BUILD_DIR="build-mac-$PLATFORM"
            ;;
        windows-*)
            VST3_DIR="$PROGRAMFILES/Common Files/VST3"
            BUILD_DIR="build-windows-$PLATFORM"
            ;;
        linux-*)
            VST3_DIR="$HOME/.vst3"
            BUILD_DIR="build-linux-$PLATFORM"
            ;;
    esac
}

# Function to verify build requirements
verify_requirements() {
    case $PLATFORM in
        windows-*)
            if ! command -v x86_64-w64-mingw32-g++ &> /dev/null && \
               ! command -v cl &> /dev/null; then
                error_exit "Windows build requires MinGW-w64 or Visual Studio"
            fi
            ;;
        linux-*)
            if [[ $PLATFORM == "linux-arm64" ]] && \
               ! command -v aarch64-linux-gnu-g++ &> /dev/null; then
                error_exit "ARM64 Linux build requires cross-compilation toolchain"
            fi
            ;;
    esac
}

# Function to print usage
print_usage() {
    echo -e "${BLUE}Usage: $0 [options]${NC}"
    echo "Options:"
    echo "  -t, --type <Debug|Release>    Build type (default: Release)"
    echo "  -c, --clean                   Clean build directory before building"
    echo "  -i, --install                 Install plugin to system directories"
    echo "  -v, --verbose                 Verbose output"
    echo "  -p, --platform <1-8>         Select platform target"
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
        -p|--platform)
            PLATFORM_SELECTION="$2"
            shift 2
            ;;
        -h|--help)
            print_usage
            ;;
        *)
            error_exit "Unknown option: $1"
            ;;
    esac
done

# Show platform menu if not specified
if [ -z "$PLATFORM_SELECTION" ]; then
    print_platform_menu
    read PLATFORM_SELECTION
fi

# Configure platform
configure_platform $PLATFORM_SELECTION
set_output_paths
verify_requirements

# Print build configuration
echo -e "\n${YELLOW}Build Configuration:${NC}"
echo -e "  Platform: ${GREEN}$PLATFORM${NC}"
echo -e "  Build Type: ${GREEN}$BUILD_TYPE${NC}"
echo -e "  Build Directory: ${GREEN}$BUILD_DIR${NC}"
echo -e "  Clean Build: ${GREEN}$CLEAN_BUILD${NC}"
echo -e "  Install Plugin: ${GREEN}$INSTALL_PLUGIN${NC}"
echo -e "  Verbose Output: ${GREEN}$VERBOSE${NC}\n"

# Ask for confirmation
read -p "Continue with build? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
fi

# Check if we're in the project root directory
if [ ! -f "CMakeLists.txt" ]; then
    error_exit "Please run this script from the project root directory"
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    print_status "Creating build directory: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
fi

# Clean if requested
if [ "$CLEAN_BUILD" = true ]; then
    print_status "Cleaning build directory..."
    rm -rf "${BUILD_DIR:?}"/*
fi

# Enter build directory
cd "$BUILD_DIR" || error_exit "Failed to enter build directory"

# Configure CMake with platform-specific options
print_status "Configuring CMake for ${PLATFORM} ${BUILD_TYPE} build..."

if [ "$VERBOSE" = true ]; then
    CMAKE_ARGS+=" -DCMAKE_VERBOSE_MAKEFILE=ON"
fi

# Add build type
CMAKE_ARGS+=" -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"

# Run CMake configuration
echo "cmake $CMAKE_ARGS .."
cmake $CMAKE_ARGS .. || error_exit "CMake configuration failed"

# Determine number of CPU cores for parallel build
if [[ "$OSTYPE" == "darwin"* ]]; then
    NUM_CORES=$(sysctl -n hw.ncpu)
else
    NUM_CORES=$(nproc)
fi

# Build the project
print_status "Building project using $NUM_CORES cores..."
if [ "$VERBOSE" = true ]; then
    cmake --build . --config "${BUILD_TYPE}" -j "${NUM_CORES}" -v || error_exit "Build failed"
else
    cmake --build . --config "${BUILD_TYPE}" -j "${NUM_CORES}" || error_exit "Build failed"
fi

# Install plugin if requested
if [ "$INSTALL_PLUGIN" = true ]; then
    print_status "Installing plugin to system directories..."
    
    case $PLATFORM in
        apple-silicon|apple-universal|intel-mac|intel-universal)
            # Create directories if they don't exist
            mkdir -p "$VST3_DIR"
            mkdir -p "$AU_DIR"
            
            # Install VST3
            VST3_PLUGIN="VST3/Polyphonic Tracker.vst3"
            if [ -d "$VST3_PLUGIN" ]; then
                cp -r "$VST3_PLUGIN" "$VST3_DIR/" || error_exit "Failed to install VST3"
                echo -e "${GREEN}VST3 installed to: $VST3_DIR${NC}"
            fi
            
            # Install AU if it exists
            AU_PLUGIN="AU/Polyphonic Tracker.component"
            if [ -d "$AU_PLUGIN" ]; then
                cp -r "$AU_PLUGIN" "$AU_DIR/" || error_exit "Failed to install AU"
                echo -e "${GREEN}AU installed to: $AU_DIR${NC}"
            fi
            ;;
        windows-*)
            print_status "Windows installation not yet implemented"
            ;;
        linux-*)
            mkdir -p "$VST3_DIR"
            VST3_PLUGIN="VST3/PolyphonicTracker.vst3"
            if [ -d "$VST3_PLUGIN" ]; then
                cp -r "$VST3_PLUGIN" "$VST3_DIR/" || error_exit "Failed to install VST3"
                echo -e "${GREEN}VST3 installed to: $VST3_DIR${NC}"
            fi
            ;;
    esac
fi

# Print success message
echo -e "\n${GREEN}Build completed successfully!${NC}"

# Print build information
echo -e "\n${YELLOW}Build Information:${NC}"
echo -e "  ${YELLOW}Platform:${NC} ${GREEN}${PLATFORM}${NC}"
echo -e "  ${YELLOW}Type:${NC} ${GREEN}${BUILD_TYPE}${NC}"
echo -e "  ${YELLOW}Directory:${NC} ${GREEN}$(pwd)${NC}"

# Print plugin locations
case $PLATFORM in
    apple-silicon|apple-universal|intel-mac|intel-universal)
        echo -e "  ${YELLOW}VST3 Location:${NC} ${GREEN}$VST3_DIR${NC}"
        echo -e "  ${YELLOW}AU Location:${NC} ${GREEN}$AU_DIR${NC}"
        ;;
    *)
        echo -e "  ${YELLOW}VST3 Location:${NC} ${GREEN}$VST3_DIR${NC}"
        ;;
esac

echo -e "\n${YELLOW}To rebuild, run:${NC}"
echo -e "${BLUE}./build.sh -p $PLATFORM_SELECTION${NC}"