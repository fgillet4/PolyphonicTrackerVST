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
    
    // Get the callback lock for thread safety
    juce::CriticalSection& getCallbackLock() { return callbackLock; }
    
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
    
    // Set background and foreground colors
    void setColours(juce::Colour background, juce::Colour foreground);
    
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
    
    // Thread safety
    juce::CriticalSection callbackLock;
    
    // Custom colors
    juce::Colour backgroundColour;
    juce::Colour foregroundColour;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent)
};