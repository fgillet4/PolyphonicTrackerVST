#include "PitchDetector.h"

PitchDetector::PitchDetector(int maxNotes)
    : learningModeActive(false),
      currentLearningNote(-1),
      maxPolyphony(maxNotes),
      requiredSpectraForLearning(10),
      instrumentType(InstrumentType::Generic),
      currentGuitarString(0),
      currentGuitarFret(0)
{
    // Set default guitar settings
    guitarSettings.openStringMidiNotes = {40, 45, 50, 55, 59, 64}; // E2, A2, D3, G3, B3, E4
    guitarSettings.numFrets = 24;
}

PitchDetector::~PitchDetector()
{
}

void PitchDetector::setLearningModeActive(bool shouldBeActive)
{
    learningModeActive = shouldBeActive;
}

bool PitchDetector::isLearningModeActive() const
{
    return learningModeActive;
}

void PitchDetector::setCurrentLearningNote(int midiNote)
{
    currentLearningNote = midiNote;
}

int PitchDetector::getCurrentLearningNote() const
{
    return currentLearningNote;
}

void PitchDetector::setGuitarSettings(const GuitarSettings& settings)
{
    guitarSettings = settings;
}

void PitchDetector::setInstrumentType(InstrumentType type)
{
    instrumentType = type;
}

const PitchDetector::GuitarSettings& PitchDetector::getGuitarSettings() const
{
    return guitarSettings;
}

PitchDetector::InstrumentType PitchDetector::getInstrumentType() const
{
    return instrumentType;
}

std::vector<int> PitchDetector::processSpectrum(const float* spectrum, int spectrumSize)
{
    std::vector<int> detectedNotes;
    
    // Convert input spectrum to vector
    std::vector<float> spectrumVec(spectrum, spectrum + spectrumSize);
    normalizeVector(spectrumVec);
    
    if (learningModeActive && currentLearningNote >= 0)
    {
        // Learning mode: store the spectrum for the current note
        addLearnedSpectrum(spectrum, spectrumSize, currentLearningNote);
        return detectedNotes; // Return empty vector in learning mode
    }
    else if (isReadyForDetection())
    {
        // Detection mode: perform polyphonic pitch detection
        detectedNotes = detectPolyphonicPitches(spectrum, spectrumSize);
        
        // Call the callback if registered
        if (noteCallback && !detectedNotes.empty())
        {
            noteCallback(detectedNotes);
        }
    }
    
    return detectedNotes;
}

void PitchDetector::addLearnedSpectrum(const float* spectrumData, int spectrumSize, int midiNote)
{
    // Create a vector from the spectrum data
    std::vector<float> spectrumVec(spectrumData, spectrumData + spectrumSize);
    normalizeVector(spectrumVec);
    
    // Add to the learned spectra for this note
    learnedSpectraPerNote[midiNote].push_back(spectrumVec);
    
    // If we have enough spectra for this note, create an average profile
    if (static_cast<int>(learnedSpectraPerNote[midiNote].size()) >= requiredSpectraForLearning)
    {
        // Calculate the average spectrum for this note
        std::vector<float> avgSpectrum(static_cast<size_t>(spectrumSize), 0.0f);        
        for (const auto& learnedSpectrum : learnedSpectraPerNote[midiNote])
        {
            for (int i = 0; i < spectrumSize; ++i)
            {
                avgSpectrum[static_cast<size_t>(i)] += learnedSpectrum[static_cast<size_t>(i)];            }
        }
        
        // Normalize the average
        for (float& value : avgSpectrum)
        {
            value /= learnedSpectraPerNote[midiNote].size();
        }
        
        normalizeVector(avgSpectrum);
        
        // Store as a learned profile
        SpectralProfile profile;
        profile.midiNote = midiNote;
        profile.spectrum = avgSpectrum;
        profile.noteName = midiNoteToName(midiNote);
        
        // Remove any existing profile for this note
        learnedProfiles.erase(
            std::remove_if(learnedProfiles.begin(), learnedProfiles.end(),
                          [midiNote](const SpectralProfile& p) { return p.midiNote == midiNote; }),
            learnedProfiles.end());
        
        // Add the new profile
        learnedProfiles.push_back(profile);
    }
}

std::vector<int> PitchDetector::detectPolyphonicPitches(const float* spectrum, int spectrumSize)
{
    if (learnedProfiles.empty())
    {
        return {};
    }
    
    // Convert input spectrum to vector
    std::vector<float> inputSpectrum(spectrum, spectrum + spectrumSize);
    normalizeVector(inputSpectrum);
    
    // Perform sparse encoding to find the most similar learned profiles
    std::vector<float> coefficients = sparseEncode(inputSpectrum);
    
    // Sort the coefficients and find the indices of the top values
    std::vector<std::pair<float, int>> coefPairs;
    for (size_t i = 0; i < coefficients.size(); ++i)
    {
        coefPairs.emplace_back(coefficients[i], static_cast<int>(i));
    }
    
    // Sort in descending order of coefficient values
    std::sort(coefPairs.begin(), coefPairs.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select the top notes, up to maxPolyphony
    std::vector<int> detectedNotes;
    
    for (int i = 0; i < std::min(maxPolyphony, static_cast<int>(coefPairs.size())); ++i)
    {
        // Only include notes with coefficients above the threshold
        if (coefPairs[static_cast<size_t>(i)].first >= minimumCoefficient)
        {
            int profileIndex = coefPairs[static_cast<size_t>(i)].second;
            int midiNote = learnedProfiles[static_cast<size_t>(profileIndex)].midiNote;
            
            // Check for octave errors or close notes (avoid duplicates)
            bool tooClose = false;
            for (int existingNote : detectedNotes)
            {
                int semitoneDistance = std::abs(existingNote - midiNote);
                if (semitoneDistance < maximumSemitoneDistance)
                {
                    tooClose = true;
                    break;
                }
            }
            
            if (!tooClose)
            {
                detectedNotes.push_back(midiNote);
            }
        }
    }
    
    return detectedNotes;
}

void PitchDetector::normalizeVector(std::vector<float>& vec)
{
    // Calculate the L2 norm (Euclidean length) of the vector
    float sumSquares = 0.0f;
    for (float val : vec)
    {
        sumSquares += val * val;
    }
    
    if (sumSquares > 0.0f)
    {
        float norm = std::sqrt(sumSquares);
        for (float& val : vec)
        {
            val /= norm;
        }
    }
}

std::vector<float> PitchDetector::sparseEncode(const std::vector<float>& input)
{
    // Simple implementation of sparse encoding using cosine similarity
    // In a more advanced implementation, this would use an L1-regularized solver
    
    std::vector<float> coefficients;
    
    for (const auto& profile : learnedProfiles)
    {
        // Calculate the dot product (cosine similarity for normalized vectors)
        float similarity = 0.0f;
        for (size_t i = 0; i < std::min(input.size(), profile.spectrum.size()); ++i)
        {
            similarity += input[i] * profile.spectrum[i];
        }
        
        coefficients.push_back(similarity);
    }
    
    return coefficients;
}

bool PitchDetector::saveInstrumentData(const juce::String& filePath)
{
    // Use simpler file-based approach for now
    juce::File file(filePath);
    juce::FileOutputStream outStream(file);
    
    if (!outStream.openedOk())
        return false;
    
    // Write the number of profiles
    outStream.writeInt(static_cast<int>(learnedProfiles.size()));
    
    for (const auto& profile : learnedProfiles)
    {
        // Write MIDI note
        outStream.writeInt(profile.midiNote);
        
        // Write note name length and string
        outStream.writeInt(static_cast<int>(profile.noteName.length()));        
        // Write spectrum size and data
        outStream.writeInt(static_cast<int>(profile.spectrum.size()));
        for (float val : profile.spectrum)
        {
            outStream.writeFloat(val);
        }
    }
    
    return true;
}

bool PitchDetector::loadInstrumentData(const juce::String& filePath)
{
    juce::File file(filePath);
    juce::FileInputStream inStream(file);
    
    if (!inStream.openedOk())
        return false;
    
    clearInstrumentData();
    
    // Read number of profiles
    int numProfiles = inStream.readInt();
    
    for (int i = 0; i < numProfiles; ++i)
    {
        SpectralProfile profile;
        
        // Read MIDI note
        profile.midiNote = inStream.readInt();
        
        // Read note name
        
        // Read spectrum
        int spectrumSize = inStream.readInt();
        profile.spectrum.resize(static_cast<size_t>(spectrumSize));        
        for (int j = 0; j < spectrumSize; ++j)
        {
            profile.spectrum[static_cast<size_t>(j)] = inStream.readFloat();        }
        
        learnedProfiles.push_back(profile);
    }
    
    return true;
}


int PitchDetector::setCurrentGuitarPosition(int stringIndex, int fret)
{
    currentGuitarString = stringIndex;
    currentGuitarFret = fret;
    
    // Calculate MIDI note based on string and fret
    if (stringIndex >= 0 && stringIndex < static_cast<int>(guitarSettings.openStringMidiNotes.size()))
    {
        int baseMidiNote = guitarSettings.openStringMidiNotes[static_cast<size_t>(stringIndex)];
        int midiNote = baseMidiNote + fret;
        return midiNote;
    }
    
    return 60; // Default to middle C if string is invalid
}


void PitchDetector::getCurrentGuitarPosition(int& stringIndex, int& fretNumber) const
{
    stringIndex = currentGuitarString;
    fretNumber = currentGuitarFret;
}

void PitchDetector::clearInstrumentData()
{
    learnedProfiles.clear();
    learnedSpectraPerNote.clear();
}

void PitchDetector::setMaxPolyphony(int maxNotes)
{
    maxPolyphony = maxNotes;
}

int PitchDetector::getMaxPolyphony() const
{
    return maxPolyphony;
}

bool PitchDetector::isReadyForDetection() const
{
    return !learnedProfiles.empty();
}

void PitchDetector::setNoteDetectionCallback(std::function<void(const std::vector<int>&)> callback)
{
    noteCallback = callback;
}

std::string PitchDetector::midiNoteToName(int midiNote)
{
    static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    
    int octave = (midiNote / 12) - 1;
    int noteIndex = midiNote % 12;
    
    return std::string(noteNames[noteIndex]) + std::to_string(octave);
}