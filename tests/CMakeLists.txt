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