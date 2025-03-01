#include "FFTProcessor.h"

FFTProcessor::FFTProcessor(int fftSizeParam)
    : fftSize(fftSizeParam),
      spectrumSize(fftSizeParam / 2),
      overlapFactor(0.5f),
      inputBufferPos(0),
      fft(static_cast<int>(std::log2(static_cast<double>(fftSizeParam)))),
      window(static_cast<size_t>(fftSizeParam), juce::dsp::WindowingFunction<float>::hann)
{
    // Allocate memory for buffers
    inputBuffer.resize(static_cast<size_t>(fftSize), 0.0f);
    fftData.resize(static_cast<size_t>(fftSize * 2), 0.0f); // Complex data (real/imag pairs)
    magnitudeSpectrum.resize(static_cast<size_t>(spectrumSize), 0.0f);
}

FFTProcessor::~FFTProcessor()
{
}

bool FFTProcessor::processBlock(const float* inBuffer, int numSamples)
{
    bool fftPerformed = false;
    
    // Calculate hop size based on overlap factor
    int hopSize = static_cast<int>(fftSize * (1.0f - overlapFactor));
    if (hopSize < 1) hopSize = 1;
    
    // Add samples to the input buffer
    for (int i = 0; i < numSamples; ++i)
    {
        inputBuffer[static_cast<size_t>(inputBufferPos)] = inBuffer[i];
        inputBufferPos++;
        
        // If we have enough samples, perform the FFT
        if (inputBufferPos >= fftSize)
        {
            performFFT();
            
            // Shift the buffer by the hop size
            const int samplesToKeep = fftSize - hopSize;
            if (samplesToKeep > 0)
            {
                std::copy(inputBuffer.begin() + hopSize, inputBuffer.end(), inputBuffer.begin());
            }
            
            inputBufferPos = samplesToKeep;
            fftPerformed = true;
        }
    }
    
    return fftPerformed;
}

// In FFTProcessor.cpp
void FFTProcessor::performFFT()
{
    // Apply window function
    applyWindow();
    
    // Copy windowed input to FFT data array (real part)
    for (int i = 0; i < fftSize; ++i)
    {
        fftData[static_cast<size_t>(i * 2)] = inputBuffer[static_cast<size_t>(i)];
        fftData[static_cast<size_t>(i * 2 + 1)] = 0.0f; // Imaginary part is zero
    }
    
    // Perform the FFT
    fft.performRealOnlyForwardTransform(fftData.data(), true);
    
    // Calculate the magnitude spectrum
    for (int i = 0; i < spectrumSize; ++i)
    {
        float real = fftData[static_cast<size_t>(i * 2)];
        float imag = fftData[static_cast<size_t>(i * 2 + 1)];
        
        // Calculate magnitude (sqrt of real^2 + imag^2)
        magnitudeSpectrum[static_cast<size_t>(i)] = std::sqrt(real * real + imag * imag);
    }
    
    // Call the callback if registered - in a try/catch block
    if (spectrumCallback)
    {
        try {
            std::function<void(const float*, int)> callbackCopy = spectrumCallback;
            if (callbackCopy) {
                callbackCopy(magnitudeSpectrum.data(), spectrumSize);
            }
        }
        catch (const std::exception& e) {
            juce::Logger::writeToLog("Error in FFT callback: " + juce::String(e.what()));
        }
    }
}

void FFTProcessor::applyWindow()
{
    window.multiplyWithWindowingTable(inputBuffer.data(), static_cast<size_t>(fftSize));
}

const float* FFTProcessor::getMagnitudeSpectrum() const
{
    return magnitudeSpectrum.data();
}

int FFTProcessor::getSpectrumSize() const
{
    return spectrumSize;
}

int FFTProcessor::getFFTSize() const
{
    return fftSize;
}

void FFTProcessor::setOverlapFactor(float newOverlap)
{
    overlapFactor = juce::jlimit(0.0f, 0.95f, newOverlap);
}

void FFTProcessor::reset()
{
    std::fill(inputBuffer.begin(), inputBuffer.end(), 0.0f);
    std::fill(fftData.begin(), fftData.end(), 0.0f);
    std::fill(magnitudeSpectrum.begin(), magnitudeSpectrum.end(), 0.0f);
    inputBufferPos = 0;
}

void FFTProcessor::setSpectrumDataCallback(std::function<void(const float*, int)> callback)
{
    spectrumCallback = callback;
}