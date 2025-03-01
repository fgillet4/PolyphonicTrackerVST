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
    // In PluginProcessor.h
    void logDebugState() const
    {
        juce::String state = "PolyphonicTracker state:";
        state += "\n - Learning mode: " + juce::String(isLearningModeActive() ? "ON" : "OFF");
        state += "\n - Current note: " + juce::String(getCurrentLearningNote());
        state += "\n - Max polyphony: " + juce::String(getMaxPolyphony());
        state += "\n - FFT size: " + juce::String(getFFTSize());
        state += "\n - FFT overlap: " + juce::String(getFFTOverlap());
        
        juce::Logger::writeToLog(state);
    }

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