#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <set>
#include <vector>

/**
 * MIDIManager handles the generation and management of MIDI messages
 * for pitch detection output.
 */
class MIDIManager
{
public:
    /**
     * Constructor
     */
    MIDIManager();
    
    /**
     * Destructor
     */
    ~MIDIManager();
    
    /**
     * Process a new set of detected notes, generating MIDI messages as needed
     * @param detectedNotes Vector of MIDI note numbers
     * @param midiBuffer MIDI buffer to add messages to
     * @param sampleNumber Sample position for the MIDI messages
     */
    void processNotes(const std::vector<int>& detectedNotes, juce::MidiBuffer& midiBuffer, int sampleNumber);
    
    /**
     * Resets the manager, turning off all active notes
     * @param midiBuffer MIDI buffer to add note-off messages to
     * @param sampleNumber Sample position for the MIDI messages
     */
    void reset(juce::MidiBuffer& midiBuffer, int sampleNumber);
    
    /**
     * Sets the MIDI channel for output
     * @param channel MIDI channel (1-16)
     */
    void setMidiChannel(int channel);
    
    /**
     * Sets the MIDI velocity for note-on messages
     * @param velocity MIDI velocity (0-127)
     */
    void setMidiVelocity(int velocity);
    
    /**
     * Sets the minimum time in milliseconds a note must be detected 
     * before sending a note-on message
     * @param ms Time in milliseconds
     */
    void setNoteOnDelayMs(int ms);
    
    /**
     * Sets the minimum time in milliseconds a note must be absent
     * before sending a note-off message
     * @param ms Time in milliseconds
     */
    void setNoteOffDelayMs(int ms);
    
private:
    std::set<int> activeNotes;         // Currently active (sounding) notes
    std::set<int> pendingNoteOns;      // Notes waiting to be turned on
    std::map<int, int> pendingNoteOffs; // Notes waiting to be turned off <note, samplesRemaining>
    
    int midiChannel;
    int midiVelocity;
    
    int noteOnDelaySamples;
    int noteOffDelaySamples;
    double sampleRate;
    
    /**
     * Updates the sample rate dependent parameters
     * @param newSampleRate New sample rate in Hz
     */
    void updateSampleRate(double newSampleRate);
};