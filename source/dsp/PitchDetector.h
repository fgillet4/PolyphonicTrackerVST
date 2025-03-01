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
     * Enum for learning mode instrument types
     */
    enum class InstrumentType {
        Generic,
        Guitar,
        Piano,
        Bass
    };
    /**
     * Struct for guitar settings
     */
    struct GuitarSettings {
        // Standard tuning (E, A, D, G, B, E)
        std::vector<int> openStringMidiNotes = {40, 45, 50, 55, 59, 64};
        int numFrets = 24;  // Maximum number of frets to learn
    };
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
     * Sets the instrument type for learning mode
     * @param type The type of instrument being learned
     */
    void setInstrumentType(InstrumentType type);
    
    /**
     * Gets the current instrument type
     * @return Current instrument type
     */
    InstrumentType getInstrumentType() const;

    /**
     * Gets the current guitar settings
     * @return Current guitar settings
     */
    const GuitarSettings& getGuitarSettings() const;

    /**
     * Sets the guitar settings for guitar mode
     * @param settings Guitar configuration settings
     */
    void setGuitarSettings(const GuitarSettings& settings);

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
     * Gets the current guitar position
     * @param stringIndex Output parameter for string index
     * @param fretNumber Output parameter for fret number
     */
    void getCurrentGuitarPosition(int& stringIndex, int& fretNumber) const;

    /**
     * Sets the current guitar string and fret for learning (guitar mode only)
     * @param stringIndex String index (0-5, where 0 is the low E string)
     * @param fret Fret number (0 for open string)
     * @return The corresponding MIDI note number
     */
    int setCurrentGuitarPosition(int stringIndex, int fret);
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

        // Guitar-specific information (if applicable)
        int guitarString = -1;
        int guitarFret = -1;
    };
    
    bool learningModeActive;
    int currentLearningNote;
    int maxPolyphony;
    int requiredSpectraForLearning;

    // Instrument type and settings
    InstrumentType instrumentType;
    GuitarSettings guitarSettings;
    
    // Current guitar position (for guitar mode)
    int currentGuitarString;
    int currentGuitarFret;
    
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