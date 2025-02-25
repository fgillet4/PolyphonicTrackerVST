#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class PolyphonicTrackerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PolyphonicTrackerAudioProcessorEditor (PolyphonicTrackerAudioProcessor&);
    ~PolyphonicTrackerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PolyphonicTrackerAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyphonicTrackerAudioProcessorEditor)
};