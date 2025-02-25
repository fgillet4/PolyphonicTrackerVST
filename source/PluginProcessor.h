#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// Include our DSP components
#include "dsp/FFTProcessor.h"
#include "dsp/PitchDetector.h"
#include "midi/MIDIManager.h"

class PolyphonicTrackerAudioProcessor : public juce::AudioProcessor,
                                        private juce::Timer
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
    // Polyphonic tracker parameters
    
    // Learning mode
    void setLearningModeActive(bool shouldBeActive);
    bool isLearningModeActive() const;
    
    // Learning note
    void setCurrentLearningNote(int midiNote);
    int getCurrentLearningNote() const;
    
    // Set max polyphony
    void setMaxPolyphony(int maxNotes);
    int getMaxPolyphony() const;
    
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
    
private:
    //==============================================================================
    // Timer callback for periodic updates
    void timerCallback() override;
    
    // Process the FFT results and handle pitch detection
    void handleNewFFTBlock(const float* fftData, int fftSize);
    
    // Create parameter layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    //==============================================================================
    // Parameter storage
    juce::AudioProcessorValueTreeState parameters;
    
    // DSP components
    std::unique_ptr<FFTProcessor> fftProcessor;
    std::unique_ptr<PitchDetector> pitchDetector;
    std::unique_ptr<MIDIManager> midiManager;
    
    // Internal state
    int currentFFTSize;
    float currentOverlapFactor;
    
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