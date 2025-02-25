#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolyphonicTrackerAudioProcessor::PolyphonicTrackerAudioProcessor()
    : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout()),
      currentFFTSize(4096),
      currentOverlapFactor(0.75f)
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
void PolyphonicTrackerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Configure DSP components with the current sample rate
    fftProcessor->reset();
    pitchDetector->setMaxPolyphony(static_cast<int>(*maxPolyphonyParam));
    pitchDetector->setLearningModeActive(*learningModeParam > 0.5f);
    pitchDetector->setCurrentLearningNote(static_cast<int>(*currentNoteParam));
    
    midiManager->setMidiChannel(static_cast<int>(*midiChannelParam));
    midiManager->setMidiVelocity(static_cast<int>(*midiVelocityParam));
    midiManager->setNoteOnDelayMs(static_cast<int>(*noteOnDelayParam));
    midiManager->setNoteOffDelayMs(static_cast<int>(*noteOffDelayParam));
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
    
    // The MIDI messages will be sent in the next processBlock call
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