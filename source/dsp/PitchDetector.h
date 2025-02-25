#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <map>
#include <string>

/**
 * PitchDetector class implements polyphonic pitch detection using
 * machine learning techniques to analyze spectral information.
 */
class PitchDetector
{
public:
    /**
     * Constructor
     * @param maxNotes Maximum number of notes to detect at once
     */
    PitchDetector(int maxNotes = 6);
    
    /**
     * Destructor
     */
    ~PitchDetector();
    
    /**
     * Activates or deactivates learning mode
     * @param shouldBeActive True to enable learning mode, false to disable
     */
    void setLearningModeActive(bool shouldBeActive);
    
    /**
     * Checks if learning mode is active
     * @return True if learning mode is active
     */
    bool isLearningModeActive() const;
    
    /**
     * Sets the current monophonic note being learned (when in learning mode)
     * @param midiNote MIDI note number being learned
     */
    void setCurrentLearningNote(int midiNote);
    
    /**
     * Gets the current monophonic note being learned
     * @return Current MIDI note number, or -1 if not set
     */
    int getCurrentLearningNote() const;
    
    /**
     * Process a new spectrum for pitch detection or learning
     * @param spectrum Pointer to the magnitude spectrum data
     * @param spectrumSize Size of the spectrum data
     * @return Vector of detected MIDI notes (when not in learning mode)
     */
    std::vector<int> processSpectrum(const float* spectrum, int spectrumSize);
    
    /**
     * Saves learned instrument data to a file
     * @param filePath Path to save the data
     * @return True if successful, false otherwise
     */
    bool saveInstrumentData(const juce::String& filePath);
    
    /**
     * Loads instrument data from a file
     * @param filePath Path to the data file
     * @return True if successful, false otherwise
     */
    bool loadInstrumentData(const juce::String& filePath);
    
    /**
     * Clears all learned instrument data
     */
    void clearInstrumentData();
    
    /**
     * Sets the maximum number of simultaneous notes to detect
     * @param maxNotes New maximum number of notes
     */
    void setMaxPolyphony(int maxNotes);
    
    /**
     * Gets the maximum number of simultaneous notes
     * @return Maximum number of notes
     */
    int getMaxPolyphony() const;
    
    /**
     * Checks if enough data is available for pitch detection
     * @return True if enough data is learned
     */
    bool isReadyForDetection() const;
    
    /**
     * Registers a callback to be called when new notes are detected
     * @param callback Function to call with detected notes
     */
    void setNoteDetectionCallback(std::function<void(const std::vector<int>&)> callback);
    
private:
    struct SpectralProfile {
        int midiNote;
        std::vector<float> spectrum;
        std::string noteName;
    };
    
    bool learningModeActive;
    int currentLearningNote;
    int maxPolyphony;
    int requiredSpectraForLearning;
    
    std::vector<SpectralProfile> learnedProfiles;
    std::map<int, std::vector<std::vector<float>>> learnedSpectraPerNote;
    
    std::function<void(const std::vector<int>&)> noteCallback;
    
    // Methods for spectrum processing and analysis
    std::vector<int> detectPolyphonicPitches(const float* spectrum, int spectrumSize);
    void addLearnedSpectrum(const float* spectrum, int spectrumSize, int midiNote);
    void normalizeVector(std::vector<float>& vec);
    std::vector<float> sparseEncode(const std::vector<float>& input);
    std::string midiNoteToName(int midiNote);
    
    // Constants for pitch detection
    const float minimumCoefficient = 0.1f;   // Minimum coefficient for a note to be detected
    const int maximumSemitoneDistance = 2;   // Maximum semitone distance for note filtering
};