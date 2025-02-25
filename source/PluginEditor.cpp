#include "PluginProcessor.h"
#include "PluginEditor.h"

PolyphonicTrackerAudioProcessorEditor::PolyphonicTrackerAudioProcessorEditor(PolyphonicTrackerAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processorRef(p)
{
    setSize (400, 300);
}

PolyphonicTrackerAudioProcessorEditor::~PolyphonicTrackerAudioProcessorEditor()
{
}

void PolyphonicTrackerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PolyphonicTrackerAudioProcessorEditor::resized()
{
}