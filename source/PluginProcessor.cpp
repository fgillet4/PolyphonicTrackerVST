#include "PluginProcessor.h"
#include "PluginEditor.h"

PolyphonicTrackerAudioProcessor::PolyphonicTrackerAudioProcessor()
    : juce::AudioProcessor (juce::AudioProcessor::BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

PolyphonicTrackerAudioProcessor::~PolyphonicTrackerAudioProcessor()
{
}

void PolyphonicTrackerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void PolyphonicTrackerAudioProcessor::releaseResources()
{
}

void PolyphonicTrackerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                   juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // For now, just pass through the audio
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        // Process channel data here
    }
}

juce::AudioProcessorEditor* PolyphonicTrackerAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

void PolyphonicTrackerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void PolyphonicTrackerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolyphonicTrackerAudioProcessor();
}