# .gitignore

```
# Build directories
build/
bin/
lib/
build-mac-apple-silicon/

# IDE specific files
.vscode/
.idea/
*.swp
.DS_Store

# CMake files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
install_manifest.txt

# Compiled Object files
*.slo
*.lo
*.o
*.obj

# Compiled Dynamic libraries
*.so
*.dylib
*.dll

# Compiled Static libraries
*.lai
*.la
*.a
*.lib

# Executables
*.exe
*.out
*.app

# VST specific
*.vst
*.vst3
*.component

# Backup files
*~
*.bak

```

# .gitmodules

```
[submodule "libs/JUCE"]
	path = libs/JUCE
	url = https://github.com/juce-framework/JUCE.git

```

# .vscode/settings.json

```json
{
    "files.associations": {
        "*.mm": "cpp",
        "__bits": "cpp",
        "__config": "cpp",
        "__debug": "cpp",
        "array": "cpp",
        "atomic": "cpp",
        "cmath": "cpp",
        "condition_variable": "cpp",
        "cstddef": "cpp",
        "cstring": "cpp",
        "ctime": "cpp",
        "future": "cpp",
        "iomanip": "cpp",
        "iostream": "cpp",
        "limits": "cpp",
        "list": "cpp",
        "map": "cpp",
        "memory": "cpp",
        "mutex": "cpp",
        "optional": "cpp",
        "queue": "cpp",
        "set": "cpp",
        "sstream": "cpp",
        "string_view": "cpp",
        "thread": "cpp",
        "type_traits": "cpp",
        "typeindex": "cpp",
        "unordered_map": "cpp",
        "unordered_set": "cpp",
        "variant": "cpp",
        "vector": "cpp",
        "__bit_reference": "cpp",
        "__errc": "cpp",
        "__hash_table": "cpp",
        "__locale": "cpp",
        "__mutex_base": "cpp",
        "__node_handle": "cpp",
        "__split_buffer": "cpp",
        "__threading_support": "cpp",
        "__tree": "cpp",
        "__tuple": "cpp",
        "__verbose_abort": "cpp",
        "bit": "cpp",
        "bitset": "cpp",
        "cctype": "cpp",
        "charconv": "cpp",
        "clocale": "cpp",
        "codecvt": "cpp",
        "complex": "cpp",
        "cstdarg": "cpp",
        "cstdint": "cpp",
        "cstdio": "cpp",
        "cstdlib": "cpp",
        "cwchar": "cpp",
        "cwctype": "cpp",
        "deque": "cpp",
        "exception": "cpp",
        "fstream": "cpp",
        "initializer_list": "cpp",
        "ios": "cpp",
        "iosfwd": "cpp",
        "istream": "cpp",
        "locale": "cpp",
        "new": "cpp",
        "ostream": "cpp",
        "ratio": "cpp",
        "regex": "cpp",
        "shared_mutex": "cpp",
        "stack": "cpp",
        "stdexcept": "cpp",
        "streambuf": "cpp",
        "string": "cpp",
        "system_error": "cpp",
        "tuple": "cpp",
        "typeinfo": "cpp",
        "__nullptr": "cpp",
        "__string": "cpp",
        "chrono": "cpp",
        "compare": "cpp",
        "concepts": "cpp",
        "numeric": "cpp",
        "algorithm": "cpp"
    }
}
```

# build_help.txt

```txt
1. Basic build (Release mode):

./build.sh

2. Debug build:

./build.sh -t Debug

3. Clean build in Release mode:

./build.sh -c

4. Debug build with installation:

./build.sh -t Debug -i

5. Verbose Release build with cleaning:

./build.sh -c -v

6. See all options:

bash./build.sh --help
```

# build_proj.sh

```sh
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
```

# build.sh

```sh
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
```

# capture_build.sh

```sh
#!/bin/bash

# Run the build command and capture both stdout and stderr into a temporary file
./build.sh -p 1 2>&1 | tee /tmp/build_output.log

# Filter the output to include only warnings and errors, then copy to clipboard
grep -E "warning:|error:" /tmp/build_output.log | pbcopy

# Clean up the temporary file
rm /tmp/build_output.log

# Notify the user
echo "Build output (warnings and errors) copied to clipboard!"
```

# CMakeLists.txt

```txt
cmake_minimum_required(VERSION 3.15)
project(PolyphonicTrackerVST VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory
add_subdirectory(libs/JUCE)

# JUCE project configuration
juce_add_plugin(PolyphonicTrackerVST
    VERSION "1.0.0"                             
    COMPANY_NAME "FrancisBrain"                   
    IS_SYNTH FALSE                               
    NEEDS_MIDI_OUTPUT TRUE                       
    NEEDS_MIDI_INPUT FALSE                       
    IS_MIDI_EFFECT FALSE                         
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE            
    COPY_PLUGIN_AFTER_BUILD TRUE                 
    PLUGIN_MANUFACTURER_CODE Frbr               
    PLUGIN_CODE Ptrc                             
    FORMATS VST3                              
    PRODUCT_NAME "Polyphonic Tracker"           
    DESCRIPTION "Real-time polyphonic pitch tracking VST"
    VST3_CATEGORIES "Analyzer"
)

# Create directory structure if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/dsp)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/midi)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/gui)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/utils)

# Add source files
target_sources(PolyphonicTrackerVST
    PRIVATE
        # Main plugin files
        source/PluginProcessor.cpp
        source/PluginEditor.cpp
        
        # DSP components
        source/dsp/FFTProcessor.cpp
        source/dsp/PitchDetector.cpp
        
        # MIDI components
        source/midi/MIDIManager.cpp

        # GUI components
        source/gui/SpectrogramComponent.cpp
)

# Add include directories
target_include_directories(PolyphonicTrackerVST
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# Link with required JUCE modules
target_link_libraries(PolyphonicTrackerVST
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_plugin_client
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

# Set Mac deployment target
if(APPLE)
    set_target_properties(PolyphonicTrackerVST PROPERTIES
        OSX_DEPLOYMENT_TARGET "10.13"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ""
    )
endif()

# Add optimization flags for release builds
if(CMAKE_BUILD_TYPE MATCHES Release)
    if(APPLE)
        # Mac-specific optimizations
        target_compile_options(PolyphonicTrackerVST PRIVATE
            -O3
            -flto
            -march=native
        )
    elseif(WIN32)
        # Windows-specific optimizations
        target_compile_options(PolyphonicTrackerVST PRIVATE
            /O2
            /GL
        )
    else()
        # Linux-specific optimizations
        target_compile_options(PolyphonicTrackerVST PRIVATE
            -O3
            -flto
            -march=native
        )
    endif()
endif()
```

# codebase_production.md

```md
# .gitignore

\`\`\`
# Build directories
build/
bin/
lib/
build-mac-apple-silicon/

# IDE specific files
.vscode/
.idea/
*.swp
.DS_Store

# CMake files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
install_manifest.txt

# Compiled Object files
*.slo
*.lo
*.o
*.obj

# Compiled Dynamic libraries
*.so
*.dylib
*.dll

# Compiled Static libraries
*.lai
*.la
*.a
*.lib

# Executables
*.exe
*.out
*.app

# VST specific
*.vst
*.vst3
*.component

# Backup files
*~
*.bak

\`\`\`

# .gitmodules

\`\`\`
[submodule "libs/JUCE"]
	path = libs/JUCE
	url = https://github.com/juce-framework/JUCE.git

\`\`\`

# .vscode/settings.json

\`\`\`json
{
    "files.associations": {
        "*.mm": "cpp",
        "__bits": "cpp",
        "__config": "cpp",
        "__debug": "cpp",
        "array": "cpp",
        "atomic": "cpp",
        "cmath": "cpp",
        "condition_variable": "cpp",
        "cstddef": "cpp",
        "cstring": "cpp",
        "ctime": "cpp",
        "future": "cpp",
        "iomanip": "cpp",
        "iostream": "cpp",
        "limits": "cpp",
        "list": "cpp",
        "map": "cpp",
        "memory": "cpp",
        "mutex": "cpp",
        "optional": "cpp",
        "queue": "cpp",
        "set": "cpp",
        "sstream": "cpp",
        "string_view": "cpp",
        "thread": "cpp",
        "type_traits": "cpp",
        "typeindex": "cpp",
        "unordered_map": "cpp",
        "unordered_set": "cpp",
        "variant": "cpp",
        "vector": "cpp",
        "__bit_reference": "cpp",
        "__errc": "cpp",
        "__hash_table": "cpp",
        "__locale": "cpp",
        "__mutex_base": "cpp",
        "__node_handle": "cpp",
        "__split_buffer": "cpp",
        "__threading_support": "cpp",
        "__tree": "cpp",
        "__tuple": "cpp",
        "__verbose_abort": "cpp",
        "bit": "cpp",
        "bitset": "cpp",
        "cctype": "cpp",
        "charconv": "cpp",
        "clocale": "cpp",
        "codecvt": "cpp",
        "complex": "cpp",
        "cstdarg": "cpp",
        "cstdint": "cpp",
        "cstdio": "cpp",
        "cstdlib": "cpp",
        "cwchar": "cpp",
        "cwctype": "cpp",
        "deque": "cpp",
        "exception": "cpp",
        "fstream": "cpp",
        "initializer_list": "cpp",
        "ios": "cpp",
        "iosfwd": "cpp",
        "istream": "cpp",
        "locale": "cpp",
        "new": "cpp",
        "ostream": "cpp",
        "ratio": "cpp",
        "regex": "cpp",
        "shared_mutex": "cpp",
        "stack": "cpp",
        "stdexcept": "cpp",
        "streambuf": "cpp",
        "string": "cpp",
        "system_error": "cpp",
        "tuple": "cpp",
        "typeinfo": "cpp",
        "__nullptr": "cpp",
        "__string": "cpp",
        "chrono": "cpp",
        "compare": "cpp",
        "concepts": "cpp",
        "numeric": "cpp",
        "algorithm": "cpp"
    }
}
\`\`\`

# build_help.txt

\`\`\`txt
1. Basic build (Release mode):

./build.sh

2. Debug build:

./build.sh -t Debug

3. Clean build in Release mode:

./build.sh -c

4. Debug build with installation:

./build.sh -t Debug -i

5. Verbose Release build with cleaning:

./build.sh -c -v

6. See all options:

bash./build.sh --help
\`\`\`

# build_proj.sh

\`\`\`sh
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
\`\`\`

# build.sh

\`\`\`sh
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
\`\`\`

# CMakeLists.txt

\`\`\`txt
cmake_minimum_required(VERSION 3.15)
project(PolyphonicTrackerVST VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory
add_subdirectory(libs/JUCE)

# JUCE project configuration
juce_add_plugin(PolyphonicTrackerVST
    VERSION "1.0.0"                             
    COMPANY_NAME "FrancisBrain"                   
    IS_SYNTH FALSE                               
    NEEDS_MIDI_OUTPUT TRUE                       
    NEEDS_MIDI_INPUT FALSE                       
    IS_MIDI_EFFECT FALSE                         
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE            
    COPY_PLUGIN_AFTER_BUILD TRUE                 
    PLUGIN_MANUFACTURER_CODE Frbr               
    PLUGIN_CODE Ptrc                             
    FORMATS VST3                              
    PRODUCT_NAME "Polyphonic Tracker"           
    DESCRIPTION "Real-time polyphonic pitch tracking VST"
    VST3_CATEGORIES "Analyzer"
)

# Create directory structure if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/dsp)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/midi)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/gui)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/utils)

# Add source files
target_sources(PolyphonicTrackerVST
    PRIVATE
        # Main plugin files
        source/PluginProcessor.cpp
        source/PluginEditor.cpp
        
        # DSP components
        source/dsp/FFTProcessor.cpp
        source/dsp/PitchDetector.cpp
        
        # MIDI components
        source/midi/MIDIManager.cpp

        # GUI components
        source/gui/SpectrogramComponent.cpp
)

# Add include directories
target_include_directories(PolyphonicTrackerVST
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# Link with required JUCE modules
target_link_libraries(PolyphonicTrackerVST
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_plugin_client
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

# Set Mac deployment target
if(APPLE)
    set_target_properties(PolyphonicTrackerVST PROPERTIES
        OSX_DEPLOYMENT_TARGET "10.13"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ""
    )
endif()

# Add optimization flags for release builds
if(CMAKE_BUILD_TYPE MATCHES Release)
    if(APPLE)
        # Mac-specific optimizations
        target_compile_options(PolyphonicTrackerVST PRIVATE
            -O3
            -flto
            -march=native
        )
    elseif(WIN32)
        # Windows-specific optimizations
        target_compile_options(PolyphonicTrackerVST PRIVATE
            /O2
            /GL
        )
    else()
        # Linux-specific optimizations
        target_compile_options(PolyphonicTrackerVST PRIVATE
            -O3
            -flto
            -march=native
        )
    endif()
endif()
\`\`\`

# docs/BUILDING.md

\`\`\`md

\`\`\`

# docs/CONTRIBUTING.md

\`\`\`md

\`\`\`

# docs/README.md

\`\`\`md

\`\`\`

# README.md

\`\`\`md
# Polyphonic Tracker VST

A VST plugin for real-time polyphonic pitch tracking, based on machine learning techniques.

## Features
- Real-time polyphonic pitch detection
- Instrument learning capability
- MIDI output
- Spectral analysis visualization
- Low latency processing

## Building
See [BUILDING.md](docs/BUILDING.md) for detailed build instructions.

## Contributing
Contributions are welcome! Please read [CONTRIBUTING.md](docs/CONTRIBUTING.md) for details on our code of conduct and the process for submitting pull requests.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

\`\`\`

# source/audio/AudioEngine.cpp

\`\`\`cpp

\`\`\`

# source/audio/AudioEngine.h

\`\`\`h

\`\`\`

# source/dsp/FFTProcessor.cpp

\`\`\`cpp
#include "FFTProcessor.h"

FFTProcessor::FFTProcessor(int fftSizeParam)
    : fftSize(fftSizeParam),
      spectrumSize(fftSizeParam / 2),
      overlapFactor(0.5f),
      inputBufferPos(0),
      fft(static_cast<int>(std::log2(static_cast<double>(fftSizeParam)))),
      window(static_cast<size_t>(fftSizeParam), juce::dsp::WindowingFunction<float>::hann)
{
    // Allocate memory for buffers
    inputBuffer.resize(static_cast<size_t>(fftSize), 0.0f);
    fftData.resize(static_cast<size_t>(fftSize * 2), 0.0f); // Complex data (real/imag pairs)
    magnitudeSpectrum.resize(static_cast<size_t>(spectrumSize), 0.0f);
}

FFTProcessor::~FFTProcessor()
{
}

bool FFTProcessor::processBlock(const float* inBuffer, int numSamples)
{
    bool fftPerformed = false;
    
    // Calculate hop size based on overlap factor
    int hopSize = static_cast<int>(fftSize * (1.0f - overlapFactor));
    if (hopSize < 1) hopSize = 1;
    
    // Add samples to the input buffer
    for (int i = 0; i < numSamples; ++i)
    {
        inputBuffer[static_cast<size_t>(inputBufferPos)] = inBuffer[i];
        inputBufferPos++;
        
        // If we have enough samples, perform the FFT
        if (inputBufferPos >= fftSize)
        {
            performFFT();
            
            // Shift the buffer by the hop size
            const int samplesToKeep = fftSize - hopSize;
            if (samplesToKeep > 0)
            {
                std::copy(inputBuffer.begin() + hopSize, inputBuffer.end(), inputBuffer.begin());
            }
            
            inputBufferPos = samplesToKeep;
            fftPerformed = true;
        }
    }
    
    return fftPerformed;
}

void FFTProcessor::performFFT()
{
    // Apply window function
    applyWindow();
    
    // Copy windowed input to FFT data array (real part)
    for (int i = 0; i < fftSize; ++i)
    {
        fftData[static_cast<size_t>(i * 2)] = inputBuffer[static_cast<size_t>(i)];
        fftData[static_cast<size_t>(i * 2 + 1)] = 0.0f; // Imaginary part is zero
    }
    
    // Perform the FFT
    fft.performRealOnlyForwardTransform(fftData.data(), true);
    
    // Calculate the magnitude spectrum
    for (int i = 0; i < spectrumSize; ++i)
    {
        float real = fftData[static_cast<size_t>(i * 2)];
        float imag = fftData[static_cast<size_t>(i * 2 + 1)];
        
        // Calculate magnitude (sqrt of real^2 + imag^2)
        magnitudeSpectrum[static_cast<size_t>(i)] = std::sqrt(real * real + imag * imag);
    }
    
    // Call the callback if registered
    if (spectrumCallback)
    {
        spectrumCallback(magnitudeSpectrum.data(), spectrumSize);
    }
}

void FFTProcessor::applyWindow()
{
    window.multiplyWithWindowingTable(inputBuffer.data(), static_cast<size_t>(fftSize));
}

const float* FFTProcessor::getMagnitudeSpectrum() const
{
    return magnitudeSpectrum.data();
}

int FFTProcessor::getSpectrumSize() const
{
    return spectrumSize;
}

int FFTProcessor::getFFTSize() const
{
    return fftSize;
}

void FFTProcessor::setOverlapFactor(float newOverlap)
{
    overlapFactor = juce::jlimit(0.0f, 0.95f, newOverlap);
}

void FFTProcessor::reset()
{
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(fftData.begin(), fftData.end(), 0.0f);
    std::fill(magnitudeSpectrum.begin(), magnitudeSpectrum.end(), 0.0f);
    inputBufferPos = 0;
}

void FFTProcessor::setSpectrumDataCallback(std::function<void(const float*, int)> callback)
{
    spectrumCallback = callback;
}
\`\`\`

# source/dsp/FFTProcessor.h

\`\`\`h
#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * FFTProcessor handles the FFT analysis of incoming audio.
 * It applies windowing, performs FFT, and calculates the magnitude spectrum.
 */
class FFTProcessor
{
public:
    /**
     * Constructor, initializes FFT with a specific size
     * @param fftSize The size of the FFT (must be a power of 2)
     */
    FFTProcessor(int fftSize = 4096);
    
    /**
     * Destructor
     */
    ~FFTProcessor();
    
    /**
     * Processes a block of audio and updates the FFT if enough samples are collected
     * @param inBuffer Audio input buffer
     * @param numSamples Number of samples in the buffer
     * @return True if FFT was performed, false otherwise
     */
    bool processBlock(const float* inBuffer, int numSamples);
    
    /**
     * Gets the current FFT magnitude spectrum
     * @return Pointer to the spectrum data
     */
    const float* getMagnitudeSpectrum() const;
    
    /**
     * Gets the size of the spectrum (fftSize / 2)
     * @return Size of the magnitude spectrum array
     */
    int getSpectrumSize() const;
    
    /**
     * Gets the current FFT size
     * @return Current FFT size
     */
    int getFFTSize() const;
    
    /**
     * Sets the overlap factor for the FFT processing
     * @param newOverlap Overlap factor (0.0 to 0.95)
     */
    void setOverlapFactor(float newOverlap);
    
    /**
     * Resets the FFT processor, clearing all buffers
     */
    void reset();
    
    /**
     * Registers a callback function to be called when new FFT data is available
     * @param callback Function to call with new spectrum data
     */
    void setSpectrumDataCallback(std::function<void(const float*, int)> callback);
    
private:
    int fftSize;
    int spectrumSize;
    float overlapFactor;
    
    std::vector<float> inputBuffer;
    int inputBufferPos;
    
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    
    std::vector<float> fftData;
    std::vector<float> magnitudeSpectrum;
    
    std::function<void(const float*, int)> spectrumCallback;
    
    void performFFT();
    void applyWindow();
};
\`\`\`

# source/dsp/PitchDetector.cpp

\`\`\`cpp
#include "PitchDetector.h"

PitchDetector::PitchDetector(int maxNotes)
    : learningModeActive(false),
      currentLearningNote(-1),
      maxPolyphony(maxNotes),
      requiredSpectraForLearning(10),
      instrumentType(InstrumentType::Generic),
      currentGuitarString(0),
      currentGuitarFret(0)
{
    // Set default guitar settings
    guitarSettings.openStringMidiNotes = {40, 45, 50, 55, 59, 64}; // E2, A2, D3, G3, B3, E4
    guitarSettings.numFrets = 24;
}

PitchDetector::~PitchDetector()
{
}

void PitchDetector::setLearningModeActive(bool shouldBeActive)
{
    learningModeActive = shouldBeActive;
}

bool PitchDetector::isLearningModeActive() const
{
    return learningModeActive;
}

void PitchDetector::setCurrentLearningNote(int midiNote)
{
    currentLearningNote = midiNote;
}

int PitchDetector::getCurrentLearningNote() const
{
    return currentLearningNote;
}

std::vector<int> PitchDetector::processSpectrum(const float* spectrum, int spectrumSize)
{
    std::vector<int> detectedNotes;
    
    // Convert input spectrum to vector
    std::vector<float> spectrumVec(spectrum, spectrum + spectrumSize);
    normalizeVector(spectrumVec);
    
    if (learningModeActive && currentLearningNote >= 0)
    {
        // Learning mode: store the spectrum for the current note
        addLearnedSpectrum(spectrum, spectrumSize, currentLearningNote);
        return detectedNotes; // Return empty vector in learning mode
    }
    else if (isReadyForDetection())
    {
        // Detection mode: perform polyphonic pitch detection
        detectedNotes = detectPolyphonicPitches(spectrum, spectrumSize);
        
        // Call the callback if registered
        if (noteCallback && !detectedNotes.empty())
        {
            noteCallback(detectedNotes);
        }
    }
    
    return detectedNotes;
}

void PitchDetector::addLearnedSpectrum(const float* spectrumData, int spectrumSize, int midiNote)
{
    // Create a vector from the spectrum data
    std::vector<float> spectrumVec(spectrumData, spectrumData + spectrumSize);
    normalizeVector(spectrumVec);
    
    // Add to the learned spectra for this note
    learnedSpectraPerNote[midiNote].push_back(spectrumVec);
    
    // If we have enough spectra for this note, create an average profile
    if (static_cast<int>(learnedSpectraPerNote[midiNote].size()) >= requiredSpectraForLearning)
    {
        // Calculate the average spectrum for this note
        std::vector<float> avgSpectrum(static_cast<size_t>(spectrumSize), 0.0f);        
        for (const auto& learnedSpectrum : learnedSpectraPerNote[midiNote])
        {
            for (int i = 0; i < spectrumSize; ++i)
            {
                avgSpectrum[static_cast<size_t>(i)] += learnedSpectrum[static_cast<size_t>(i)];            }
        }
        
        // Normalize the average
        for (float& value : avgSpectrum)
        {
            value /= learnedSpectraPerNote[midiNote].size();
        }
        
        normalizeVector(avgSpectrum);
        
        // Store as a learned profile
        SpectralProfile profile;
        profile.midiNote = midiNote;
        profile.spectrum = avgSpectrum;
        profile.noteName = midiNoteToName(midiNote);
        
        // Remove any existing profile for this note
        learnedProfiles.erase(
            std::remove_if(learnedProfiles.begin(), learnedProfiles.end(),
                          [midiNote](const SpectralProfile& p) { return p.midiNote == midiNote; }),
            learnedProfiles.end());
        
        // Add the new profile
        learnedProfiles.push_back(profile);
    }
}

std::vector<int> PitchDetector::detectPolyphonicPitches(const float* spectrum, int spectrumSize)
{
    if (learnedProfiles.empty())
    {
        return {};
    }
    
    // Convert input spectrum to vector
    std::vector<float> inputSpectrum(spectrum, spectrum + spectrumSize);
    normalizeVector(inputSpectrum);
    
    // Perform sparse encoding to find the most similar learned profiles
    std::vector<float> coefficients = sparseEncode(inputSpectrum);
    
    // Sort the coefficients and find the indices of the top values
    std::vector<std::pair<float, int>> coefPairs;
    for (size_t i = 0; i < coefficients.size(); ++i)
    {
        coefPairs.emplace_back(coefficients[i], static_cast<int>(i));
    }
    
    // Sort in descending order of coefficient values
    std::sort(coefPairs.begin(), coefPairs.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select the top notes, up to maxPolyphony
    std::vector<int> detectedNotes;
    
    for (int i = 0; i < std::min(maxPolyphony, static_cast<int>(coefPairs.size())); ++i)
    {
        // Only include notes with coefficients above the threshold
        if (coefPairs[static_cast<size_t>(i)].first >= minimumCoefficient)
        {
            int profileIndex = coefPairs[static_cast<size_t>(i)].second;
            int midiNote = learnedProfiles[static_cast<size_t>(profileIndex)].midiNote;
            
            // Check for octave errors or close notes (avoid duplicates)
            bool tooClose = false;
            for (int existingNote : detectedNotes)
            {
                int semitoneDistance = std::abs(existingNote - midiNote);
                if (semitoneDistance < maximumSemitoneDistance)
                {
                    tooClose = true;
                    break;
                }
            }
            
            if (!tooClose)
            {
                detectedNotes.push_back(midiNote);
            }
        }
    }
    
    return detectedNotes;
}

void PitchDetector::normalizeVector(std::vector<float>& vec)
{
    // Calculate the L2 norm (Euclidean length) of the vector
    float sumSquares = 0.0f;
    for (float val : vec)
    {
        sumSquares += val * val;
    }
    
    if (sumSquares > 0.0f)
    {
        float norm = std::sqrt(sumSquares);
        for (float& val : vec)
        {
            val /= norm;
        }
    }
}

std::vector<float> PitchDetector::sparseEncode(const std::vector<float>& input)
{
    // Simple implementation of sparse encoding using cosine similarity
    // In a more advanced implementation, this would use an L1-regularized solver
    
    std::vector<float> coefficients;
    
    for (const auto& profile : learnedProfiles)
    {
        // Calculate the dot product (cosine similarity for normalized vectors)
        float similarity = 0.0f;
        for (size_t i = 0; i < std::min(input.size(), profile.spectrum.size()); ++i)
        {
            similarity += input[i] * profile.spectrum[i];
        }
        
        coefficients.push_back(similarity);
    }
    
    return coefficients;
}

bool PitchDetector::saveInstrumentData(const juce::String& filePath)
{
    // Use simpler file-based approach for now
    juce::File file(filePath);
    juce::FileOutputStream outStream(file);
    
    if (!outStream.openedOk())
        return false;
    
    // Write the number of profiles
    outStream.writeInt(static_cast<int>(learnedProfiles.size()));
    
    for (const auto& profile : learnedProfiles)
    {
        // Write MIDI note
        outStream.writeInt(profile.midiNote);
        
        // Write note name length and string
        outStream.writeInt(static_cast<int>(profile.noteName.length()));        
        // Write spectrum size and data
        outStream.writeInt(static_cast<int>(profile.spectrum.size()));
        for (float val : profile.spectrum)
        {
            outStream.writeFloat(val);
        }
    }
    
    return true;
}

bool PitchDetector::loadInstrumentData(const juce::String& filePath)
{
    juce::File file(filePath);
    juce::FileInputStream inStream(file);
    
    if (!inStream.openedOk())
        return false;
    
    clearInstrumentData();
    
    // Read number of profiles
    int numProfiles = inStream.readInt();
    
    for (int i = 0; i < numProfiles; ++i)
    {
        SpectralProfile profile;
        
        // Read MIDI note
        profile.midiNote = inStream.readInt();
        
        // Read note name
        
        // Read spectrum
        int spectrumSize = inStream.readInt();
        profile.spectrum.resize(static_cast<size_t>(spectrumSize));        
        for (int j = 0; j < spectrumSize; ++j)
        {
            profile.spectrum[static_cast<size_t>(j)] = inStream.readFloat();        }
        
        learnedProfiles.push_back(profile);
    }
    
    return true;
}

void PitchDetector::clearInstrumentData()
{
    learnedProfiles.clear();
    learnedSpectraPerNote.clear();
}

void PitchDetector::setMaxPolyphony(int maxNotes)
{
    maxPolyphony = maxNotes;
}

int PitchDetector::getMaxPolyphony() const
{
    return maxPolyphony;
}

bool PitchDetector::isReadyForDetection() const
{
    return !learnedProfiles.empty();
}

void PitchDetector::setNoteDetectionCallback(std::function<void(const std::vector<int>&)> callback)
{
    noteCallback = callback;
}

std::string PitchDetector::midiNoteToName(int midiNote)
{
    static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    
    int octave = (midiNote / 12) - 1;
    int noteIndex = midiNote % 12;
    
    return std::string(noteNames[noteIndex]) + std::to_string(octave);
}
\`\`\`

# source/dsp/PitchDetector.h

\`\`\`h
#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <map>
#include <string>

/**
 * PitchDetector class implements polyphonic pitch detection using
 * machine learning techniques to analyze spectral information.
 */
class PitchDetector
{
public:
    /**
     * Constructor
     * @param maxNotes Maximum number of notes to detect at once
     */
    PitchDetector(int maxNotes = 6);
    
    /**
     * Destructor
     */
    ~PitchDetector();
    

    /**
     * Enum for learning mode instrument types
     */
    enum class InstrumentType {
        Generic,
        Guitar,
        Piano,
        Bass
    };
    /**
     * Struct for guitar settings
     */
    struct GuitarSettings {
        // Standard tuning (E, A, D, G, B, E)
        std::vector<int> openStringMidiNotes = {40, 45, 50, 55, 59, 64};
        int numFrets = 24;  // Maximum number of frets to learn
    };
    /**
     * Activates or deactivates learning mode
     * @param shouldBeActive True to enable learning mode, false to disable
     */
    void setLearningModeActive(bool shouldBeActive);
    
    /**
     * Checks if learning mode is active
     * @return True if learning mode is active
     */
    bool isLearningModeActive() const;
    /**
     * Sets the instrument type for learning mode
     * @param type The type of instrument being learned
     */
    void setInstrumentType(InstrumentType type);
    
    /**
     * Gets the current instrument type
     * @return Current instrument type
     */
    InstrumentType getInstrumentType() const;

    /**
     * Gets the current guitar settings
     * @return Current guitar settings
     */
    const GuitarSettings& getGuitarSettings() const;

    /**
     * Sets the guitar settings for guitar mode
     * @param settings Guitar configuration settings
     */
    void setGuitarSettings(const GuitarSettings& settings);

    /**
     * Sets the current monophonic note being learned (when in learning mode)
     * @param midiNote MIDI note number being learned
     */
    void setCurrentLearningNote(int midiNote);
    
    /**
     * Gets the current monophonic note being learned
     * @return Current MIDI note number, or -1 if not set
     */
    int getCurrentLearningNote() const;

    /**
     * Gets the current guitar position
     * @param stringIndex Output parameter for string index
     * @param fretNumber Output parameter for fret number
     */
    void getCurrentGuitarPosition(int& stringIndex, int& fretNumber) const;

    /**
     * Sets the current guitar string and fret for learning (guitar mode only)
     * @param stringIndex String index (0-5, where 0 is the low E string)
     * @param fret Fret number (0 for open string)
     * @return The corresponding MIDI note number
     */
    int setCurrentGuitarPosition(int stringIndex, int fret);
    /**
     * Process a new spectrum for pitch detection or learning
     * @param spectrum Pointer to the magnitude spectrum data
     * @param spectrumSize Size of the spectrum data
     * @return Vector of detected MIDI notes (when not in learning mode)
     */
    std::vector<int> processSpectrum(const float* spectrum, int spectrumSize);
    
    /**
     * Saves learned instrument data to a file
     * @param filePath Path to save the data
     * @return True if successful, false otherwise
     */
    bool saveInstrumentData(const juce::String& filePath);
    
    /**
     * Loads instrument data from a file
     * @param filePath Path to the data file
     * @return True if successful, false otherwise
     */
    bool loadInstrumentData(const juce::String& filePath);
    
    /**
     * Clears all learned instrument data
     */
    void clearInstrumentData();
    
    /**
     * Sets the maximum number of simultaneous notes to detect
     * @param maxNotes New maximum number of notes
     */
    void setMaxPolyphony(int maxNotes);
    
    /**
     * Gets the maximum number of simultaneous notes
     * @return Maximum number of notes
     */
    int getMaxPolyphony() const;
    
    /**
     * Checks if enough data is available for pitch detection
     * @return True if enough data is learned
     */
    bool isReadyForDetection() const;
    
    /**
     * Registers a callback to be called when new notes are detected
     * @param callback Function to call with detected notes
     */
    void setNoteDetectionCallback(std::function<void(const std::vector<int>&)> callback);
    
private:
    struct SpectralProfile {
        int midiNote;
        std::vector<float> spectrum;
        std::string noteName;

        // Guitar-specific information (if applicable)
        int guitarString = -1;
        int guitarFret = -1;
    };
    
    bool learningModeActive;
    int currentLearningNote;
    int maxPolyphony;
    int requiredSpectraForLearning;

    // Instrument type and settings
    InstrumentType instrumentType;
    GuitarSettings guitarSettings;
    
    // Current guitar position (for guitar mode)
    int currentGuitarString;
    int currentGuitarFret;
    
    std::vector<SpectralProfile> learnedProfiles;
    std::map<int, std::vector<std::vector<float>>> learnedSpectraPerNote;
    
    std::function<void(const std::vector<int>&)> noteCallback;
    
    // Methods for spectrum processing and analysis
    std::vector<int> detectPolyphonicPitches(const float* spectrum, int spectrumSize);
    void addLearnedSpectrum(const float* spectrum, int spectrumSize, int midiNote);
    void normalizeVector(std::vector<float>& vec);
    std::vector<float> sparseEncode(const std::vector<float>& input);
    std::string midiNoteToName(int midiNote);
    
    // Constants for pitch detection
    const float minimumCoefficient = 0.1f;   // Minimum coefficient for a note to be detected
    const int maximumSemitoneDistance = 2;   // Maximum semitone distance for note filtering
};
\`\`\`

# source/dsp/SpectralAnalyzer.cpp

\`\`\`cpp

\`\`\`

# source/dsp/SpectralAnalyzer.h

\`\`\`h

\`\`\`

# source/gui/LearningModeComponent.cpp

\`\`\`cpp

\`\`\`

# source/gui/LearningModeComponent.h

\`\`\`h

\`\`\`

# source/gui/MainComponent.cpp

\`\`\`cpp

\`\`\`

# source/gui/MainComponent.h

\`\`\`h

\`\`\`

# source/gui/SpectrogramComponent.cpp

\`\`\`cpp
#include "SpectrogramComponent.h"

SpectrogramComponent::SpectrogramComponent()
    : threshold(0.01f),
      useLogFrequency(true),
      showThreshold(true),
      sampleRate(44100.0f)
{
    // Initialize FFT data
    fftData.resize(kMaxFFTSize, 0.0f);
    peakData.resize(kMaxFFTSize, 0.0f);
    
    // Initialize frequency scale
    for (size_t i = 1; i < fftData.size(); ++i)
    {
        float prevFreq = frequencyScale[static_cast<size_t>(i - 1)];
        float freq = frequencyScale[i];
        float proportion = static_cast<float>(i) / static_cast<float>(kMaxFFTSize - 1);
        frequencyScale[i] = (useLogFrequency)
            ? kMinFrequency * std::pow(kMaxFrequency / kMinFrequency, proportion)
            : kMinFrequency + (kMaxFrequency - kMinFrequency) * proportion;
    }
    
    // Start the timer for updates
    startTimer(kRefreshRateMs);
}

SpectrogramComponent::~SpectrogramComponent()
{
    stopTimer();
}

void SpectrogramComponent::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colours::black);
    
    // Get component dimensions
    int width = getWidth();
    int height = getHeight();
    
    // Draw grid lines and frequency labels
    g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
    
    // Octave grid lines (if using log scale)
    if (useLogFrequency)
    {
        for (int freq = static_cast<int>(kMinFrequency); freq <= kMaxFrequency; freq *= 2)
        {
            float x = frequencyToX(static_cast<float>(freq));
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
            
            // Draw frequency label
            g.setColour(juce::Colours::grey);
            juce::String label = (freq >= 1000) 
                ? juce::String(freq / 1000) + "kHz" 
                : juce::String(freq) + "Hz";
            g.drawText(label, static_cast<int>(x) - 20, height - 20, 40, 20, 
                      juce::Justification::centred, false);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        }
    }
    else
    {
        // Linear frequency grid
        for (int freq = 0; freq <= kMaxFrequency; freq += 1000)
        {
            float x = frequencyToX(static_cast<float>(freq));
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
            
            // Draw frequency label
            g.setColour(juce::Colours::grey);
            juce::String label = (freq >= 1000) 
                ? juce::String(freq / 1000) + "kHz" 
                : juce::String(freq) + "Hz";
            g.drawText(label, static_cast<int>(x) - 20, height - 20, 40, 20, 
                      juce::Justification::centred, false);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        }
    }
    
    // Amplitude grid lines
    for (float amp = 0.0f; amp <= 1.0f; amp += 0.2f)
    {
        float y = amplitudeToY(amp);
        g.drawHorizontalLine(static_cast<int>(y), 0.0f, static_cast<float>(width));
        
        // Draw amplitude label
        g.setColour(juce::Colours::grey);
        juce::String label = juce::String(static_cast<int>(amp * 100)) + "%";
        g.drawText(label, 5, static_cast<int>(y) - 10, 40, 20, 
                  juce::Justification::left, false);
        g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
    }
    
    // Draw threshold line if enabled
    if (showThreshold)
    {
        g.setColour(juce::Colours::red.withAlpha(0.5f));
        float thresholdY = amplitudeToY(threshold);
        g.drawHorizontalLine(static_cast<int>(thresholdY), 0.0f, static_cast<float>(width));
    }
    
    // Draw waterfall history
    {
        int historySize = static_cast<int>(history.size());
        float historyAlphaStep = 0.8f / static_cast<float>(historySize);
        
        for (int h = 0; h < historySize; ++h)
        {
            float alpha = 0.8f - h * historyAlphaStep;
            const auto& historyData = history[static_cast<size_t>(h)];
            
            // Draw spectrum as a series of vertical lines
            for (int i = 1; i < static_cast<int>(historyData.size()); ++i)
            {
                float prevFreq = frequencyScale[static_cast<size_t>(i - 1)];
                float freq = frequencyScale[static_cast<size_t>(i)];
                
                float prevAmp = historyData[static_cast<size_t>(i - 1)];
                float amp = historyData[static_cast<size_t>(i)];
                
                float x1 = frequencyToX(prevFreq);
                float y1 = amplitudeToY(prevAmp);
                float x2 = frequencyToX(freq);
                float y2 = amplitudeToY(amp);
                
                // Color based on amplitude
                juce::Colour color;
                if (amp > threshold)
                {
                    // Above threshold - gradient from yellow to red
                    color = juce::Colour::fromHSV(0.1f - (amp - threshold) * 0.3f, 0.8f, 1.0f, alpha);
                }
                else
                {
                    // Below threshold - blue
                    color = juce::Colours::blue.withAlpha(alpha * 0.5f);
                }
                
                g.setColour(color);
                g.drawLine(x1, y1, x2, y2, 1.0f);
            }
        }
    }
    
    // Draw current spectrum
    {
        // Draw spectrum as a series of vertical lines
        for (int i = 1; i < static_cast<int>(fftData.size()); ++i)
        {
            float prevFreq = frequencyScale[i - 1];
            float freq = frequencyScale[i];
            
            float prevAmp = fftData[static_cast<size_t>(i - 1)];
            float amp = fftData[static_cast<size_t>(i)];
            
            float x1 = frequencyToX(prevFreq);
            float y1 = amplitudeToY(prevAmp);
            float x2 = frequencyToX(freq);
            float y2 = amplitudeToY(amp);
            
            // Color based on amplitude
            juce::Colour color;
            if (amp > threshold)
            {
                // Above threshold - gradient from yellow to red
                color = juce::Colour::fromHSV(0.1f - (amp - threshold) * 0.3f, 0.8f, 1.0f, 1.0f);
            }
            else
            {
                // Below threshold - blue
                color = juce::Colours::blue.withAlpha(0.5f);
            }
            
            g.setColour(color);
            g.drawLine(x1, y1, x2, y2, 2.0f);
        }
    }
    
    // Draw marked frequencies
    for (const auto& markedFreq : markedFrequencies)
    {
        float x = frequencyToX(markedFreq.frequency);
        
        if (markedFreq.active)
        {
            // Active frequency - draw as a vertical green line
            g.setColour(juce::Colours::lime);
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
            
            // Draw a label
            g.drawText(juce::String(markedFreq.frequency, 1) + "Hz",
                       static_cast<int>(x) - 30, 5, 60, 20,
                       juce::Justification::centred, false);
        }
        else
        {
            // Inactive frequency - draw as a vertical grey line
            g.setColour(juce::Colours::grey);
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
        }
    }
}

void SpectrogramComponent::resized()
{
    // Nothing to do here
}

void SpectrogramComponent::updateFFT(const float* newFFTData, int size)
{
    if (size > kMaxFFTSize)
        size = kMaxFFTSize;
    
    // Copy the new data
    for (int i = 0; i < size; ++i)
    {
        fftData[static_cast<size_t>(i)] = newFFTData[i];
        
        // Update peak data with decay
        peakData[static_cast<size_t>(i)] = std::max(peakData[static_cast<size_t>(i)] * 0.95f, newFFTData[i]);
    }
    
    // Add current data to history
    history.push_front(fftData);
    
    // Limit history size
    while (history.size() > kHistorySize)
    {
        history.pop_back();
    }
    
    // Request a repaint
    repaint();
}

void SpectrogramComponent::setThreshold(float newThreshold)
{
    threshold = juce::jlimit(0.0f, 1.0f, newThreshold);
    repaint();
}

void SpectrogramComponent::setUseLogFrequency(bool shouldUseLogScale)
{
    if (useLogFrequency != shouldUseLogScale)
    {
        useLogFrequency = shouldUseLogScale;
        
        // Update frequency scale
        for (int i = 0; i < kMaxFFTSize; ++i)
        {
            float proportion = static_cast<float>(i) / static_cast<float>(kMaxFFTSize - 1);
            frequencyScale[i] = (useLogFrequency)
                ? kMinFrequency * std::pow(kMaxFrequency / kMinFrequency, proportion)
                : kMinFrequency + (kMaxFrequency - kMinFrequency) * proportion;
        }
        
        repaint();
    }
}

void SpectrogramComponent::setShowThreshold(bool shouldShow)
{
    showThreshold = shouldShow;
    repaint();
}

void SpectrogramComponent::markFrequency(float freqHz, bool isActive)
{
    // Check if this frequency is already marked
    for (auto& markedFreq : markedFrequencies)
    {
        if (std::abs(markedFreq.frequency - freqHz) < 0.1f)
        {
            markedFreq.active = isActive;
            repaint();
            return;
        }
    }
    
    // Add new marked frequency
    markedFrequencies.push_back({freqHz, isActive});
    repaint();
}

void SpectrogramComponent::clearMarkedFrequencies()
{
    markedFrequencies.clear();
    repaint();
}

void SpectrogramComponent::timerCallback()
{
    // Trigger a repaint to update any animations
    repaint();
}

float SpectrogramComponent::frequencyToX(float frequency) const
{
    float normX;
    
    if (useLogFrequency)
    {
        // Logarithmic scale
        normX = std::log(frequency / kMinFrequency) / std::log(kMaxFrequency / kMinFrequency);
    }
    else
    {
        // Linear scale
        normX = (frequency - kMinFrequency) / (kMaxFrequency - kMinFrequency);
    }
    
    return normX * static_cast<float>(getWidth());
}

float SpectrogramComponent::amplitudeToY(float amplitude) const
{
    // Invert Y coordinate (0 amplitude at bottom)
    float normY = 1.0f - amplitude / kMaxAmplitude;
    return normY * static_cast<float>(getHeight());
}
\`\`\`

# source/gui/SpectrogramComponent.h

\`\`\`h
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <array>
#include <deque>

/**
 * A component for visualizing FFT spectral data
 */
class SpectrogramComponent : public juce::Component, 
                             private juce::Timer
{
public:
    SpectrogramComponent();
    ~SpectrogramComponent() override;
    
    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    /**
     * Update the component with new FFT data
     * @param fftData Pointer to FFT magnitude spectrum
     * @param size Size of the FFT data
     */
    void updateFFT(const float* fftData, int size);
    
    /**
     * Set the amplitude threshold for display
     * @param newThreshold Threshold value (0.0 to 1.0)
     */
    void setThreshold(float newThreshold);
    
    /**
     * Set whether to use logarithmic frequency scaling
     * @param shouldUseLogScale True to use log scale, false for linear
     */
    void setUseLogFrequency(bool shouldUseLogScale);
    
    /**
     * Set whether to display the threshold line
     * @param shouldShow True to show the threshold line
     */
    void setShowThreshold(bool shouldShow);
    
    /**
     * Mark a specific frequency as "active" (e.g., for learning mode)
     * @param freqHz Frequency in Hz to mark
     * @param isActive Whether to show as active
     */
    void markFrequency(float freqHz, bool isActive);
    
    /**
     * Clear all marked frequencies
     */
    void clearMarkedFrequencies();
    
private:
    //==============================================================================
    void timerCallback() override;
    
    /**
     * Convert a frequency value to an x-coordinate
     * @param frequency Frequency in Hz
     * @return X coordinate in the component space
     */
    float frequencyToX(float frequency) const;
    
    /**
     * Convert an amplitude value to a y-coordinate
     * @param amplitude Amplitude value (0.0 to 1.0)
     * @return Y coordinate in the component space
     */
    float amplitudeToY(float amplitude) const;
    
    //==============================================================================
    // Constants
    static constexpr float kMinFrequency = 20.0f;   // Hz
    static constexpr float kMaxFrequency = 20000.0f; // Hz
    static constexpr float kMaxAmplitude = 1.0f;
    static constexpr int kMaxFFTSize = 4096;
    
    // FFT data
    std::vector<float> fftData;
    std::array<float, kMaxFFTSize> frequencyScale;
    
    // Visual settings
    float threshold;
    bool useLogFrequency;
    bool showThreshold;
    
    // Marked frequencies (e.g., for learning mode)
    struct MarkedFreq {
        float frequency;
        bool active;
    };
    std::vector<MarkedFreq> markedFrequencies;
    
    // Sample rate
    float sampleRate;
    
    // Decay buffer for smoother visualization
    std::vector<float> peakData;
    
    // Display history for waterfall effect
    std::deque<std::vector<float>> history;
    static constexpr int kHistorySize = 20;
    
    // Refresh rate
    static constexpr int kRefreshRateMs = 30;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent)
};
\`\`\`

# source/midi/MIDIManager.cpp

\`\`\`cpp
#include "MIDIManager.h"

MIDIManager::MIDIManager()
    : midiChannel(1),
      midiVelocity(100),
      noteOnDelaySamples(0),
      noteOffDelaySamples(0),
      sampleRate(44100.0)
{
    setNoteOnDelayMs(50);   // 50ms delay before sending note-on
    setNoteOffDelayMs(100); // 100ms delay before sending note-off
}

MIDIManager::~MIDIManager()
{
}

void MIDIManager::processNotes(const std::vector<int>& detectedNotes, juce::MidiBuffer& midiBuffer, int sampleNumber)
{
    // Convert detected notes to a set for easier comparison
    std::set<int> currentNotes(detectedNotes.begin(), detectedNotes.end());
    
    // Process pending note-offs
    std::vector<int> notesToRemove;
    
    for (auto& pair : pendingNoteOffs)
    {
        int note = pair.first;
        int& samplesRemaining = pair.second;
        
        // If the note is detected again, remove it from pending note-offs
        if (currentNotes.find(note) != currentNotes.end())
        {
            notesToRemove.push_back(note);
            continue;
        }
        
        // Decrement counter
        samplesRemaining--;
        
        // If countdown is complete, send note-off
        if (samplesRemaining <= 0)
        {
            if (activeNotes.find(note) != activeNotes.end())
            {
                juce::MidiMessage noteOff = juce::MidiMessage::noteOff(midiChannel, note, 0.0f);
                midiBuffer.addEvent(noteOff, sampleNumber);
                activeNotes.erase(note);
            }
            notesToRemove.push_back(note);
        }
    }
    
    // Remove processed note-offs
    for (int note : notesToRemove)
    {
        pendingNoteOffs.erase(note);
    }
    
    // Find notes to turn off (notes that were active but are no longer detected)
    for (int note : activeNotes)
    {
        if (currentNotes.find(note) == currentNotes.end() && 
            pendingNoteOffs.find(note) == pendingNoteOffs.end())
        {
            // Add to pending note-offs
            pendingNoteOffs[note] = noteOffDelaySamples;
        }
    }
    
    // Process pending note-ons
    std::vector<int> notesToSend;
    
    for (int note : currentNotes)
    {
        // If the note is already active, skip it
        if (activeNotes.find(note) != activeNotes.end())
            continue;
        
        // If the note is not in pending note-ons yet, add it
        if (pendingNoteOns.find(note) == pendingNoteOns.end())
        {
            pendingNoteOns.insert(note);
            continue;
        }
        
        // If the note has been pending long enough, send note-on
        if (pendingNoteOns.find(note) != pendingNoteOns.end())
        {
            notesToSend.push_back(note);
        }
    }
    
    // Send note-on messages for validated notes
    for (int note : notesToSend)
    {
        juce::MidiMessage noteOn = juce::MidiMessage::noteOn(midiChannel, note, (float)midiVelocity / 127.0f);
        midiBuffer.addEvent(noteOn, sampleNumber);
        activeNotes.insert(note);
        pendingNoteOns.erase(note);
    }
    
    // Remove notes from pending note-ons if they're no longer detected
    std::vector<int> pendingToRemove;
    for (int note : pendingNoteOns)
    {
        if (currentNotes.find(note) == currentNotes.end())
        {
            pendingToRemove.push_back(note);
        }
    }
    
    for (int note : pendingToRemove)
    {
        pendingNoteOns.erase(note);
    }
}

void MIDIManager::reset(juce::MidiBuffer& midiBuffer, int sampleNumber)
{
    // Send note-off messages for all active notes
    for (int note : activeNotes)
    {
        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(midiChannel, note, 0.0f);
        midiBuffer.addEvent(noteOff, sampleNumber);
    }
    
    // Clear all states
    activeNotes.clear();
    pendingNoteOns.clear();
    pendingNoteOffs.clear();
}

void MIDIManager::setMidiChannel(int channel)
{
    midiChannel = juce::jlimit(1, 16, channel);
}

void MIDIManager::setMidiVelocity(int velocity)
{
    midiVelocity = juce::jlimit(0, 127, velocity);
}

void MIDIManager::setNoteOnDelayMs(int ms)
{
    double delaySeconds = ms / 1000.0;
    noteOnDelaySamples = static_cast<int>(delaySeconds * sampleRate);
}

void MIDIManager::setNoteOffDelayMs(int ms)
{
    double delaySeconds = ms / 1000.0;
    noteOffDelaySamples = static_cast<int>(delaySeconds * sampleRate);
}

void MIDIManager::updateSampleRate(double newSampleRate)
{
    // Avoid floating point comparison warning by checking if the difference is significant
    if (std::abs(sampleRate - newSampleRate) > 0.001)
    {
        sampleRate = newSampleRate;
        
        // Recalculate sample delays
        setNoteOnDelayMs(static_cast<int>(noteOnDelaySamples * 1000.0 / sampleRate));
        setNoteOffDelayMs(static_cast<int>(noteOffDelaySamples * 1000.0 / sampleRate));
    }
}
\`\`\`

# source/midi/MIDIManager.h

\`\`\`h
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <set>
#include <vector>

/**
 * MIDIManager handles the generation and management of MIDI messages
 * for pitch detection output.
 */
class MIDIManager
{
public:
    /**
     * Constructor
     */
    MIDIManager();
    
    /**
     * Destructor
     */
    ~MIDIManager();
    
    /**
     * Process a new set of detected notes, generating MIDI messages as needed
     * @param detectedNotes Vector of MIDI note numbers
     * @param midiBuffer MIDI buffer to add messages to
     * @param sampleNumber Sample position for the MIDI messages
     */
    void processNotes(const std::vector<int>& detectedNotes, juce::MidiBuffer& midiBuffer, int sampleNumber);
    
    /**
     * Resets the manager, turning off all active notes
     * @param midiBuffer MIDI buffer to add note-off messages to
     * @param sampleNumber Sample position for the MIDI messages
     */
    void reset(juce::MidiBuffer& midiBuffer, int sampleNumber);
    
    /**
     * Sets the MIDI channel for output
     * @param channel MIDI channel (1-16)
     */
    void setMidiChannel(int channel);
    
    /**
     * Sets the MIDI velocity for note-on messages
     * @param velocity MIDI velocity (0-127)
     */
    void setMidiVelocity(int velocity);
    
    /**
     * Sets the minimum time in milliseconds a note must be detected 
     * before sending a note-on message
     * @param ms Time in milliseconds
     */
    void setNoteOnDelayMs(int ms);
    
    /**
     * Sets the minimum time in milliseconds a note must be absent
     * before sending a note-off message
     * @param ms Time in milliseconds
     */
    void setNoteOffDelayMs(int ms);

    void updateSampleRate(double newSampleRate);
    
private:
    std::set<int> activeNotes;         // Currently active (sounding) notes
    std::set<int> pendingNoteOns;      // Notes waiting to be turned on
    std::map<int, int> pendingNoteOffs; // Notes waiting to be turned off <note, samplesRemaining>
    
    int midiChannel;
    int midiVelocity;
    
    int noteOnDelaySamples;
    int noteOffDelaySamples;
    double sampleRate;
    
    /**
     * Updates the sample rate dependent parameters
     * @param newSampleRate New sample rate in Hz
     */
};
\`\`\`

# source/midi/MIDIProcessor.cpp

\`\`\`cpp

\`\`\`

# source/midi/MIDIProcessor.h

\`\`\`h

\`\`\`

# source/PluginEditor.cpp

\`\`\`cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolyphonicTrackerAudioProcessorEditor::PolyphonicTrackerAudioProcessorEditor (PolyphonicTrackerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set up the main component size
    setSize (600, 400);
    
    // Add tabbed component
    addAndMakeVisible(tabbedComponent);
    
    // Create tabs
    auto* mainPanel = new juce::Component();
    auto* visualPanel = new juce::Component();
    
    tabbedComponent.addTab("Controls", juce::Colours::lightgrey, mainPanel, true);
    tabbedComponent.addTab("Visualization", juce::Colours::darkgrey, visualPanel, true);
    
    // Add controls to the main panel
    mainPanel->addAndMakeVisible(learningModeToggle);
    
    // Set up current note slider
    currentNoteSlider.setRange(21, 108, 1);
    currentNoteSlider.setValue(60);
    currentNoteSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    currentNoteSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(currentNoteSlider);
    mainPanel->addAndMakeVisible(currentNoteLabel);
    
    // Set up max polyphony slider
    maxPolyphonySlider.setRange(1, 8, 1);
    maxPolyphonySlider.setValue(6);
    maxPolyphonySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    maxPolyphonySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(maxPolyphonySlider);
    mainPanel->addAndMakeVisible(maxPolyphonyLabel);
    
    // Set up MIDI channel slider
    midiChannelSlider.setRange(1, 16, 1);
    midiChannelSlider.setValue(1);
    midiChannelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    midiChannelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(midiChannelSlider);
    mainPanel->addAndMakeVisible(midiChannelLabel);
    
    // Set up MIDI velocity slider
    midiVelocitySlider.setRange(0, 127, 1);
    midiVelocitySlider.setValue(100);
    midiVelocitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    midiVelocitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(midiVelocitySlider);
    mainPanel->addAndMakeVisible(midiVelocityLabel);
    
    // Set up note on delay slider
    noteOnDelaySlider.setRange(0, 500, 1);
    noteOnDelaySlider.setValue(50);
    noteOnDelaySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noteOnDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(noteOnDelaySlider);
    mainPanel->addAndMakeVisible(noteOnDelayLabel);
    
    // Set up note off delay slider
    noteOffDelaySlider.setRange(0, 500, 1);
    noteOffDelaySlider.setValue(100);
    noteOffDelaySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noteOffDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(noteOffDelaySlider);
    mainPanel->addAndMakeVisible(noteOffDelayLabel);
    
    // Create parameter attachments
    auto& params = audioProcessor.getParameterTree();
    
    learningModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        params, "learningMode", learningModeToggle);
    
    currentNoteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "currentNote", currentNoteSlider);
    
    maxPolyphonyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "maxPolyphony", maxPolyphonySlider);
    
    midiChannelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "midiChannel", midiChannelSlider);
    
    midiVelocityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "midiVelocity", midiVelocitySlider);
    
    noteOnDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "noteOnDelay", noteOnDelaySlider);
    
    noteOffDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "noteOffDelay", noteOffDelaySlider);
}

PolyphonicTrackerAudioProcessorEditor::~PolyphonicTrackerAudioProcessorEditor()
{
}

//==============================================================================
void PolyphonicTrackerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill the background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PolyphonicTrackerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    // Position the tabbed component to fill the entire window
    tabbedComponent.setBounds(area);
    
    // Position controls on the main panel
    if (auto* mainPanel = tabbedComponent.getTabContentComponent(0))
    {
        const int margin = 10;
        const int labelWidth = 150;
        const int controlHeight = 24;
        int y = margin;
        
        // Learning mode toggle
        learningModeToggle.setBounds(margin, y, 150, controlHeight);
        y += controlHeight + margin;
        
        // Current note slider
        currentNoteLabel.setBounds(margin, y, labelWidth, controlHeight);
        currentNoteSlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        y += controlHeight + margin;
        
        // Max polyphony slider
        maxPolyphonyLabel.setBounds(margin, y, labelWidth, controlHeight);
        maxPolyphonySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        y += controlHeight + margin;
        
        // MIDI channel slider
        midiChannelLabel.setBounds(margin, y, labelWidth, controlHeight);
        midiChannelSlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        y += controlHeight + margin;
        
        // MIDI velocity slider
        midiVelocityLabel.setBounds(margin, y, labelWidth, controlHeight);
        midiVelocitySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        y += controlHeight + margin;
        
        // Note on delay slider
        noteOnDelayLabel.setBounds(margin, y, labelWidth, controlHeight);
        noteOnDelaySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        y += controlHeight + margin;
        
        // Note off delay slider
        noteOffDelayLabel.setBounds(margin, y, labelWidth, controlHeight);
        noteOffDelaySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
    }
}
\`\`\`

# source/PluginEditor.h

\`\`\`h
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

//==============================================================================
class PolyphonicTrackerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit PolyphonicTrackerAudioProcessorEditor (PolyphonicTrackerAudioProcessor&);
    ~PolyphonicTrackerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Reference to the processor
    PolyphonicTrackerAudioProcessor& audioProcessor;

    // UI Components
    juce::TabbedComponent tabbedComponent {juce::TabbedButtonBar::TabsAtTop};    
    // Learning mode controls
    juce::ToggleButton learningModeToggle {"Learning Mode"};
    juce::Slider currentNoteSlider;
    juce::Label currentNoteLabel {"", "Current Note:"};
    
    // Guitar mode controls
    juce::ComboBox guitarStringCombo;
    juce::Slider guitarFretSlider;
    juce::Label guitarPositionLabel {"", "Position:"};
    
    // MIDI output controls
    juce::Slider maxPolyphonySlider;
    juce::Label maxPolyphonyLabel {"", "Max Polyphony:"};
    
    juce::Slider midiChannelSlider;
    juce::Label midiChannelLabel {"", "MIDI Channel:"};
    
    juce::Slider midiVelocitySlider;
    juce::Label midiVelocityLabel {"", "MIDI Velocity:"};
    
    juce::Slider noteOnDelaySlider;
    juce::Label noteOnDelayLabel {"", "Note On Delay (ms):"};
    
    juce::Slider noteOffDelaySlider;
    juce::Label noteOffDelayLabel {"", "Note Off Delay (ms):"};
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> learningModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> currentNoteAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> maxPolyphonyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midiChannelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midiVelocityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noteOnDelayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noteOffDelayAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyphonicTrackerAudioProcessorEditor)
};
\`\`\`

# source/PluginProcessor.cpp

\`\`\`cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/FFTProcessor.h"
#include "dsp/PitchDetector.h"
#include "midi/MIDIManager.h"

//==============================================================================
PolyphonicTrackerAudioProcessor::PolyphonicTrackerAudioProcessor()
    : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout()),
      currentFFTSize(4096),
      currentOverlapFactor(0.75f),
      numFrets(24),
      currentGuitarString(0),
      currentGuitarFret(0),
      instrumentType(0)
{
    // Initialize DSP components
    fftProcessor = std::make_unique<FFTProcessor>(currentFFTSize);
    pitchDetector = std::make_unique<PitchDetector>(6); // Default to 6 notes of polyphony
    midiManager = std::make_unique<MIDIManager>();
    
    // Set up FFT processor callback
    fftProcessor->setSpectrumDataCallback([this](const float* spectrum, int size) {
        handleNewFFTBlock(spectrum, size);
    });
    
    // Initialize parameters
    learningModeParam = parameters.getRawParameterValue("learningMode");
    currentNoteParam = parameters.getRawParameterValue("currentNote");
    maxPolyphonyParam = parameters.getRawParameterValue("maxPolyphony");
    midiChannelParam = parameters.getRawParameterValue("midiChannel");
    midiVelocityParam = parameters.getRawParameterValue("midiVelocity");
    noteOnDelayParam = parameters.getRawParameterValue("noteOnDelay");
    noteOffDelayParam = parameters.getRawParameterValue("noteOffDelay");
    
    // Start the timer for GUI updates
    startTimerHz(30); // 30 updates per second
}

PolyphonicTrackerAudioProcessor::~PolyphonicTrackerAudioProcessor()
{
    stopTimer();
}

//==============================================================================
const juce::String PolyphonicTrackerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PolyphonicTrackerAudioProcessor::acceptsMidi() const
{
    return false;
}

bool PolyphonicTrackerAudioProcessor::producesMidi() const
{
    return true;
}

bool PolyphonicTrackerAudioProcessor::isMidiEffect() const
{
    return false;
}

double PolyphonicTrackerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PolyphonicTrackerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PolyphonicTrackerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PolyphonicTrackerAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String PolyphonicTrackerAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void PolyphonicTrackerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void PolyphonicTrackerAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    fftProcessor->reset();
    pitchDetector->setMaxPolyphony(static_cast<int>(*maxPolyphonyParam));
    pitchDetector->setLearningModeActive(*learningModeParam > 0.5f);
    pitchDetector->setCurrentLearningNote(static_cast<int>(*currentNoteParam));
    
    midiManager->setMidiChannel(static_cast<int>(*midiChannelParam));
    midiManager->setMidiVelocity(static_cast<int>(*midiVelocityParam));
    midiManager->setNoteOnDelayMs(static_cast<int>(*noteOnDelayParam));
    midiManager->setNoteOffDelayMs(static_cast<int>(*noteOffDelayParam));
    midiManager->updateSampleRate(sampleRate); // Add this line
}

void PolyphonicTrackerAudioProcessor::releaseResources()
{
    // Release any resources when playback stops
}

bool PolyphonicTrackerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // We only support mono or stereo inputs
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // We require at least one input channel
    if (layouts.getMainInputChannelSet().size() < 1)
        return false;
    
    // Input and output layouts should match
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    
    return true;
}

void PolyphonicTrackerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Clear the incoming MIDI buffer as we'll be generating our own MIDI
    midiMessages.clear();

    // Extract mono input from the buffer (average if stereo)
    juce::AudioBuffer<float> monoBuffer(1, buffer.getNumSamples());
    monoBuffer.clear();
    
    // Mix down to mono
    if (totalNumInputChannels == 1)
    {
        monoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
    }
    else
    {
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            monoBuffer.addFrom(0, 0, buffer, ch, 0, buffer.getNumSamples(), 1.0f / totalNumInputChannels);
        }
    }
    
    // Process the mono buffer through FFT
    bool fftPerformed = fftProcessor->processBlock(monoBuffer.getReadPointer(0), monoBuffer.getNumSamples());
    
    // If an FFT was performed, handle the detected notes directly
    if (fftPerformed)
    {
        auto spectrum = fftProcessor->getMagnitudeSpectrum();
        auto detectedNotes = pitchDetector->processSpectrum(spectrum, fftProcessor->getSpectrumSize());
        
        // Process the detected notes into MIDI events
        midiManager->processNotes(detectedNotes, midiMessages, 0);
    }
    
    // Pass through the audio
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // No audio processing, just pass through
        juce::ignoreUnused(buffer.getWritePointer(channel));
    }
}

//==============================================================================
bool PolyphonicTrackerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PolyphonicTrackerAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
    // We'll create a custom editor later
    // return new PolyphonicTrackerAudioProcessorEditor(*this);
}

//==============================================================================
void PolyphonicTrackerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Store parameters
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PolyphonicTrackerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore parameters
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
void PolyphonicTrackerAudioProcessor::timerCallback()
{
    // Update parameters from GUI
    if (pitchDetector != nullptr)
    {
        bool learningMode = *learningModeParam > 0.5f;
        int currentNote = static_cast<int>(*currentNoteParam);
        int maxPolyphony = static_cast<int>(*maxPolyphonyParam);
        
        pitchDetector->setLearningModeActive(learningMode);
        pitchDetector->setCurrentLearningNote(currentNote);
        pitchDetector->setMaxPolyphony(maxPolyphony);
    }
    
    if (midiManager != nullptr)
    {
        int midiChannel = static_cast<int>(*midiChannelParam);
        int midiVelocity = static_cast<int>(*midiVelocityParam);
        int noteOnDelay = static_cast<int>(*noteOnDelayParam);
        int noteOffDelay = static_cast<int>(*noteOffDelayParam);
        
        midiManager->setMidiChannel(midiChannel);
        midiManager->setMidiVelocity(midiVelocity);
        midiManager->setNoteOnDelayMs(noteOnDelay);
        midiManager->setNoteOffDelayMs(noteOffDelay);
    }
}

void PolyphonicTrackerAudioProcessor::handleNewFFTBlock(const float* fftData, int fftSize)
{
    // Process the FFT data through the pitch detector
    auto detectedNotes = pitchDetector->processSpectrum(fftData, fftSize);
    
    // Create a temporary MIDI buffer for the notes
    juce::MidiBuffer tempBuffer;
    
    // Process the detected notes and generate MIDI
    midiManager->processNotes(detectedNotes, tempBuffer, 0);
    
    // Call the FFT data callback if registered
    if (fftDataCallback)
    {
        fftDataCallback(fftData, fftSize);
    }
    
    // The MIDI messages will be sent in the next processBlock call
}

void PolyphonicTrackerAudioProcessor::setFFTDataCallback(std::function<void(const float*, int)> callback)
{
    fftDataCallback = callback;
}

const float* PolyphonicTrackerAudioProcessor::getLatestFFTData() const
{
    return fftProcessor ? fftProcessor->getMagnitudeSpectrum() : nullptr;
}

int PolyphonicTrackerAudioProcessor::getLatestFFTSize() const
{
    return fftProcessor ? fftProcessor->getSpectrumSize() : 0;
}



void PolyphonicTrackerAudioProcessor::setInstrumentType(PitchDetector::InstrumentType type)
{
    pitchDetector->setInstrumentType(type);
}

PitchDetector::InstrumentType PolyphonicTrackerAudioProcessor::getInstrumentType() const
{
    return pitchDetector->getInstrumentType();
}

void PolyphonicTrackerAudioProcessor::setGuitarSettings(const PitchDetector::GuitarSettings& settings)
{
    pitchDetector->setGuitarSettings(settings);
}

const PitchDetector::GuitarSettings& PolyphonicTrackerAudioProcessor::getGuitarSettings() const
{
    return pitchDetector->getGuitarSettings();
}

int PolyphonicTrackerAudioProcessor::setCurrentGuitarPosition(int stringIndex, int fret)
{
    return pitchDetector->setCurrentGuitarPosition(stringIndex, fret);
}

void PolyphonicTrackerAudioProcessor::getCurrentGuitarPosition(int& stringIndex, int& fretNumber) const
{
    pitchDetector->getCurrentGuitarPosition(stringIndex, fretNumber);
}

juce::AudioProcessorValueTreeState::ParameterLayout PolyphonicTrackerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Learning mode switch
    layout.add(std::make_unique<juce::AudioParameterBool>(
        "learningMode", "Learning Mode", false));
    
    // Current note for learning (MIDI note number)
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "currentNote", "Current Note", 21, 108, 60));
    
    // Maximum polyphony
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "maxPolyphony", "Max Polyphony", 1, 16, 6));
    
    // MIDI output parameters
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "midiChannel", "MIDI Channel", 1, 16, 1));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "midiVelocity", "MIDI Velocity", 0, 127, 100));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "noteOnDelay", "Note On Delay (ms)", 0, 500, 50));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "noteOffDelay", "Note Off Delay (ms)", 0, 500, 100));
    
    return layout;
}

//==============================================================================
// Polyphonic tracker parameter methods
//==============================================================================
void PolyphonicTrackerAudioProcessor::setLearningModeActive(bool shouldBeActive)
{
    if (pitchDetector != nullptr)
        pitchDetector->setLearningModeActive(shouldBeActive);
    
    *parameters.getRawParameterValue("learningMode") = shouldBeActive ? 1.0f : 0.0f;
}

bool PolyphonicTrackerAudioProcessor::isLearningModeActive() const
{
    return pitchDetector != nullptr && pitchDetector->isLearningModeActive();
}

void PolyphonicTrackerAudioProcessor::setCurrentLearningNote(int midiNote)
{
    if (pitchDetector != nullptr)
        pitchDetector->setCurrentLearningNote(midiNote);
    
    *parameters.getRawParameterValue("currentNote") = static_cast<float>(midiNote);
}

int PolyphonicTrackerAudioProcessor::getCurrentLearningNote() const
{
    return pitchDetector != nullptr ? pitchDetector->getCurrentLearningNote() : -1;
}

void PolyphonicTrackerAudioProcessor::setMaxPolyphony(int maxNotes)
{
    if (pitchDetector != nullptr)
        pitchDetector->setMaxPolyphony(maxNotes);
    
    *parameters.getRawParameterValue("maxPolyphony") = static_cast<float>(maxNotes);
}

int PolyphonicTrackerAudioProcessor::getMaxPolyphony() const
{
    return pitchDetector != nullptr ? pitchDetector->getMaxPolyphony() : 6;
}

bool PolyphonicTrackerAudioProcessor::saveInstrumentData(const juce::String& filePath)
{
    return pitchDetector != nullptr && pitchDetector->saveInstrumentData(filePath);
}

bool PolyphonicTrackerAudioProcessor::loadInstrumentData(const juce::String& filePath)
{
    return pitchDetector != nullptr && pitchDetector->loadInstrumentData(filePath);
}

void PolyphonicTrackerAudioProcessor::setMidiChannel(int channel)
{
    if (midiManager != nullptr)
        midiManager->setMidiChannel(channel);
    
    *parameters.getRawParameterValue("midiChannel") = static_cast<float>(channel);
}

void PolyphonicTrackerAudioProcessor::setMidiVelocity(int velocity)
{
    if (midiManager != nullptr)
        midiManager->setMidiVelocity(velocity);
    
    *parameters.getRawParameterValue("midiVelocity") = static_cast<float>(velocity);
}

void PolyphonicTrackerAudioProcessor::setNoteOnDelayMs(int ms)
{
    if (midiManager != nullptr)
        midiManager->setNoteOnDelayMs(ms);
    
    *parameters.getRawParameterValue("noteOnDelay") = static_cast<float>(ms);
}

void PolyphonicTrackerAudioProcessor::setNoteOffDelayMs(int ms)
{
    if (midiManager != nullptr)
        midiManager->setNoteOffDelayMs(ms);
    
    *parameters.getRawParameterValue("noteOffDelay") = static_cast<float>(ms);
}

void PolyphonicTrackerAudioProcessor::setFFTSize(int fftSize)
{
    if (fftSize != currentFFTSize && fftProcessor != nullptr)
    {
        currentFFTSize = fftSize;
        fftProcessor.reset(new FFTProcessor(fftSize));
        fftProcessor->setOverlapFactor(currentOverlapFactor);
        
        // Reconnect the FFT callback
        fftProcessor->setSpectrumDataCallback([this](const float* spectrum, int size) {
            handleNewFFTBlock(spectrum, size);
        });
    }
}

int PolyphonicTrackerAudioProcessor::getFFTSize() const
{
    return currentFFTSize;
}

void PolyphonicTrackerAudioProcessor::setFFTOverlap(float overlapFactor)
{
    currentOverlapFactor = overlapFactor;
    
    if (fftProcessor != nullptr)
    {
        fftProcessor->setOverlapFactor(overlapFactor);
    }
}

float PolyphonicTrackerAudioProcessor::getFFTOverlap() const
{
    return currentOverlapFactor;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolyphonicTrackerAudioProcessor();
}
\`\`\`

# source/PluginProcessor.h

\`\`\`h
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "dsp/PitchDetector.h"

// Forward declarations
class FFTProcessor;
class PitchDetector;
class MIDIManager;

//==============================================================================
class PolyphonicTrackerAudioProcessor : public juce::AudioProcessor, public juce::Timer
{
public:
    //==============================================================================
    PolyphonicTrackerAudioProcessor();
    ~PolyphonicTrackerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getParameterTree() { return parameters; }
    
    // FFT Visualization support
    void setFFTDataCallback(std::function<void(const float*, int)> callback);
    const float* getLatestFFTData() const;
    int getLatestFFTSize() const;

    //==============================================================================
    // Learning mode
    void setLearningModeActive(bool shouldBeActive);
    bool isLearningModeActive() const;
    
    // Learning note
    void setCurrentLearningNote(int midiNote);
    int getCurrentLearningNote() const;
    
    // Set max polyphony
    void setMaxPolyphony(int maxNotes);
    PitchDetector::InstrumentType getInstrumentType() const;
    
    // Instrument type
    void setInstrumentType(PitchDetector::InstrumentType type);
    int getInstrumentType() const;
    
    // Guitar-specific learning
    void setGuitarSettings(const PitchDetector::GuitarSettings& settings);
    int setCurrentGuitarPosition(int stringIndex, int fret);
    void getCurrentGuitarPosition(int& stringIndex, int& fretNumber) const;
    const PitchDetector::GuitarSettings& getGuitarSettings() const;
    
    // Guitar settings access
    const juce::StringArray& getOpenStringMidiNotes() const;
    int getNumFrets() const;
    
    // Save/load instrument data
    bool saveInstrumentData(const juce::String& filePath);
    bool loadInstrumentData(const juce::String& filePath);
    
    // Parameters for MIDI output
    void setMidiChannel(int channel);
    void setMidiVelocity(int velocity);
    void setNoteOnDelayMs(int ms);
    void setNoteOffDelayMs(int ms);
    
    // Processor settings
    void setFFTSize(int fftSize);
    int getFFTSize() const;
    
    void setFFTOverlap(float overlapFactor);
    float getFFTOverlap() const;
    void timerCallback() override; // Declare the virtual method

private:
    //==============================================================================
    // Create parameter layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // Process the FFT results and handle pitch detection
    void handleNewFFTBlock(const float* fftData, int fftSize);
    
    //==============================================================================
    // Parameter storage
    juce::AudioProcessorValueTreeState parameters;
    
    // DSP components
    std::unique_ptr<FFTProcessor> fftProcessor;
    std::unique_ptr<PitchDetector> pitchDetector;
    std::unique_ptr<MIDIManager> midiManager;
    
    // FFT visualization support
    std::function<void(const float*, int)> fftDataCallback;
    
    // Internal state
    int currentFFTSize;
    float currentOverlapFactor;
    
    // Guitar settings
    juce::StringArray openStringMidiNotes;
    int numFrets;
    int currentGuitarString;
    int currentGuitarFret;
    int instrumentType;
    
    // Parameter pointers
    std::atomic<float>* learningModeParam = nullptr;
    std::atomic<float>* currentNoteParam = nullptr;
    std::atomic<float>* maxPolyphonyParam = nullptr;
    std::atomic<float>* midiChannelParam = nullptr;
    std::atomic<float>* midiVelocityParam = nullptr;
    std::atomic<float>* noteOnDelayParam = nullptr;
    std::atomic<float>* noteOffDelayParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyphonicTrackerAudioProcessor)
};
\`\`\`

# source/utils/Constants.h

\`\`\`h

\`\`\`

# source/utils/DSPUtils.cpp

\`\`\`cpp

\`\`\`

# source/utils/DSPUtils.h

\`\`\`h

\`\`\`

# tests/CMakeLists.txt

\`\`\`txt
enable_testing()

# Add test executable
add_executable(PolyphonicTrackerTests
    TestMain.cpp
    PitchDetectionTests.cpp
    FFTProcessorTests.cpp
)

# Link with main project and testing framework
target_link_libraries(PolyphonicTrackerTests
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
)

# Add tests to CTest
add_test(NAME PolyphonicTrackerTests COMMAND PolyphonicTrackerTests)
\`\`\`

# tests/FFTProcessorTests.cpp

\`\`\`cpp

\`\`\`

# tests/PitchDetectionTests.cpp

\`\`\`cpp

\`\`\`

# tests/TestMain.cpp

\`\`\`cpp

\`\`\`


```

# docs/BUILDING.md

```md

```

# docs/CONTRIBUTING.md

```md

```

# docs/README.md

```md

```

# README.md

```md
# Polyphonic Tracker VST

A VST plugin for real-time polyphonic pitch tracking, based on machine learning techniques.

## Features
- Real-time polyphonic pitch detection
- Instrument learning capability
- MIDI output
- Spectral analysis visualization
- Low latency processing

## Building
See [BUILDING.md](docs/BUILDING.md) for detailed build instructions.

## Contributing
Contributions are welcome! Please read [CONTRIBUTING.md](docs/CONTRIBUTING.md) for details on our code of conduct and the process for submitting pull requests.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```

# resources/images/jay.jpg

This is a binary file of the type: Image

# source/audio/AudioEngine.cpp

```cpp

```

# source/audio/AudioEngine.h

```h

```

# source/dsp/FFTProcessor.cpp

```cpp
#include "FFTProcessor.h"

FFTProcessor::FFTProcessor(int fftSizeParam)
    : fftSize(fftSizeParam),
      spectrumSize(fftSizeParam / 2),
      overlapFactor(0.5f),
      inputBufferPos(0),
      fft(static_cast<int>(std::log2(static_cast<double>(fftSizeParam)))),
      window(static_cast<size_t>(fftSizeParam), juce::dsp::WindowingFunction<float>::hann)
{
    // Allocate memory for buffers
    inputBuffer.resize(static_cast<size_t>(fftSize), 0.0f);
    fftData.resize(static_cast<size_t>(fftSize * 2), 0.0f); // Complex data (real/imag pairs)
    magnitudeSpectrum.resize(static_cast<size_t>(spectrumSize), 0.0f);
}

FFTProcessor::~FFTProcessor()
{
}

bool FFTProcessor::processBlock(const float* inBuffer, int numSamples)
{
    bool fftPerformed = false;
    
    // Calculate hop size based on overlap factor
    int hopSize = static_cast<int>(fftSize * (1.0f - overlapFactor));
    if (hopSize < 1) hopSize = 1;
    
    // Add samples to the input buffer
    for (int i = 0; i < numSamples; ++i)
    {
        inputBuffer[static_cast<size_t>(inputBufferPos)] = inBuffer[i];
        inputBufferPos++;
        
        // If we have enough samples, perform the FFT
        if (inputBufferPos >= fftSize)
        {
            performFFT();
            
            // Shift the buffer by the hop size
            const int samplesToKeep = fftSize - hopSize;
            if (samplesToKeep > 0)
            {
                std::copy(inputBuffer.begin() + hopSize, inputBuffer.end(), inputBuffer.begin());
            }
            
            inputBufferPos = samplesToKeep;
            fftPerformed = true;
        }
    }
    
    return fftPerformed;
}

void FFTProcessor::performFFT()
{
    // Apply window function
    applyWindow();
    
    // Copy windowed input to FFT data array (real part)
    for (int i = 0; i < fftSize; ++i)
    {
        fftData[static_cast<size_t>(i * 2)] = inputBuffer[static_cast<size_t>(i)];
        fftData[static_cast<size_t>(i * 2 + 1)] = 0.0f; // Imaginary part is zero
    }
    
    // Perform the FFT
    fft.performRealOnlyForwardTransform(fftData.data(), true);
    
    // Calculate the magnitude spectrum
    for (int i = 0; i < spectrumSize; ++i)
    {
        float real = fftData[static_cast<size_t>(i * 2)];
        float imag = fftData[static_cast<size_t>(i * 2 + 1)];
        
        // Calculate magnitude (sqrt of real^2 + imag^2)
        magnitudeSpectrum[static_cast<size_t>(i)] = std::sqrt(real * real + imag * imag);
    }
    
    // Call the callback if registered
    if (spectrumCallback)
    {
        spectrumCallback(magnitudeSpectrum.data(), spectrumSize);
    }
}

void FFTProcessor::applyWindow()
{
    window.multiplyWithWindowingTable(inputBuffer.data(), static_cast<size_t>(fftSize));
}

const float* FFTProcessor::getMagnitudeSpectrum() const
{
    return magnitudeSpectrum.data();
}

int FFTProcessor::getSpectrumSize() const
{
    return spectrumSize;
}

int FFTProcessor::getFFTSize() const
{
    return fftSize;
}

void FFTProcessor::setOverlapFactor(float newOverlap)
{
    overlapFactor = juce::jlimit(0.0f, 0.95f, newOverlap);
}

void FFTProcessor::reset()
{
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(fftData.begin(), fftData.end(), 0.0f);
    std::fill(magnitudeSpectrum.begin(), magnitudeSpectrum.end(), 0.0f);
    inputBufferPos = 0;
}

void FFTProcessor::setSpectrumDataCallback(std::function<void(const float*, int)> callback)
{
    spectrumCallback = callback;
}
```

# source/dsp/FFTProcessor.h

```h
#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * FFTProcessor handles the FFT analysis of incoming audio.
 * It applies windowing, performs FFT, and calculates the magnitude spectrum.
 */
class FFTProcessor
{
public:
    /**
     * Constructor, initializes FFT with a specific size
     * @param fftSize The size of the FFT (must be a power of 2)
     */
    FFTProcessor(int fftSize = 4096);
    
    /**
     * Destructor
     */
    ~FFTProcessor();
    
    /**
     * Processes a block of audio and updates the FFT if enough samples are collected
     * @param inBuffer Audio input buffer
     * @param numSamples Number of samples in the buffer
     * @return True if FFT was performed, false otherwise
     */
    bool processBlock(const float* inBuffer, int numSamples);
    
    /**
     * Gets the current FFT magnitude spectrum
     * @return Pointer to the spectrum data
     */
    const float* getMagnitudeSpectrum() const;
    
    /**
     * Gets the size of the spectrum (fftSize / 2)
     * @return Size of the magnitude spectrum array
     */
    int getSpectrumSize() const;
    
    /**
     * Gets the current FFT size
     * @return Current FFT size
     */
    int getFFTSize() const;
    
    /**
     * Sets the overlap factor for the FFT processing
     * @param newOverlap Overlap factor (0.0 to 0.95)
     */
    void setOverlapFactor(float newOverlap);
    
    /**
     * Resets the FFT processor, clearing all buffers
     */
    void reset();
    
    /**
     * Registers a callback function to be called when new FFT data is available
     * @param callback Function to call with new spectrum data
     */
    void setSpectrumDataCallback(std::function<void(const float*, int)> callback);
    
private:
    int fftSize;
    int spectrumSize;
    float overlapFactor;
    
    std::vector<float> inputBuffer;
    int inputBufferPos;
    
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    
    std::vector<float> fftData;
    std::vector<float> magnitudeSpectrum;
    
    std::function<void(const float*, int)> spectrumCallback;
    
    void performFFT();
    void applyWindow();
};
```

# source/dsp/PitchDetector.cpp

```cpp
#include "PitchDetector.h"

PitchDetector::PitchDetector(int maxNotes)
    : learningModeActive(false),
      currentLearningNote(-1),
      maxPolyphony(maxNotes),
      requiredSpectraForLearning(10),
      instrumentType(InstrumentType::Generic),
      currentGuitarString(0),
      currentGuitarFret(0)
{
    // Set default guitar settings
    guitarSettings.openStringMidiNotes = {40, 45, 50, 55, 59, 64}; // E2, A2, D3, G3, B3, E4
    guitarSettings.numFrets = 24;
}

PitchDetector::~PitchDetector()
{
}

void PitchDetector::setLearningModeActive(bool shouldBeActive)
{
    learningModeActive = shouldBeActive;
}

bool PitchDetector::isLearningModeActive() const
{
    return learningModeActive;
}

void PitchDetector::setCurrentLearningNote(int midiNote)
{
    currentLearningNote = midiNote;
}

int PitchDetector::getCurrentLearningNote() const
{
    return currentLearningNote;
}

void PitchDetector::setGuitarSettings(const GuitarSettings& settings)
{
    guitarSettings = settings;
}

void PitchDetector::setInstrumentType(InstrumentType type)
{
    instrumentType = type;
}

const PitchDetector::GuitarSettings& PitchDetector::getGuitarSettings() const
{
    return guitarSettings;
}

PitchDetector::InstrumentType PitchDetector::getInstrumentType() const
{
    return instrumentType;
}

std::vector<int> PitchDetector::processSpectrum(const float* spectrum, int spectrumSize)
{
    std::vector<int> detectedNotes;
    
    // Convert input spectrum to vector
    std::vector<float> spectrumVec(spectrum, spectrum + spectrumSize);
    normalizeVector(spectrumVec);
    
    if (learningModeActive && currentLearningNote >= 0)
    {
        // Learning mode: store the spectrum for the current note
        addLearnedSpectrum(spectrum, spectrumSize, currentLearningNote);
        return detectedNotes; // Return empty vector in learning mode
    }
    else if (isReadyForDetection())
    {
        // Detection mode: perform polyphonic pitch detection
        detectedNotes = detectPolyphonicPitches(spectrum, spectrumSize);
        
        // Call the callback if registered
        if (noteCallback && !detectedNotes.empty())
        {
            noteCallback(detectedNotes);
        }
    }
    
    return detectedNotes;
}

void PitchDetector::addLearnedSpectrum(const float* spectrumData, int spectrumSize, int midiNote)
{
    // Create a vector from the spectrum data
    std::vector<float> spectrumVec(spectrumData, spectrumData + spectrumSize);
    normalizeVector(spectrumVec);
    
    // Add to the learned spectra for this note
    learnedSpectraPerNote[midiNote].push_back(spectrumVec);
    
    // If we have enough spectra for this note, create an average profile
    if (static_cast<int>(learnedSpectraPerNote[midiNote].size()) >= requiredSpectraForLearning)
    {
        // Calculate the average spectrum for this note
        std::vector<float> avgSpectrum(static_cast<size_t>(spectrumSize), 0.0f);        
        for (const auto& learnedSpectrum : learnedSpectraPerNote[midiNote])
        {
            for (int i = 0; i < spectrumSize; ++i)
            {
                avgSpectrum[static_cast<size_t>(i)] += learnedSpectrum[static_cast<size_t>(i)];            }
        }
        
        // Normalize the average
        for (float& value : avgSpectrum)
        {
            value /= learnedSpectraPerNote[midiNote].size();
        }
        
        normalizeVector(avgSpectrum);
        
        // Store as a learned profile
        SpectralProfile profile;
        profile.midiNote = midiNote;
        profile.spectrum = avgSpectrum;
        profile.noteName = midiNoteToName(midiNote);
        
        // Remove any existing profile for this note
        learnedProfiles.erase(
            std::remove_if(learnedProfiles.begin(), learnedProfiles.end(),
                          [midiNote](const SpectralProfile& p) { return p.midiNote == midiNote; }),
            learnedProfiles.end());
        
        // Add the new profile
        learnedProfiles.push_back(profile);
    }
}

std::vector<int> PitchDetector::detectPolyphonicPitches(const float* spectrum, int spectrumSize)
{
    if (learnedProfiles.empty())
    {
        return {};
    }
    
    // Convert input spectrum to vector
    std::vector<float> inputSpectrum(spectrum, spectrum + spectrumSize);
    normalizeVector(inputSpectrum);
    
    // Perform sparse encoding to find the most similar learned profiles
    std::vector<float> coefficients = sparseEncode(inputSpectrum);
    
    // Sort the coefficients and find the indices of the top values
    std::vector<std::pair<float, int>> coefPairs;
    for (size_t i = 0; i < coefficients.size(); ++i)
    {
        coefPairs.emplace_back(coefficients[i], static_cast<int>(i));
    }
    
    // Sort in descending order of coefficient values
    std::sort(coefPairs.begin(), coefPairs.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select the top notes, up to maxPolyphony
    std::vector<int> detectedNotes;
    
    for (int i = 0; i < std::min(maxPolyphony, static_cast<int>(coefPairs.size())); ++i)
    {
        // Only include notes with coefficients above the threshold
        if (coefPairs[static_cast<size_t>(i)].first >= minimumCoefficient)
        {
            int profileIndex = coefPairs[static_cast<size_t>(i)].second;
            int midiNote = learnedProfiles[static_cast<size_t>(profileIndex)].midiNote;
            
            // Check for octave errors or close notes (avoid duplicates)
            bool tooClose = false;
            for (int existingNote : detectedNotes)
            {
                int semitoneDistance = std::abs(existingNote - midiNote);
                if (semitoneDistance < maximumSemitoneDistance)
                {
                    tooClose = true;
                    break;
                }
            }
            
            if (!tooClose)
            {
                detectedNotes.push_back(midiNote);
            }
        }
    }
    
    return detectedNotes;
}

void PitchDetector::normalizeVector(std::vector<float>& vec)
{
    // Calculate the L2 norm (Euclidean length) of the vector
    float sumSquares = 0.0f;
    for (float val : vec)
    {
        sumSquares += val * val;
    }
    
    if (sumSquares > 0.0f)
    {
        float norm = std::sqrt(sumSquares);
        for (float& val : vec)
        {
            val /= norm;
        }
    }
}

std::vector<float> PitchDetector::sparseEncode(const std::vector<float>& input)
{
    // Simple implementation of sparse encoding using cosine similarity
    // In a more advanced implementation, this would use an L1-regularized solver
    
    std::vector<float> coefficients;
    
    for (const auto& profile : learnedProfiles)
    {
        // Calculate the dot product (cosine similarity for normalized vectors)
        float similarity = 0.0f;
        for (size_t i = 0; i < std::min(input.size(), profile.spectrum.size()); ++i)
        {
            similarity += input[i] * profile.spectrum[i];
        }
        
        coefficients.push_back(similarity);
    }
    
    return coefficients;
}

bool PitchDetector::saveInstrumentData(const juce::String& filePath)
{
    // Use simpler file-based approach for now
    juce::File file(filePath);
    juce::FileOutputStream outStream(file);
    
    if (!outStream.openedOk())
        return false;
    
    // Write the number of profiles
    outStream.writeInt(static_cast<int>(learnedProfiles.size()));
    
    for (const auto& profile : learnedProfiles)
    {
        // Write MIDI note
        outStream.writeInt(profile.midiNote);
        
        // Write note name length and string
        outStream.writeInt(static_cast<int>(profile.noteName.length()));        
        // Write spectrum size and data
        outStream.writeInt(static_cast<int>(profile.spectrum.size()));
        for (float val : profile.spectrum)
        {
            outStream.writeFloat(val);
        }
    }
    
    return true;
}

bool PitchDetector::loadInstrumentData(const juce::String& filePath)
{
    juce::File file(filePath);
    juce::FileInputStream inStream(file);
    
    if (!inStream.openedOk())
        return false;
    
    clearInstrumentData();
    
    // Read number of profiles
    int numProfiles = inStream.readInt();
    
    for (int i = 0; i < numProfiles; ++i)
    {
        SpectralProfile profile;
        
        // Read MIDI note
        profile.midiNote = inStream.readInt();
        
        // Read note name
        
        // Read spectrum
        int spectrumSize = inStream.readInt();
        profile.spectrum.resize(static_cast<size_t>(spectrumSize));        
        for (int j = 0; j < spectrumSize; ++j)
        {
            profile.spectrum[static_cast<size_t>(j)] = inStream.readFloat();        }
        
        learnedProfiles.push_back(profile);
    }
    
    return true;
}


int PitchDetector::setCurrentGuitarPosition(int stringIndex, int fret)
{
    currentGuitarString = stringIndex;
    currentGuitarFret = fret;
    
    // Calculate MIDI note based on string and fret
    if (stringIndex >= 0 && stringIndex < static_cast<int>(guitarSettings.openStringMidiNotes.size()))
    {
        int baseMidiNote = guitarSettings.openStringMidiNotes[static_cast<size_t>(stringIndex)];
        int midiNote = baseMidiNote + fret;
        return midiNote;
    }
    
    return 60; // Default to middle C if string is invalid
}


void PitchDetector::getCurrentGuitarPosition(int& stringIndex, int& fretNumber) const
{
    stringIndex = currentGuitarString;
    fretNumber = currentGuitarFret;
}

void PitchDetector::clearInstrumentData()
{
    learnedProfiles.clear();
    learnedSpectraPerNote.clear();
}

void PitchDetector::setMaxPolyphony(int maxNotes)
{
    maxPolyphony = maxNotes;
}

int PitchDetector::getMaxPolyphony() const
{
    return maxPolyphony;
}

bool PitchDetector::isReadyForDetection() const
{
    return !learnedProfiles.empty();
}

void PitchDetector::setNoteDetectionCallback(std::function<void(const std::vector<int>&)> callback)
{
    noteCallback = callback;
}

std::string PitchDetector::midiNoteToName(int midiNote)
{
    static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    
    int octave = (midiNote / 12) - 1;
    int noteIndex = midiNote % 12;
    
    return std::string(noteNames[noteIndex]) + std::to_string(octave);
}
```

# source/dsp/PitchDetector.h

```h
#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <map>
#include <string>

/**
 * PitchDetector class implements polyphonic pitch detection using
 * machine learning techniques to analyze spectral information.
 */
class PitchDetector
{
public:
    /**
     * Constructor
     * @param maxNotes Maximum number of notes to detect at once
     */
    PitchDetector(int maxNotes = 6);
    
    /**
     * Destructor
     */
    ~PitchDetector();
    

    /**
     * Enum for learning mode instrument types
     */
    enum class InstrumentType {
        Generic,
        Guitar,
        Piano,
        Bass
    };
    /**
     * Struct for guitar settings
     */
    struct GuitarSettings {
        // Standard tuning (E, A, D, G, B, E)
        std::vector<int> openStringMidiNotes = {40, 45, 50, 55, 59, 64};
        int numFrets = 24;  // Maximum number of frets to learn
    };
    /**
     * Activates or deactivates learning mode
     * @param shouldBeActive True to enable learning mode, false to disable
     */
    void setLearningModeActive(bool shouldBeActive);
    
    /**
     * Checks if learning mode is active
     * @return True if learning mode is active
     */
    bool isLearningModeActive() const;
    /**
     * Sets the instrument type for learning mode
     * @param type The type of instrument being learned
     */
    void setInstrumentType(InstrumentType type);
    
    /**
     * Gets the current instrument type
     * @return Current instrument type
     */
    InstrumentType getInstrumentType() const;

    /**
     * Gets the current guitar settings
     * @return Current guitar settings
     */
    const GuitarSettings& getGuitarSettings() const;

    /**
     * Sets the guitar settings for guitar mode
     * @param settings Guitar configuration settings
     */
    void setGuitarSettings(const GuitarSettings& settings);

    /**
     * Sets the current monophonic note being learned (when in learning mode)
     * @param midiNote MIDI note number being learned
     */
    void setCurrentLearningNote(int midiNote);
    
    /**
     * Gets the current monophonic note being learned
     * @return Current MIDI note number, or -1 if not set
     */
    int getCurrentLearningNote() const;

    /**
     * Gets the current guitar position
     * @param stringIndex Output parameter for string index
     * @param fretNumber Output parameter for fret number
     */
    void getCurrentGuitarPosition(int& stringIndex, int& fretNumber) const;

    /**
     * Sets the current guitar string and fret for learning (guitar mode only)
     * @param stringIndex String index (0-5, where 0 is the low E string)
     * @param fret Fret number (0 for open string)
     * @return The corresponding MIDI note number
     */
    int setCurrentGuitarPosition(int stringIndex, int fret);
    /**
     * Process a new spectrum for pitch detection or learning
     * @param spectrum Pointer to the magnitude spectrum data
     * @param spectrumSize Size of the spectrum data
     * @return Vector of detected MIDI notes (when not in learning mode)
     */
    std::vector<int> processSpectrum(const float* spectrum, int spectrumSize);
    
    /**
     * Saves learned instrument data to a file
     * @param filePath Path to save the data
     * @return True if successful, false otherwise
     */
    bool saveInstrumentData(const juce::String& filePath);
    
    /**
     * Loads instrument data from a file
     * @param filePath Path to the data file
     * @return True if successful, false otherwise
     */
    bool loadInstrumentData(const juce::String& filePath);
    
    /**
     * Clears all learned instrument data
     */
    void clearInstrumentData();
    
    /**
     * Sets the maximum number of simultaneous notes to detect
     * @param maxNotes New maximum number of notes
     */
    void setMaxPolyphony(int maxNotes);
    
    /**
     * Gets the maximum number of simultaneous notes
     * @return Maximum number of notes
     */
    int getMaxPolyphony() const;
    
    /**
     * Checks if enough data is available for pitch detection
     * @return True if enough data is learned
     */
    bool isReadyForDetection() const;
    
    /**
     * Registers a callback to be called when new notes are detected
     * @param callback Function to call with detected notes
     */
    void setNoteDetectionCallback(std::function<void(const std::vector<int>&)> callback);
    
private:
    struct SpectralProfile {
        int midiNote;
        std::vector<float> spectrum;
        std::string noteName;

        // Guitar-specific information (if applicable)
        int guitarString = -1;
        int guitarFret = -1;
    };
    
    bool learningModeActive;
    int currentLearningNote;
    int maxPolyphony;
    int requiredSpectraForLearning;

    // Instrument type and settings
    InstrumentType instrumentType;
    GuitarSettings guitarSettings;
    
    // Current guitar position (for guitar mode)
    int currentGuitarString;
    int currentGuitarFret;
    
    std::vector<SpectralProfile> learnedProfiles;
    std::map<int, std::vector<std::vector<float>>> learnedSpectraPerNote;
    
    std::function<void(const std::vector<int>&)> noteCallback;
    
    // Methods for spectrum processing and analysis
    std::vector<int> detectPolyphonicPitches(const float* spectrum, int spectrumSize);
    void addLearnedSpectrum(const float* spectrum, int spectrumSize, int midiNote);
    void normalizeVector(std::vector<float>& vec);
    std::vector<float> sparseEncode(const std::vector<float>& input);
    std::string midiNoteToName(int midiNote);
    
    // Constants for pitch detection
    const float minimumCoefficient = 0.1f;   // Minimum coefficient for a note to be detected
    const int maximumSemitoneDistance = 2;   // Maximum semitone distance for note filtering
};
```

# source/dsp/SpectralAnalyzer.cpp

```cpp

```

# source/dsp/SpectralAnalyzer.h

```h

```

# source/gui/CustomPanel.h

```h
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Custom panel class with a dark background
class CustomPanel : public juce::Component
{
public:
    CustomPanel() { setOpaque(true); }
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF1A1A1A));
        g.setColour(juce::Colours::white);
        g.drawText("Test Panel", getLocalBounds(), juce::Justification::centred);
        g.setColour(juce::Colours::red);
        g.drawRect(getLocalBounds(), 1); // Debug border
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomPanel)
};
```

# source/gui/LearningModeComponent.cpp

```cpp

```

# source/gui/LearningModeComponent.h

```h

```

# source/gui/MainComponent.cpp

```cpp

```

# source/gui/MainComponent.h

```h

```

# source/gui/SpectrogramComponent.cpp

```cpp
#include "SpectrogramComponent.h"

SpectrogramComponent::SpectrogramComponent()
    : threshold(0.001f), // Lower threshold to catch smaller amplitudes
      useLogFrequency(true),
      showThreshold(true),
      sampleRate(44100.0f)
{
    // Initialize FFT data
    fftData.resize(kMaxFFTSize, 0.0f);
    peakData.resize(kMaxFFTSize, 0.0f);
    
    // Initialize frequency scale
    for (size_t i = 0; i < frequencyScale.size(); ++i)
    {
        float proportion = static_cast<float>(i) / static_cast<float>(kMaxFFTSize - 1);
        frequencyScale[i] = (useLogFrequency)
            ? kMinFrequency * std::pow(kMaxFrequency / kMinFrequency, proportion)
            : kMinFrequency + (kMaxFrequency - kMinFrequency) * proportion;
    }
    
    // Start the timer for updates
    startTimer(kRefreshRateMs);
    DBG("SpectrogramComponent initialized");
}

SpectrogramComponent::~SpectrogramComponent()
{
    stopTimer();
}

void SpectrogramComponent::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colour(0xFF0A0A0A)); // Very dark background
    
    // Get component dimensions
    int width = getWidth();
    int height = getHeight();
    DBG("SpectrogramComponent paint: width=" << width << ", height=" << height); // Debug bounds
    
    if (width <= 0 || height <= 0)
    {
        g.setColour(juce::Colours::red);
        g.drawText("Invalid Size", getLocalBounds(), juce::Justification::centred);
        return;
    }

    // Draw debug text to confirm rendering
    g.setColour(juce::Colours::white);
    g.drawText("Spectrogram Active", getLocalBounds().withTrimmedBottom(20), juce::Justification::centredTop);
    // Draw grid lines and frequency labels
    g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
    // Octave grid lines (if using log scale)
    if (useLogFrequency)
    {
        for (int freq = static_cast<int>(kMinFrequency); freq <= kMaxFrequency; freq *= 2)
        {
            float x = frequencyToX(static_cast<float>(freq));
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
            g.setColour(juce::Colours::grey);
            juce::String label = (freq >= 1000) ? juce::String(freq / 1000) + "kHz" : juce::String(freq) + "Hz";
            g.drawText(label, static_cast<int>(x) - 20, height - 20, 40, 20, juce::Justification::centred, false);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        }
    }
    else
    {
        for (int freq = 0; freq <= kMaxFrequency; freq += 1000)
        {
            float x = frequencyToX(static_cast<float>(freq));
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
            g.setColour(juce::Colours::grey);
            juce::String label = (freq >= 1000) ? juce::String(freq / 1000) + "kHz" : juce::String(freq) + "Hz";
            g.drawText(label, static_cast<int>(x) - 20, height - 20, 40, 20, juce::Justification::centred, false);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        }
    }
    
    // Amplitude grid lines
    for (float amp = 0.0f; amp <= 1.0f; amp += 0.2f)
    {
        float y = amplitudeToY(amp);
        g.drawHorizontalLine(static_cast<int>(y), 0.0f, static_cast<float>(width));
        g.setColour(juce::Colours::grey);
        juce::String label = juce::String(static_cast<int>(amp * 100)) + "%";
        g.drawText(label, 5, static_cast<int>(y) - 10, 40, 20, juce::Justification::left, false);
        g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
    }
    
    // Draw threshold line if enabled
    if (showThreshold)
    {
        g.setColour(juce::Colour(0xFF9C33FF).withAlpha(0.7f)); // Purple line
        float thresholdY = amplitudeToY(threshold);
        g.drawHorizontalLine(static_cast<int>(thresholdY), 0.0f, static_cast<float>(width));
    }
    // Draw current spectrum (force visibility for debug)
    bool drewSomething = false;
    for (int i = 1; i < static_cast<int>(fftData.size()); ++i)
    {
        if (i >= static_cast<int>(frequencyScale.size()))
            break;
        float prevFreq = frequencyScale[static_cast<size_t>(i - 1)];
        float freq = frequencyScale[static_cast<size_t>(i)];
        float prevAmp = fftData[static_cast<size_t>(i - 1)];
        float amp = fftData[static_cast<size_t>(i)];
        if (amp > 0.0f) // Draw even small amplitudes
        {
            drewSomething = true;
            float x1 = frequencyToX(prevFreq);
            float y1 = amplitudeToY(prevAmp);
            float x2 = frequencyToX(freq);
            float y2 = amplitudeToY(amp);
            juce::Colour color = (amp > threshold)
                ? juce::Colour::fromHSV(0.7f - (amp - threshold) * 0.3f, 0.8f, 1.0f, 1.0f)
                : juce::Colour(0xFF1A1A4A).withAlpha(0.5f);
            g.setColour(color);
            g.drawLine(x1, y1, x2, y2, 2.0f);
        }
    }
    if (!drewSomething)
    {
        g.setColour(juce::Colours::yellow);
        g.drawText("No Data", getLocalBounds(), juce::Justification::centred);
    }
    // Draw waterfall history
    {
        int historySize = static_cast<int>(history.size());
        float historyAlphaStep = 0.8f / static_cast<float>(historySize);
        
        for (int h = 0; h < historySize; ++h)
        {
            float alpha = 0.8f - h * historyAlphaStep;
            const auto& historyData = history[static_cast<size_t>(h)];
            
            // Draw spectrum as a series of vertical lines
            for (int i = 1; i < static_cast<int>(historyData.size()); ++i)
            {
                if (i >= static_cast<int>(frequencyScale.size()))
                    break;
                
                float prevFreq = frequencyScale[static_cast<size_t>(i - 1)];
                float freq = frequencyScale[static_cast<size_t>(i)];
                
                float prevAmp = historyData[static_cast<size_t>(i - 1)];
                float amp = historyData[static_cast<size_t>(i)];
                
                float x1 = frequencyToX(prevFreq);
                float y1 = amplitudeToY(prevAmp);
                float x2 = frequencyToX(freq);
                float y2 = amplitudeToY(amp);
                
                // Color based on amplitude
                juce::Colour color;
                if (amp > threshold)
                {
                    // Above threshold - gradient from blue to purple
                    color = juce::Colour::fromHSV(0.7f - (amp - threshold) * 0.3f, 0.8f, 1.0f, alpha);
                }
                else
                {
                    // Below threshold - dark blue
                    color = juce::Colour(0xFF1A1A4A).withAlpha(alpha * 0.5f);
                }
                
                g.setColour(color);
                g.drawLine(x1, y1, x2, y2, 1.0f);
            }
        }
    }
    
    // Draw current spectrum
    {
        // Draw spectrum as a series of vertical lines
        for (int i = 1; i < static_cast<int>(fftData.size()); ++i)
        {
            if (i >= static_cast<int>(frequencyScale.size()))
                break;
                
            float prevFreq = frequencyScale[static_cast<size_t>(i - 1)];
            float freq = frequencyScale[static_cast<size_t>(i)];
            
            float prevAmp = fftData[static_cast<size_t>(i - 1)];
            float amp = fftData[static_cast<size_t>(i)];
            
            float x1 = frequencyToX(prevFreq);
            float y1 = amplitudeToY(prevAmp);
            float x2 = frequencyToX(freq);
            float y2 = amplitudeToY(amp);
            
            // Color based on amplitude
            juce::Colour color;
            if (amp > threshold)
            {
                // Above threshold - gradient from blue to purple
                color = juce::Colour::fromHSV(0.7f - (amp - threshold) * 0.3f, 0.8f, 1.0f, 1.0f);
            }
            else
            {
                // Below threshold - dark blue
                color = juce::Colour(0xFF1A1A4A).withAlpha(0.5f);
            }
            
            g.setColour(color);
            g.drawLine(x1, y1, x2, y2, 2.0f);
        }
    }
    
    // Draw marked frequencies
    for (const auto& markedFreq : markedFrequencies)
    {
        float x = frequencyToX(markedFreq.frequency);
        
        if (markedFreq.active)
        {
            // Active frequency - draw as a vertical green line
            g.setColour(juce::Colour(0xFF33FF99));
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
            
            // Draw a label
            g.drawText(juce::String(markedFreq.frequency, 1) + "Hz",
                       static_cast<int>(x) - 30, 5, 60, 20,
                       juce::Justification::centred, false);
        }
        else
        {
            // Inactive frequency - draw as a vertical grey line
            g.setColour(juce::Colours::grey);
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
        }
    }
}

void SpectrogramComponent::resized()
{
    // Nothing to do here
}

void SpectrogramComponent::updateFFT(const float* newFFTData, int size)
{
    if (size > kMaxFFTSize)
        size = kMaxFFTSize;
    
    // Copy the new data
    for (int i = 0; i < size; ++i)
    {
        fftData[static_cast<size_t>(i)] = newFFTData[i];
        
        // Update peak data with decay
        peakData[static_cast<size_t>(i)] = std::max(peakData[static_cast<size_t>(i)] * 0.95f, newFFTData[i]);
    }
    
    // Add current data to history
    history.push_front(fftData);
    
    // Limit history size
    while (history.size() > kHistorySize)
    {
        history.pop_back();
    }
    // DEBUG
    DBG("SpectrogramComponent updating with size: " << size);
    // Request a repaint
    repaint();
}

void SpectrogramComponent::setThreshold(float newThreshold)
{
    threshold = juce::jlimit(0.0f, 1.0f, newThreshold);
    repaint();
}

void SpectrogramComponent::setUseLogFrequency(bool shouldUseLogScale)
{
    if (useLogFrequency != shouldUseLogScale)
    {
        useLogFrequency = shouldUseLogScale;
        
        // Update frequency scale
        for (int i = 0; i < kMaxFFTSize; ++i)
        {
            float proportion = static_cast<float>(i) / static_cast<float>(kMaxFFTSize - 1);
            frequencyScale[static_cast<size_t>(i)] = (useLogFrequency)
                ? kMinFrequency * std::pow(kMaxFrequency / kMinFrequency, proportion)
                : kMinFrequency + (kMaxFrequency - kMinFrequency) * proportion;
        }
        
        repaint();
    }
}

void SpectrogramComponent::setShowThreshold(bool shouldShow)
{
    showThreshold = shouldShow;
    repaint();
}

void SpectrogramComponent::markFrequency(float freqHz, bool isActive)
{
    // Check if this frequency is already marked
    for (auto& markedFreq : markedFrequencies)
    {
        if (std::abs(markedFreq.frequency - freqHz) < 0.1f)
        {
            markedFreq.active = isActive;
            repaint();
            return;
        }
    }
    
    // Add new marked frequency
    markedFrequencies.push_back({freqHz, isActive});
    repaint();
}

void SpectrogramComponent::clearMarkedFrequencies()
{
    markedFrequencies.clear();
    repaint();
}

void SpectrogramComponent::timerCallback()
{
    // Trigger a repaint to update any animations
    repaint();
}

float SpectrogramComponent::frequencyToX(float frequency) const
{
    float normX;
    
    if (useLogFrequency)
    {
        // Logarithmic scale
        normX = std::log(frequency / kMinFrequency) / std::log(kMaxFrequency / kMinFrequency);
    }
    else
    {
        // Linear scale
        normX = (frequency - kMinFrequency) / (kMaxFrequency - kMinFrequency);
    }
    
    return normX * static_cast<float>(getWidth());
}

float SpectrogramComponent::amplitudeToY(float amplitude) const
{
    // Invert Y coordinate (0 amplitude at bottom)
    float normY = 1.0f - amplitude / kMaxAmplitude;
    return normY * static_cast<float>(getHeight());
}
```

# source/gui/SpectrogramComponent.h

```h
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <array>
#include <deque>

// A component for visualizing FFT spectral data
class SpectrogramComponent : public juce::Component, 
                             private juce::Timer
{
public:
    SpectrogramComponent();
    ~SpectrogramComponent() override;
    
    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    // Update the component with new FFT data
    void updateFFT(const float* fftData, int size);
    
    // Set the amplitude threshold for display
    void setThreshold(float newThreshold);
    
    // Set whether to use logarithmic frequency scaling
    void setUseLogFrequency(bool shouldUseLogScale);
    
    // Set whether to display the threshold line
    void setShowThreshold(bool shouldShow);
    
    // Mark a specific frequency as "active" (e.g., for learning mode)
    void markFrequency(float freqHz, bool isActive);
    
    // Clear all marked frequencies
    void clearMarkedFrequencies();
    
private:
    //==============================================================================
    void timerCallback() override;
    
    // Convert a frequency value to an x-coordinate
    float frequencyToX(float frequency) const;
    
    // Convert an amplitude value to a y-coordinate
    float amplitudeToY(float amplitude) const;
    
    //==============================================================================
    // Constants
    static constexpr float kMinFrequency = 20.0f;   // Hz
    static constexpr float kMaxFrequency = 20000.0f; // Hz
    static constexpr float kMaxAmplitude = 1.0f;
    static constexpr int kMaxFFTSize = 4096;
    
    // FFT data
    std::vector<float> fftData;
    std::array<float, kMaxFFTSize> frequencyScale;
    
    // Visual settings
    float threshold;
    bool useLogFrequency;
    bool showThreshold;
    
    // Marked frequencies (e.g., for learning mode)
    struct MarkedFreq {
        float frequency;
        bool active;
    };
    std::vector<MarkedFreq> markedFrequencies;
    
    // Sample rate
    float sampleRate;
    
    // Decay buffer for smoother visualization
    std::vector<float> peakData;
    
    // Display history for waterfall effect
    std::deque<std::vector<float>> history;
    static constexpr int kHistorySize = 20;
    
    // Refresh rate
    static constexpr int kRefreshRateMs = 30;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent)
};
```

# source/midi/MIDIManager.cpp

```cpp
#include "MIDIManager.h"

MIDIManager::MIDIManager()
    : midiChannel(1),
      midiVelocity(100),
      noteOnDelaySamples(0),
      noteOffDelaySamples(0),
      sampleRate(44100.0)
{
    setNoteOnDelayMs(50);   // 50ms delay before sending note-on
    setNoteOffDelayMs(100); // 100ms delay before sending note-off
}

MIDIManager::~MIDIManager()
{
}

void MIDIManager::processNotes(const std::vector<int>& detectedNotes, juce::MidiBuffer& midiBuffer, int sampleNumber)
{
    // Convert detected notes to a set for easier comparison
    std::set<int> currentNotes(detectedNotes.begin(), detectedNotes.end());
    
    // Process pending note-offs
    std::vector<int> notesToRemove;
    
    for (auto& pair : pendingNoteOffs)
    {
        int note = pair.first;
        int& samplesRemaining = pair.second;
        
        // If the note is detected again, remove it from pending note-offs
        if (currentNotes.find(note) != currentNotes.end())
        {
            notesToRemove.push_back(note);
            continue;
        }
        
        // Decrement counter
        samplesRemaining--;
        
        // If countdown is complete, send note-off
        if (samplesRemaining <= 0)
        {
            if (activeNotes.find(note) != activeNotes.end())
            {
                juce::MidiMessage noteOff = juce::MidiMessage::noteOff(midiChannel, note, 0.0f);
                midiBuffer.addEvent(noteOff, sampleNumber);
                activeNotes.erase(note);
            }
            notesToRemove.push_back(note);
        }
    }
    
    // Remove processed note-offs
    for (int note : notesToRemove)
    {
        pendingNoteOffs.erase(note);
    }
    
    // Find notes to turn off (notes that were active but are no longer detected)
    for (int note : activeNotes)
    {
        if (currentNotes.find(note) == currentNotes.end() && 
            pendingNoteOffs.find(note) == pendingNoteOffs.end())
        {
            // Add to pending note-offs
            pendingNoteOffs[note] = noteOffDelaySamples;
        }
    }
    
    // Process pending note-ons
    std::vector<int> notesToSend;
    
    for (int note : currentNotes)
    {
        // If the note is already active, skip it
        if (activeNotes.find(note) != activeNotes.end())
            continue;
        
        // If the note is not in pending note-ons yet, add it
        if (pendingNoteOns.find(note) == pendingNoteOns.end())
        {
            pendingNoteOns.insert(note);
            continue;
        }
        
        // If the note has been pending long enough, send note-on
        if (pendingNoteOns.find(note) != pendingNoteOns.end())
        {
            notesToSend.push_back(note);
        }
    }
    
    // Send note-on messages for validated notes
    for (int note : notesToSend)
    {
        juce::MidiMessage noteOn = juce::MidiMessage::noteOn(midiChannel, note, (float)midiVelocity / 127.0f);
        midiBuffer.addEvent(noteOn, sampleNumber);
        activeNotes.insert(note);
        pendingNoteOns.erase(note);
    }
    
    // Remove notes from pending note-ons if they're no longer detected
    std::vector<int> pendingToRemove;
    for (int note : pendingNoteOns)
    {
        if (currentNotes.find(note) == currentNotes.end())
        {
            pendingToRemove.push_back(note);
        }
    }
    
    for (int note : pendingToRemove)
    {
        pendingNoteOns.erase(note);
    }
}

void MIDIManager::reset(juce::MidiBuffer& midiBuffer, int sampleNumber)
{
    // Send note-off messages for all active notes
    for (int note : activeNotes)
    {
        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(midiChannel, note, 0.0f);
        midiBuffer.addEvent(noteOff, sampleNumber);
    }
    
    // Clear all states
    activeNotes.clear();
    pendingNoteOns.clear();
    pendingNoteOffs.clear();
}

void MIDIManager::setMidiChannel(int channel)
{
    midiChannel = juce::jlimit(1, 16, channel);
}

void MIDIManager::setMidiVelocity(int velocity)
{
    midiVelocity = juce::jlimit(0, 127, velocity);
}

void MIDIManager::setNoteOnDelayMs(int ms)
{
    double delaySeconds = ms / 1000.0;
    noteOnDelaySamples = static_cast<int>(delaySeconds * sampleRate);
}

void MIDIManager::setNoteOffDelayMs(int ms)
{
    double delaySeconds = ms / 1000.0;
    noteOffDelaySamples = static_cast<int>(delaySeconds * sampleRate);
}

void MIDIManager::updateSampleRate(double newSampleRate)
{
    // Avoid floating point comparison warning by checking if the difference is significant
    if (std::abs(sampleRate - newSampleRate) > 0.001)
    {
        sampleRate = newSampleRate;
        
        // Recalculate sample delays
        setNoteOnDelayMs(static_cast<int>(noteOnDelaySamples * 1000.0 / sampleRate));
        setNoteOffDelayMs(static_cast<int>(noteOffDelaySamples * 1000.0 / sampleRate));
    }
}
```

# source/midi/MIDIManager.h

```h
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <set>
#include <vector>

/**
 * MIDIManager handles the generation and management of MIDI messages
 * for pitch detection output.
 */
class MIDIManager
{
public:
    /**
     * Constructor
     */
    MIDIManager();
    
    /**
     * Destructor
     */
    ~MIDIManager();
    
    /**
     * Process a new set of detected notes, generating MIDI messages as needed
     * @param detectedNotes Vector of MIDI note numbers
     * @param midiBuffer MIDI buffer to add messages to
     * @param sampleNumber Sample position for the MIDI messages
     */
    void processNotes(const std::vector<int>& detectedNotes, juce::MidiBuffer& midiBuffer, int sampleNumber);
    
    /**
     * Resets the manager, turning off all active notes
     * @param midiBuffer MIDI buffer to add note-off messages to
     * @param sampleNumber Sample position for the MIDI messages
     */
    void reset(juce::MidiBuffer& midiBuffer, int sampleNumber);
    
    /**
     * Sets the MIDI channel for output
     * @param channel MIDI channel (1-16)
     */
    void setMidiChannel(int channel);
    
    /**
     * Sets the MIDI velocity for note-on messages
     * @param velocity MIDI velocity (0-127)
     */
    void setMidiVelocity(int velocity);
    
    /**
     * Sets the minimum time in milliseconds a note must be detected 
     * before sending a note-on message
     * @param ms Time in milliseconds
     */
    void setNoteOnDelayMs(int ms);
    
    /**
     * Sets the minimum time in milliseconds a note must be absent
     * before sending a note-off message
     * @param ms Time in milliseconds
     */
    void setNoteOffDelayMs(int ms);

    void updateSampleRate(double newSampleRate);
    
private:
    std::set<int> activeNotes;         // Currently active (sounding) notes
    std::set<int> pendingNoteOns;      // Notes waiting to be turned on
    std::map<int, int> pendingNoteOffs; // Notes waiting to be turned off <note, samplesRemaining>
    
    int midiChannel;
    int midiVelocity;
    
    int noteOnDelaySamples;
    int noteOffDelaySamples;
    double sampleRate;
    
    /**
     * Updates the sample rate dependent parameters
     * @param newSampleRate New sample rate in Hz
     */
};
```

# source/midi/MIDIProcessor.cpp

```cpp

```

# source/midi/MIDIProcessor.h

```h

```

# source/PluginEditor.cpp

```cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "gui/CustomPanel.h"



PolyphonicTrackerAudioProcessorEditor::PolyphonicTrackerAudioProcessorEditor(PolyphonicTrackerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&darkTheme);
    setSize(800, 600);

    jayImage = juce::ImageCache::getFromFile(juce::File("resources/images/jay.jpg"));
    if (jayImage.isNull())
        DBG("Failed to load jay.jpg");
    else
        DBG("Successfully loaded jay.jpg");

    if (!jayImage.isNull())
        jayImageComponent = std::make_unique<JayImageComponent>(jayImage);
    else
        jayImageComponent = std::make_unique<JayImageComponent>(juce::Image(juce::Image::RGB, 200, 200, true));

    addAndMakeVisible(tabbedComponent);
    tabbedComponent.setOutline(0);
    tabbedComponent.setTabBarDepth(35);

    auto* mainPanel = new CustomPanel();
    auto* guitarPanel = new CustomPanel();
    auto* visualPanel = new CustomPanel();

    tabbedComponent.addTab("Controls", juce::Colour(0xFF111111), mainPanel, true);
    tabbedComponent.addTab("Guitar Mode", juce::Colour(0xFF111111), guitarPanel, true);
    tabbedComponent.addTab("Visualization", juce::Colour(0xFF111111), visualPanel, true);

    mainPanel->addAndMakeVisible(learningModeToggle);
    visualPanel->addAndMakeVisible(*jayImageComponent);
    mainPanel->setOpaque(true);
    guitarPanel->setOpaque(true);
    visualPanel->setOpaque(true);

    guitarPanel->addAndMakeVisible(guitarStringCombo);
    guitarStringCombo.addItemList({"E (low)", "A", "D", "G", "B", "E (high)"}, 1);
    guitarStringCombo.setSelectedItemIndex(0);
    guitarPanel->addAndMakeVisible(guitarStringLabel);

    guitarFretSlider.setRange(0, 24, 1);
    guitarFretSlider.setValue(0);
    guitarFretSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    guitarFretSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    guitarPanel->addAndMakeVisible(guitarFretSlider);
    guitarPanel->addAndMakeVisible(guitarFretLabel);

    guitarPanel->addAndMakeVisible(learnFretButton);
    guitarPanel->addAndMakeVisible(learningStatusLabel);

    spectrogramComponent = std::make_unique<SpectrogramComponent>();
    visualPanel->addAndMakeVisible(*spectrogramComponent);

    audioProcessor.setFFTDataCallback([this](const float* data, int size) {
        DBG("Received FFT data, size: " << size);
        if (spectrogramComponent)
            spectrogramComponent->updateFFT(data, size);
    });

    guitarStringAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getParameterTree(), "guitarString", guitarStringCombo);

    guitarFretAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameterTree(), "guitarFret", guitarFretSlider);

    learnFretButton.onClick = [this]() {
        int string = guitarStringCombo.getSelectedItemIndex();
        int fret = static_cast<int>(guitarFretSlider.getValue());
        int midiNote = audioProcessor.setCurrentGuitarPosition(string, fret);
        audioProcessor.setLearningModeActive(true);
        audioProcessor.setCurrentLearningNote(midiNote);
        learningStatusLabel.setText("Learning: String " + juce::String(string + 1) + ", Fret " + juce::String(fret) +
                                   " (MIDI: " + juce::String(midiNote) + ")",
                                   juce::dontSendNotification);
    };

    currentNoteSlider.setRange(21, 108, 1);
    currentNoteSlider.setValue(60);
    currentNoteSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    currentNoteSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(currentNoteSlider);
    mainPanel->addAndMakeVisible(currentNoteLabel);

    maxPolyphonySlider.setRange(1, 8, 1);
    maxPolyphonySlider.setValue(6);
    maxPolyphonySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    maxPolyphonySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(maxPolyphonySlider);
    mainPanel->addAndMakeVisible(maxPolyphonyLabel);

    midiChannelSlider.setRange(1, 16, 1);
    midiChannelSlider.setValue(1);
    midiChannelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    midiChannelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(midiChannelSlider);
    mainPanel->addAndMakeVisible(midiChannelLabel);

    midiVelocitySlider.setRange(0, 127, 1);
    midiVelocitySlider.setValue(100);
    midiVelocitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    midiVelocitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(midiVelocitySlider);
    mainPanel->addAndMakeVisible(midiVelocityLabel);

    noteOnDelaySlider.setRange(0, 500, 1);
    noteOnDelaySlider.setValue(50);
    noteOnDelaySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noteOnDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(noteOnDelaySlider);
    mainPanel->addAndMakeVisible(noteOnDelayLabel);

    noteOffDelaySlider.setRange(0, 500, 1);
    noteOffDelaySlider.setValue(100);
    noteOffDelaySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noteOffDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(noteOffDelaySlider);
    mainPanel->addAndMakeVisible(noteOffDelayLabel);

    learningModeToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    currentNoteLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    maxPolyphonyLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    midiChannelLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    midiVelocityLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    noteOnDelayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    noteOffDelayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    guitarStringLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    guitarFretLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    learningStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    guitarStringCombo.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    guitarStringCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF222222));
    guitarStringCombo.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xFF9C33FF));

    learnFretButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF333333));
    learnFretButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

    auto& params = audioProcessor.getParameterTree();

    learningModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        params, "learningMode", learningModeToggle);

    currentNoteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "currentNote", currentNoteSlider);

    maxPolyphonyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "maxPolyphony", maxPolyphonySlider);

    midiChannelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "midiChannel", midiChannelSlider);

    midiVelocityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "midiVelocity", midiVelocitySlider);

    noteOnDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "noteOnDelay", noteOnDelaySlider);

    noteOffDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "noteOffDelay", noteOffDelaySlider);

    learningModeToggle.onClick = [this]() {
        bool isLearningMode = learningModeToggle.getToggleState();
        bool isGuitarMode = tabbedComponent.getCurrentTabIndex() == 1;

        if (isLearningMode && isGuitarMode) {
            if (spectrogramComponent)
                spectrogramComponent->clearMarkedFrequencies();

            int string = guitarStringCombo.getSelectedItemIndex();
            int fret = static_cast<int>(guitarFretSlider.getValue());
            learningStatusLabel.setText("Learning Guitar: String " + juce::String(string + 1) + ", Fret " + juce::String(fret),
                                       juce::dontSendNotification);
        } else if (isLearningMode) {
            learningStatusLabel.setText("Learning Mode: Play single notes", juce::dontSendNotification);
        } else {
            learningStatusLabel.setText("Detection Mode: Play normally", juce::dontSendNotification);
        }
    };

    startTimer(500);
}

PolyphonicTrackerAudioProcessorEditor::~PolyphonicTrackerAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void PolyphonicTrackerAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Do not fill the entire area; let the TabbedComponent handle its background
}

void PolyphonicTrackerAudioProcessorEditor::timerCallback()
{
    static int lastTabIndex = -1;
    int currentTabIndex = tabbedComponent.getCurrentTabIndex();

    if (currentTabIndex != lastTabIndex)
    {
        lastTabIndex = currentTabIndex;

        bool isLearningMode = learningModeToggle.getToggleState();
        bool isGuitarMode = currentTabIndex == 1;

        if (isLearningMode && isGuitarMode) {
            if (spectrogramComponent)
                spectrogramComponent->clearMarkedFrequencies();

            int string = guitarStringCombo.getSelectedItemIndex();
            int fret = static_cast<int>(guitarFretSlider.getValue());
            learningStatusLabel.setText("Learning Guitar: String " + juce::String(string + 1) + ", Fret " + juce::String(fret),
                                       juce::dontSendNotification);
        } else if (isLearningMode) {
            learningStatusLabel.setText("Learning Mode: Play single notes", juce::dontSendNotification);
        } else {
            learningStatusLabel.setText("Detection Mode: Play normally", juce::dontSendNotification);
        }
    }
}
void PolyphonicTrackerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    tabbedComponent.setBounds(area);

    if (auto* mainPanel = tabbedComponent.getTabContentComponent(0))
    {
        const int margin = 20;
        const int labelWidth = 150;
        const int controlHeight = 30;
        int y = margin * 2;

        DBG("MainPanel bounds: " << mainPanel->getBounds().toString());
        learningModeToggle.setBounds(margin, y, 150, controlHeight);
        learningModeToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white); // Ensure visible
        y += controlHeight + margin;

        currentNoteLabel.setBounds(margin, y, labelWidth, controlHeight);
        currentNoteLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        currentNoteLabel.setText("Current Note Test", juce::dontSendNotification); // Debug text
        currentNoteSlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        DBG("CurrentNoteSlider bounds: " << currentNoteSlider.getBounds().toString());
        y += controlHeight + margin;

        maxPolyphonyLabel.setBounds(margin, y, labelWidth, controlHeight);
        maxPolyphonySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        DBG("MaxPolyphonySlider bounds: " << maxPolyphonySlider.getBounds().toString());
        y += controlHeight + margin;

        midiChannelLabel.setBounds(margin, y, labelWidth, controlHeight);
        midiChannelSlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        DBG("MidiChannelSlider bounds: " << midiChannelSlider.getBounds().toString());
        y += controlHeight + margin;

        midiVelocityLabel.setBounds(margin, y, labelWidth, controlHeight);
        midiVelocitySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        DBG("MidiVelocitySlider bounds: " << midiVelocitySlider.getBounds().toString());
        y += controlHeight + margin;

        noteOnDelayLabel.setBounds(margin, y, labelWidth, controlHeight);
        noteOnDelaySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        DBG("NoteOnDelaySlider bounds: " << noteOnDelaySlider.getBounds().toString());
        y += controlHeight + margin;

        noteOffDelayLabel.setBounds(margin, y, labelWidth, controlHeight);
        noteOffDelaySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        DBG("NoteOffDelaySlider bounds: " << noteOffDelaySlider.getBounds().toString());
    }

    if (auto* guitarPanel = tabbedComponent.getTabContentComponent(1))
    {
        const int margin = 20;
        const int labelWidth = 150;
        const int controlHeight = 30;
        int y = margin * 2;

        DBG("GuitarPanel bounds: " << guitarPanel->getBounds().toString());
        guitarStringLabel.setBounds(margin, y, labelWidth, controlHeight);
        guitarStringCombo.setBounds(margin + labelWidth, y, 200, controlHeight);
        DBG("GuitarStringCombo bounds: " << guitarStringCombo.getBounds().toString());
        y += controlHeight + margin;

        guitarFretLabel.setBounds(margin, y, labelWidth, controlHeight);
        guitarFretSlider.setBounds(margin + labelWidth, y, guitarPanel->getWidth() - margin * 3 - labelWidth, controlHeight);
        DBG("GuitarFretSlider bounds: " << guitarFretSlider.getBounds().toString());
        y += controlHeight + margin * 2;

        learnFretButton.setBounds(margin, y, 200, 40);
        DBG("LearnFretButton bounds: " << learnFretButton.getBounds().toString());
        y += 50;

        learningStatusLabel.setBounds(margin, y, guitarPanel->getWidth() - margin * 2, 40);
        DBG("LearningStatusLabel bounds: " << learningStatusLabel.getBounds().toString());
    }

    if (auto* visualPanel = tabbedComponent.getTabContentComponent(2))
    {
        auto bounds = visualPanel->getLocalBounds().reduced(20);
        auto topSection = bounds.removeFromTop(bounds.getHeight() / 2);

        if (jayImageComponent)
        {
            jayImageComponent->setBounds(topSection);
            DBG("JayImageComponent bounds: " << jayImageComponent->getBounds().toString());
        }

        if (spectrogramComponent)
        {
            spectrogramComponent->setBounds(bounds);
            DBG("SpectrogramComponent bounds: " << spectrogramComponent->getBounds().toString());
        }
    }
}
```

# source/PluginEditor.h

```h
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "gui/SpectrogramComponent.h"


// Custom dark theme
class DarkPurpleTheme : public juce::LookAndFeel_V4
{
public:
    DarkPurpleTheme()
    {
        // Set colors based on your theme
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xFF000000)); // Pure black
        setColour(juce::TabbedComponent::backgroundColourId, juce::Colour(0xFF0C0C0C)); // Dark gray
        setColour(juce::TabbedButtonBar::tabOutlineColourId, juce::Colour(0xFF333333)); // Light gray border
        setColour(juce::TabbedButtonBar::frontOutlineColourId, juce::Colour(0xFFAA33FF)); // Purple highlight
        // Add more colors for components
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF222222));
        setColour(juce::ComboBox::textColourId, juce::Colours::white);
        setColour(juce::ComboBox::arrowColourId, juce::Colour(0xFF9C33FF));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF444444));

        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF222222));
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF444444));

        // More colors for text
        setColour(juce::TextEditor::textColourId, juce::Colours::white);
        setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF222222));
        // Text colors
        setColour(juce::Label::textColourId, juce::Colours::white);
        
        // Slider colors
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFF9C33FF)); // Purple thumb
        setColour(juce::Slider::trackColourId, juce::Colour(0xFF333333)); // Dark track
        setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF1A1A1A)); // Very dark gray
        
        // Button colors
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF1A1A1A));
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF9C33FF));
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        
        // Toggle button colors
        setColour(juce::ToggleButton::tickColourId, juce::Colour(0xFF9C33FF));
        setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xFF666666));
    }
};

// Custom image component
class JayImageComponent : public juce::Component
{
public:
    JayImageComponent(const juce::Image& img) : image(img) {}
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.drawImage(image, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
    }
    
private:
    juce::Image image;
};

//==============================================================================
class PolyphonicTrackerAudioProcessorEditor : public juce::AudioProcessorEditor,
private juce::Timer
{
public:
    explicit PolyphonicTrackerAudioProcessorEditor(PolyphonicTrackerAudioProcessor&);
    ~PolyphonicTrackerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;


private:
    // UI mode update method
    void updateUIMode();
    
    // Reference to the processor
    PolyphonicTrackerAudioProcessor& audioProcessor;

    // Custom theme
    DarkPurpleTheme darkTheme;
    
    // Image of Jay
    juce::Image jayImage;
    std::unique_ptr<JayImageComponent> jayImageComponent;
    
    // UI Components
    juce::TabbedComponent tabbedComponent {juce::TabbedButtonBar::TabsAtTop};
    
    // Learning mode controls
    juce::ToggleButton learningModeToggle {"Learning Mode"};
    juce::Slider currentNoteSlider;
    juce::Label currentNoteLabel {"", "Current Note:"};
    
    // Guitar mode controls
    juce::ComboBox guitarStringCombo;
    juce::Label guitarStringLabel {"", "String:"};
    juce::Slider guitarFretSlider;
    juce::Label guitarFretLabel {"", "Fret:"};
    juce::TextButton learnFretButton {"Learn This Position"};
    juce::Label learningStatusLabel {"", ""};
    
    // Spectrogram component
    std::unique_ptr<SpectrogramComponent> spectrogramComponent;

    // MIDI output controls
    juce::Slider maxPolyphonySlider;
    juce::Label maxPolyphonyLabel {"", "Max Polyphony:"};
    
    juce::Slider midiChannelSlider;
    juce::Label midiChannelLabel {"", "MIDI Channel:"};
    
    juce::Slider midiVelocitySlider;
    juce::Label midiVelocityLabel {"", "MIDI Velocity:"};
    
    juce::Slider noteOnDelaySlider;
    juce::Label noteOnDelayLabel {"", "Note On Delay (ms):"};
    
    juce::Slider noteOffDelaySlider;
    juce::Label noteOffDelayLabel {"", "Note Off Delay (ms):"};
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> learningModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> currentNoteAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> maxPolyphonyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midiChannelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midiVelocityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noteOnDelayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noteOffDelayAttachment;

    // Parameter attachments for guitar mode
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> guitarStringAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> guitarFretAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolyphonicTrackerAudioProcessorEditor)
};
```

# source/PluginProcessor.cpp

```cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/FFTProcessor.h"
#include "dsp/PitchDetector.h"
#include "midi/MIDIManager.h"

//for debuggig
#include <fstream>
std::ofstream debugLog("/tmp/polyphonic_tracker_debug.log"); // macOS path
#define DBG(message) debugLog << message << std::endl
//==============================================================================
PolyphonicTrackerAudioProcessor::PolyphonicTrackerAudioProcessor()
    : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout()),
      currentFFTSize(4096),
      currentOverlapFactor(0.75f),
      numFrets(24),
      currentGuitarString(0),
      currentGuitarFret(0),
      instrumentType(0)
{
    // Initialize DSP components
    fftProcessor = std::make_unique<FFTProcessor>(currentFFTSize);
    pitchDetector = std::make_unique<PitchDetector>(6); // Default to 6 notes of polyphony
    midiManager = std::make_unique<MIDIManager>();
    
    // Set up FFT processor callback
    fftProcessor->setSpectrumDataCallback([this](const float* spectrum, int size) {
        handleNewFFTBlock(spectrum, size);
    });
    
    // Initialize parameters
    learningModeParam = parameters.getRawParameterValue("learningMode");
    currentNoteParam = parameters.getRawParameterValue("currentNote");
    maxPolyphonyParam = parameters.getRawParameterValue("maxPolyphony");
    midiChannelParam = parameters.getRawParameterValue("midiChannel");
    midiVelocityParam = parameters.getRawParameterValue("midiVelocity");
    noteOnDelayParam = parameters.getRawParameterValue("noteOnDelay");
    noteOffDelayParam = parameters.getRawParameterValue("noteOffDelay");
    
    // Start the timer for GUI updates
    startTimerHz(30); // 30 updates per second
}

PolyphonicTrackerAudioProcessor::~PolyphonicTrackerAudioProcessor()
{
    stopTimer();
}

//==============================================================================
const juce::String PolyphonicTrackerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PolyphonicTrackerAudioProcessor::acceptsMidi() const
{
    return false;
}

bool PolyphonicTrackerAudioProcessor::producesMidi() const
{
    return true;
}

bool PolyphonicTrackerAudioProcessor::isMidiEffect() const
{
    return false;
}

double PolyphonicTrackerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PolyphonicTrackerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PolyphonicTrackerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PolyphonicTrackerAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused(index);
}

const juce::String PolyphonicTrackerAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused(index);
    return {};
}

void PolyphonicTrackerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void PolyphonicTrackerAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    fftProcessor->reset();
    pitchDetector->setMaxPolyphony(static_cast<int>(*maxPolyphonyParam));
    pitchDetector->setLearningModeActive(*learningModeParam > 0.5f);
    pitchDetector->setCurrentLearningNote(static_cast<int>(*currentNoteParam));
    
    midiManager->setMidiChannel(static_cast<int>(*midiChannelParam));
    midiManager->setMidiVelocity(static_cast<int>(*midiVelocityParam));
    midiManager->setNoteOnDelayMs(static_cast<int>(*noteOnDelayParam));
    midiManager->setNoteOffDelayMs(static_cast<int>(*noteOffDelayParam));
    midiManager->updateSampleRate(sampleRate); // Add this line
}

void PolyphonicTrackerAudioProcessor::releaseResources()
{
    // Release any resources when playback stops
}

bool PolyphonicTrackerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // We only support mono or stereo inputs
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // We require at least one input channel
    if (layouts.getMainInputChannelSet().size() < 1)
        return false;
    
    // Input and output layouts should match
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    
    return true;
}

void PolyphonicTrackerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    // Clear output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Clear the incoming MIDI buffer as we'll be generating our own MIDI
    midiMessages.clear();

    // Extract mono input from the buffer (average if stereo)
    juce::AudioBuffer<float> monoBuffer(1, buffer.getNumSamples());
    monoBuffer.clear();
    
    // Mix down to mono
    if (totalNumInputChannels == 1)
    {
        monoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
    }
    else
    {
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            monoBuffer.addFrom(0, 0, buffer, ch, 0, buffer.getNumSamples(), 1.0f / totalNumInputChannels);
        }
    }
    
    // Process the mono buffer through FFT
    bool fftPerformed = fftProcessor->processBlock(monoBuffer.getReadPointer(0), monoBuffer.getNumSamples());
    if (fftPerformed)
        DBG("FFT performed successfully");
    // If an FFT was performed, handle the detected notes directly
    if (fftPerformed)
    {
        auto spectrum = fftProcessor->getMagnitudeSpectrum();
        auto detectedNotes = pitchDetector->processSpectrum(spectrum, fftProcessor->getSpectrumSize());
        
        // Process the detected notes into MIDI events
        midiManager->processNotes(detectedNotes, midiMessages, 0);
    }
    
    // Pass through the audio
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // No audio processing, just pass through
        juce::ignoreUnused(buffer.getWritePointer(channel));
    }
}

//==============================================================================
bool PolyphonicTrackerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PolyphonicTrackerAudioProcessor::createEditor()
{
    return new PolyphonicTrackerAudioProcessorEditor(*this);
    // We'll create a custom editor later
    // return new PolyphonicTrackerAudioProcessorEditor(*this);
}

//==============================================================================
void PolyphonicTrackerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Store parameters
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PolyphonicTrackerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore parameters
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
void PolyphonicTrackerAudioProcessor::handleNewFFTBlock(const float* fftData, int fftSize)
{
    DBG("Handling new FFT block, size: " << fftSize);
    if (fftData) // Check for null
    {
        for (int i = 0; i < fftSize; ++i)
            DBG("FFT[" << i << "] = " << fftData[i]); // Debug first few values
    }
    auto detectedNotes = pitchDetector->processSpectrum(fftData, fftSize);
    
    juce::MidiBuffer tempBuffer;
    midiManager->processNotes(detectedNotes, tempBuffer, 0);
    
    if (fftDataCallback)
    {
        DBG("Calling FFT data callback with size: " << fftSize);
        fftDataCallback(fftData, fftSize);
    }
}

void PolyphonicTrackerAudioProcessor::timerCallback()
{
    // Update parameters from GUI
    if (pitchDetector != nullptr)
    {
        bool learningMode = *learningModeParam > 0.5f;
        int currentNote = static_cast<int>(*currentNoteParam);
        int maxPolyphony = static_cast<int>(*maxPolyphonyParam);
        
        pitchDetector->setLearningModeActive(learningMode);
        pitchDetector->setCurrentLearningNote(currentNote);
        pitchDetector->setMaxPolyphony(maxPolyphony);
    }
    
    if (midiManager != nullptr)
    {
        int midiChannel = static_cast<int>(*midiChannelParam);
        int midiVelocity = static_cast<int>(*midiVelocityParam);
        int noteOnDelay = static_cast<int>(*noteOnDelayParam);
        int noteOffDelay = static_cast<int>(*noteOffDelayParam);
        
        midiManager->setMidiChannel(midiChannel);
        midiManager->setMidiVelocity(midiVelocity);
        midiManager->setNoteOnDelayMs(noteOnDelay);
        midiManager->setNoteOffDelayMs(noteOffDelay);
    }
}

void PolyphonicTrackerAudioProcessor::setFFTDataCallback(std::function<void(const float*, int)> callback)
{
    fftDataCallback = callback;
}

const float* PolyphonicTrackerAudioProcessor::getLatestFFTData() const
{
    return fftProcessor ? fftProcessor->getMagnitudeSpectrum() : nullptr;
}

int PolyphonicTrackerAudioProcessor::getLatestFFTSize() const
{
    return fftProcessor ? fftProcessor->getSpectrumSize() : 0;
}



void PolyphonicTrackerAudioProcessor::setInstrumentType(PitchDetector::InstrumentType type)
{
    pitchDetector->setInstrumentType(type);
}

PitchDetector::InstrumentType PolyphonicTrackerAudioProcessor::getInstrumentType() const
{
    return pitchDetector->getInstrumentType();
}

void PolyphonicTrackerAudioProcessor::setGuitarSettings(const PitchDetector::GuitarSettings& settings)
{
    pitchDetector->setGuitarSettings(settings);
}

const PitchDetector::GuitarSettings& PolyphonicTrackerAudioProcessor::getGuitarSettings() const
{
    return pitchDetector->getGuitarSettings();
}

int PolyphonicTrackerAudioProcessor::setCurrentGuitarPosition(int stringIndex, int fret)
{
    if (pitchDetector != nullptr)
        return pitchDetector->setCurrentGuitarPosition(stringIndex, fret);
    return 60; // Default to middle C if detector isn't ready
}

void PolyphonicTrackerAudioProcessor::getCurrentGuitarPosition(int& stringIndex, int& fretNumber) const
{
    pitchDetector->getCurrentGuitarPosition(stringIndex, fretNumber);
}

juce::AudioProcessorValueTreeState::ParameterLayout PolyphonicTrackerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Learning mode switch
    layout.add(std::make_unique<juce::AudioParameterBool>(
        "learningMode", "Learning Mode", false));
    
    // Current note for learning (MIDI note number)
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "currentNote", "Current Note", 21, 108, 60));
    
    // Maximum polyphony
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "maxPolyphony", "Max Polyphony", 1, 16, 6));
    
    // Add guitar mode parameters
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "guitarString", "Guitar String", 0, 5, 0));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "guitarFret", "Guitar Fret", 0, 24, 0));

    // MIDI output parameters
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "midiChannel", "MIDI Channel", 1, 16, 1));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "midiVelocity", "MIDI Velocity", 0, 127, 100));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "noteOnDelay", "Note On Delay (ms)", 0, 500, 50));
    
    layout.add(std::make_unique<juce::AudioParameterInt>(
        "noteOffDelay", "Note Off Delay (ms)", 0, 500, 100));
    
    return layout;
}

//==============================================================================
// Polyphonic tracker parameter methods
//==============================================================================
void PolyphonicTrackerAudioProcessor::setLearningModeActive(bool shouldBeActive)
{
    if (pitchDetector != nullptr)
        pitchDetector->setLearningModeActive(shouldBeActive);
    
    *parameters.getRawParameterValue("learningMode") = shouldBeActive ? 1.0f : 0.0f;
}

bool PolyphonicTrackerAudioProcessor::isLearningModeActive() const
{
    return pitchDetector != nullptr && pitchDetector->isLearningModeActive();
}

void PolyphonicTrackerAudioProcessor::setCurrentLearningNote(int midiNote)
{
    if (pitchDetector != nullptr)
        pitchDetector->setCurrentLearningNote(midiNote);
    
    *parameters.getRawParameterValue("currentNote") = static_cast<float>(midiNote);
}

int PolyphonicTrackerAudioProcessor::getCurrentLearningNote() const
{
    return pitchDetector != nullptr ? pitchDetector->getCurrentLearningNote() : -1;
}

void PolyphonicTrackerAudioProcessor::setMaxPolyphony(int maxNotes)
{
    if (pitchDetector != nullptr)
        pitchDetector->setMaxPolyphony(maxNotes);
    
    *parameters.getRawParameterValue("maxPolyphony") = static_cast<float>(maxNotes);
}

int PolyphonicTrackerAudioProcessor::getMaxPolyphony() const
{
    return pitchDetector != nullptr ? pitchDetector->getMaxPolyphony() : 6;
}

bool PolyphonicTrackerAudioProcessor::saveInstrumentData(const juce::String& filePath)
{
    return pitchDetector != nullptr && pitchDetector->saveInstrumentData(filePath);
}

bool PolyphonicTrackerAudioProcessor::loadInstrumentData(const juce::String& filePath)
{
    return pitchDetector != nullptr && pitchDetector->loadInstrumentData(filePath);
}

void PolyphonicTrackerAudioProcessor::setMidiChannel(int channel)
{
    if (midiManager != nullptr)
        midiManager->setMidiChannel(channel);
    
    *parameters.getRawParameterValue("midiChannel") = static_cast<float>(channel);
}

void PolyphonicTrackerAudioProcessor::setMidiVelocity(int velocity)
{
    if (midiManager != nullptr)
        midiManager->setMidiVelocity(velocity);
    
    *parameters.getRawParameterValue("midiVelocity") = static_cast<float>(velocity);
}

void PolyphonicTrackerAudioProcessor::setNoteOnDelayMs(int ms)
{
    if (midiManager != nullptr)
        midiManager->setNoteOnDelayMs(ms);
    
    *parameters.getRawParameterValue("noteOnDelay") = static_cast<float>(ms);
}

void PolyphonicTrackerAudioProcessor::setNoteOffDelayMs(int ms)
{
    if (midiManager != nullptr)
        midiManager->setNoteOffDelayMs(ms);
    
    *parameters.getRawParameterValue("noteOffDelay") = static_cast<float>(ms);
}

void PolyphonicTrackerAudioProcessor::setFFTSize(int fftSize)
{
    if (fftSize != currentFFTSize && fftProcessor != nullptr)
    {
        currentFFTSize = fftSize;
        fftProcessor.reset(new FFTProcessor(fftSize));
        fftProcessor->setOverlapFactor(currentOverlapFactor);
        
        // Reconnect the FFT callback
        fftProcessor->setSpectrumDataCallback([this](const float* spectrum, int size) {
            handleNewFFTBlock(spectrum, size);
        });
    }
}

int PolyphonicTrackerAudioProcessor::getFFTSize() const
{
    return currentFFTSize;
}

void PolyphonicTrackerAudioProcessor::setFFTOverlap(float overlapFactor)
{
    currentOverlapFactor = overlapFactor;
    
    if (fftProcessor != nullptr)
    {
        fftProcessor->setOverlapFactor(overlapFactor);
    }
}

float PolyphonicTrackerAudioProcessor::getFFTOverlap() const
{
    return currentOverlapFactor;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolyphonicTrackerAudioProcessor();
}
```

# source/PluginProcessor.h

```h
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "dsp/PitchDetector.h"

// Forward declarations
class FFTProcessor;
class PitchDetector;
class MIDIManager;

//==============================================================================
class PolyphonicTrackerAudioProcessor : public juce::AudioProcessor, public juce::Timer
{
public:
    //==============================================================================
    PolyphonicTrackerAudioProcessor();
    ~PolyphonicTrackerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getParameterTree() { return parameters; }
    
    // FFT Visualization support
    void setFFTDataCallback(std::function<void(const float*, int)> callback);
    const float* getLatestFFTData() const;
    int getLatestFFTSize() const;


    //==============================================================================
    // Learning mode
    void setLearningModeActive(bool shouldBeActive);
    bool isLearningModeActive() const;
    
    // Learning note
    void setCurrentLearningNote(int midiNote);
    int getCurrentLearningNote() const;
    
    // Set max polyphony
    void setMaxPolyphony(int maxNotes);
    int getMaxPolyphony() const;  // Add this line


    // Instrument type
    void setInstrumentType(PitchDetector::InstrumentType type);
    
    PitchDetector::InstrumentType getInstrumentType() const;


    // Guitar-specific learning
    void setGuitarSettings(const PitchDetector::GuitarSettings& settings);
    int setCurrentGuitarPosition(int stringIndex, int fret);
    void getCurrentGuitarPosition(int& stringIndex, int& fretNumber) const;
    const PitchDetector::GuitarSettings& getGuitarSettings() const;
    
    // Guitar settings access
    const juce::StringArray& getOpenStringMidiNotes() const;
    int getNumFrets() const;
    
    // Save/load instrument data
    bool saveInstrumentData(const juce::String& filePath);
    bool loadInstrumentData(const juce::String& filePath);
    
    // Parameters for MIDI output
    void setMidiChannel(int channel);
    void setMidiVelocity(int velocity);
    void setNoteOnDelayMs(int ms);
    void setNoteOffDelayMs(int ms);
    
    // Processor settings
    void setFFTSize(int fftSize);
    int getFFTSize() const;
    
    void setFFTOverlap(float overlapFactor);
    float getFFTOverlap() const;
    void timerCallback() override; // Declare the virtual method

private:
    //==============================================================================
    // Create parameter layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // Process the FFT results and handle pitch detection
    void handleNewFFTBlock(const float* fftData, int fftSize);
    
    //==============================================================================
    // Parameter storage
    juce::AudioProcessorValueTreeState parameters;
    
    // DSP components
    std::unique_ptr<FFTProcessor> fftProcessor;
    std::unique_ptr<PitchDetector> pitchDetector;
    std::unique_ptr<MIDIManager> midiManager;
    
    // FFT visualization support
    std::function<void(const float*, int)> fftDataCallback;
    
    // Internal state
    int currentFFTSize;
    float currentOverlapFactor;
    
    // Guitar settings
    juce::StringArray openStringMidiNotes;
    int numFrets;
    int currentGuitarString;
    int currentGuitarFret;
    int instrumentType;
    
    // Parameter pointers
    std::atomic<float>* learningModeParam = nullptr;
    std::atomic<float>* currentNoteParam = nullptr;
    std::atomic<float>* maxPolyphonyParam = nullptr;
    std::atomic<float>* midiChannelParam = nullptr;
    std::atomic<float>* midiVelocityParam = nullptr;
    std::atomic<float>* noteOnDelayParam = nullptr;
    std::atomic<float>* noteOffDelayParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyphonicTrackerAudioProcessor)
};
```

# source/utils/Constants.h

```h

```

# source/utils/DSPUtils.cpp

```cpp

```

# source/utils/DSPUtils.h

```h

```

# tests/CMakeLists.txt

```txt
enable_testing()

# Add test executable
add_executable(PolyphonicTrackerTests
    TestMain.cpp
    PitchDetectionTests.cpp
    FFTProcessorTests.cpp
)

# Link with main project and testing framework
target_link_libraries(PolyphonicTrackerTests
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
)

# Add tests to CTest
add_test(NAME PolyphonicTrackerTests COMMAND PolyphonicTrackerTests)
```

# tests/FFTProcessorTests.cpp

```cpp

```

# tests/PitchDetectionTests.cpp

```cpp

```

# tests/TestMain.cpp

```cpp

```

# tree.sh

```sh
tree -I "build/|build-mac-apple-silicon/|libs/"

```

