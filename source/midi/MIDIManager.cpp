#include "MIDIManager.h"

MIDIManager::MIDIManager()
    : midiChannel(1),
      midiVelocity(100),
      noteOnDelaySamples(0),
      noteOffDelaySamples(0),
      sampleRate(44100.0)
{
    setNoteOnDelayMs(50);   // 50ms delay before sending note-on
    setNoteOffDelayMs(100); // 100ms delay before sending note-off
}

MIDIManager::~MIDIManager()
{
}

void MIDIManager::processNotes(const std::vector<int>& detectedNotes, juce::MidiBuffer& midiBuffer, int sampleNumber)
{
    // Convert detected notes to a set for easier comparison
    std::set<int> currentNotes(detectedNotes.begin(), detectedNotes.end());
    
    // Process pending note-offs
    std::vector<int> notesToRemove;
    
    for (auto& pair : pendingNoteOffs)
    {
        int note = pair.first;
        int& samplesRemaining = pair.second;
        
        // If the note is detected again, remove it from pending note-offs
        if (currentNotes.find(note) != currentNotes.end())
        {
            notesToRemove.push_back(note);
            continue;
        }
        
        // Decrement counter
        samplesRemaining--;
        
        // If countdown is complete, send note-off
        if (samplesRemaining <= 0)
        {
            if (activeNotes.find(note) != activeNotes.end())
            {
                juce::MidiMessage noteOff = juce::MidiMessage::noteOff(midiChannel, note, 0.0f);
                midiBuffer.addEvent(noteOff, sampleNumber);
                activeNotes.erase(note);
            }
            notesToRemove.push_back(note);
        }
    }
    
    // Remove processed note-offs
    for (int note : notesToRemove)
    {
        pendingNoteOffs.erase(note);
    }
    
    // Find notes to turn off (notes that were active but are no longer detected)
    for (int note : activeNotes)
    {
        if (currentNotes.find(note) == currentNotes.end() && 
            pendingNoteOffs.find(note) == pendingNoteOffs.end())
        {
            // Add to pending note-offs
            pendingNoteOffs[note] = noteOffDelaySamples;
        }
    }
    
    // Process pending note-ons
    std::vector<int> notesToSend;
    
    for (int note : currentNotes)
    {
        // If the note is already active, skip it
        if (activeNotes.find(note) != activeNotes.end())
            continue;
        
        // If the note is not in pending note-ons yet, add it
        if (pendingNoteOns.find(note) == pendingNoteOns.end())
        {
            pendingNoteOns.insert(note);
            continue;
        }
        
        // If the note has been pending long enough, send note-on
        if (pendingNoteOns.find(note) != pendingNoteOns.end())
        {
            notesToSend.push_back(note);
        }
    }
    
    // Send note-on messages for validated notes
    for (int note : notesToSend)
    {
        juce::MidiMessage noteOn = juce::MidiMessage::noteOn(midiChannel, note, (float)midiVelocity / 127.0f);
        midiBuffer.addEvent(noteOn, sampleNumber);
        activeNotes.insert(note);
        pendingNoteOns.erase(note);
    }
    
    // Remove notes from pending note-ons if they're no longer detected
    std::vector<int> pendingToRemove;
    for (int note : pendingNoteOns)
    {
        if (currentNotes.find(note) == currentNotes.end())
        {
            pendingToRemove.push_back(note);
        }
    }
    
    for (int note : pendingToRemove)
    {
        pendingNoteOns.erase(note);
    }
}

void MIDIManager::reset(juce::MidiBuffer& midiBuffer, int sampleNumber)
{
    // Send note-off messages for all active notes
    for (int note : activeNotes)
    {
        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(midiChannel, note, 0.0f);
        midiBuffer.addEvent(noteOff, sampleNumber);
    }
    
    // Clear all states
    activeNotes.clear();
    pendingNoteOns.clear();
    pendingNoteOffs.clear();
}

void MIDIManager::setMidiChannel(int channel)
{
    midiChannel = juce::jlimit(1, 16, channel);
}

void MIDIManager::setMidiVelocity(int velocity)
{
    midiVelocity = juce::jlimit(0, 127, velocity);
}

void MIDIManager::setNoteOnDelayMs(int ms)
{
    double delaySeconds = ms / 1000.0;
    noteOnDelaySamples = static_cast<int>(delaySeconds * sampleRate);
}

void MIDIManager::setNoteOffDelayMs(int ms)
{
    double delaySeconds = ms / 1000.0;
    noteOffDelaySamples = static_cast<int>(delaySeconds * sampleRate);
}

void MIDIManager::updateSampleRate(double newSampleRate)
{
    // Avoid floating point comparison warning by checking if the difference is significant
    if (std::abs(sampleRate - newSampleRate) > 0.001)
    {
        sampleRate = newSampleRate;
        
        // Recalculate sample delays
        setNoteOnDelayMs(static_cast<int>(noteOnDelaySamples * 1000.0 / sampleRate));
        setNoteOffDelayMs(static_cast<int>(noteOffDelaySamples * 1000.0 / sampleRate));
    }
}