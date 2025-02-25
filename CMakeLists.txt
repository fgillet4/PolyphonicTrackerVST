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

# Add source files
target_sources(PolyphonicTrackerVST
    PRIVATE
        source/PluginProcessor.cpp
        source/PluginEditor.cpp
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