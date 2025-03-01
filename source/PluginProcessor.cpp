#include "PluginProcessor.h"
#include "dsp/FFTProcessor.h"
#include "dsp/PitchDetector.h"
#include "midi/MIDIManager.h"
// Forward declaration of the editor class - include happens later
class PolyphonicTrackerAudioProcessorEditor;

//for debugging
#include <fstream>
static std::ofstream debugLog("/tmp/polyphonic_tracker_debug.log"); // macOS path
// Use the JUCE DBG macro instead of redefining it
#define CUSTOM_LOG(message) debugLog << message << std::endl
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
// In PluginProcessor.cpp
void PolyphonicTrackerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Clear the incoming MIDI buffer as we'll be generating our own MIDI
    midiMessages.clear();

    // Get total samples
    auto numSamples = buffer.getNumSamples();
    
    // Reduce processing load by only processing every Nth block
    static int blockCounter = 0;
    blockCounter = (blockCounter + 1) % 8; // Increased to every 8th block
    
    if (blockCounter == 0 && fftProcessor != nullptr)
    {
        // Thread safety check - use atomic flag to avoid callback crashes
        static std::atomic<bool> processingFFT{false};
        
        if (!processingFFT.exchange(true))
        {
            try
            {
                // Mix down to mono more efficiently
                juce::AudioBuffer<float> monoBuffer(1, numSamples);
                monoBuffer.clear();
                
                if (totalNumInputChannels == 1)
                {
                    monoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
                }
                else
                {
                    // Mix down to mono using only first two channels
                    float scaleFactor = 1.0f / std::min(2, totalNumInputChannels);
                    for (int ch = 0; ch < std::min(2, totalNumInputChannels); ++ch)
                    {
                        monoBuffer.addFrom(0, 0, buffer, ch, 0, numSamples, scaleFactor);
                    }
                }
                
                // Process the mono buffer through FFT with safety checks
                bool fftPerformed = fftProcessor->processBlock(monoBuffer.getReadPointer(0), numSamples);
                
                // If FFT was performed, generate MIDI
                if (fftPerformed && pitchDetector != nullptr && midiManager != nullptr)
                {
                    auto spectrum = fftProcessor->getMagnitudeSpectrum();
                    auto spectrumSize = fftProcessor->getSpectrumSize();
                    
                    if (spectrum != nullptr && spectrumSize > 0)
                    {
                        // Only process pitch detection if we have meaningful data
                        bool hasSignal = false;
                        for (int i = 0; i < std::min(spectrumSize, 10); i++) {
                            if (spectrum[i] > 0.001f) {
                                hasSignal = true;
                                break;
                            }
                        }
                        
                        if (hasSignal)
                        {
                            auto detectedNotes = pitchDetector->processSpectrum(spectrum, spectrumSize);
                            if (!detectedNotes.empty())
                            {
                                midiManager->processNotes(detectedNotes, midiMessages, 0);
                            }
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                // Log error but don't crash
                juce::Logger::writeToLog("Error in processBlock: " + juce::String(e.what()));
            }
            
            processingFFT = false;
        }
    }
    
    // Always pass audio through unchanged
}

//==============================================================================
bool PolyphonicTrackerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

#include "PluginEditor.h"

juce::AudioProcessorEditor* PolyphonicTrackerAudioProcessor::createEditor()
{
    return new PolyphonicTrackerAudioProcessorEditor(*this);
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
    if (fftData == nullptr || fftSize <= 0)
        return;  // Safety check
        
    // Process the FFT data through the pitch detector
    auto detectedNotes = pitchDetector->processSpectrum(fftData, fftSize);
    
    // Create a temporary MIDI buffer for the notes
    juce::MidiBuffer tempBuffer;
    
    // Process the detected notes and generate MIDI
    midiManager->processNotes(detectedNotes, tempBuffer, 0);
    
    // Call the FFT data callback if registered, with additional safety
    if (fftDataCallback && fftData != nullptr && fftSize > 0)
    {
        try {
            fftDataCallback(fftData, fftSize);
        }
        catch (const std::exception& e) {
            CUSTOM_LOG("Exception in FFT callback: " << e.what());
        }
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