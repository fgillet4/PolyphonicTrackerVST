#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * FFTProcessor handles the FFT analysis of incoming audio.
 * It applies windowing, performs FFT, and calculates the magnitude spectrum.
 */
class FFTProcessor
{
public:
    /**
     * Constructor, initializes FFT with a specific size
     * @param fftSize The size of the FFT (must be a power of 2)
     */
    FFTProcessor(int fftSize = 4096);
    
    /**
     * Destructor
     */
    ~FFTProcessor();
    
    /**
     * Processes a block of audio and updates the FFT if enough samples are collected
     * @param inBuffer Audio input buffer
     * @param numSamples Number of samples in the buffer
     * @return True if FFT was performed, false otherwise
     */
    bool processBlock(const float* inBuffer, int numSamples);
    
    /**
     * Gets the current FFT magnitude spectrum
     * @return Pointer to the spectrum data
     */
    const float* getMagnitudeSpectrum() const;
    
    /**
     * Gets the size of the spectrum (fftSize / 2)
     * @return Size of the magnitude spectrum array
     */
    int getSpectrumSize() const;
    
    /**
     * Gets the current FFT size
     * @return Current FFT size
     */
    int getFFTSize() const;
    
    /**
     * Sets the overlap factor for the FFT processing
     * @param newOverlap Overlap factor (0.0 to 0.95)
     */
    void setOverlapFactor(float newOverlap);
    
    /**
     * Resets the FFT processor, clearing all buffers
     */
    void reset();
    
    /**
     * Registers a callback function to be called when new FFT data is available
     * @param callback Function to call with new spectrum data
     */
    void setSpectrumDataCallback(std::function<void(const float*, int)> callback);
    
private:
    int fftSize;
    int spectrumSize;
    float overlapFactor;
    
    std::vector<float> inputBuffer;
    int inputBufferPos;
    
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    
    std::vector<float> fftData;
    std::vector<float> magnitudeSpectrum;
    
    std::function<void(const float*, int)> spectrumCallback;
    
    void performFFT();
    void applyWindow();
};