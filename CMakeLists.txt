cmake_minimum_required(VERSION 3.15)
project(PolyphonicTrackerVST VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory (you'll need to add JUCE as a submodule)
# add_subdirectory(libs/JUCE)

# Include subdirectories
add_subdirectory(source)
add_subdirectory(tests)
