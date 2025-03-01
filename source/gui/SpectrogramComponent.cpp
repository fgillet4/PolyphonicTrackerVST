#include "SpectrogramComponent.h"

SpectrogramComponent::SpectrogramComponent()
    : threshold(0.001f), // Lower threshold to catch smaller amplitudes
      useLogFrequency(true),
      showThreshold(true),
      sampleRate(44100.0f),
      backgroundColour(juce::Colour(0xFF0A0A0A)),
      foregroundColour(juce::Colour(0xFF9C33FF))
{
    // Initialize FFT data
    fftData.resize(kMaxFFTSize, 0.0f);
    peakData.resize(kMaxFFTSize, 0.0f);
    
    // Initialize frequency scale
    for (size_t i = 0; i < frequencyScale.size(); ++i)
    {
        float proportion = static_cast<float>(i) / static_cast<float>(kMaxFFTSize - 1);
        frequencyScale[i] = (useLogFrequency)
            ? kMinFrequency * std::pow(kMaxFrequency / kMinFrequency, proportion)
            : kMinFrequency + (kMaxFrequency - kMinFrequency) * proportion;
    }
    
    // Start the timer for updates
    startTimer(kRefreshRateMs);
    DBG("SpectrogramComponent initialized");
}

SpectrogramComponent::~SpectrogramComponent()
{
    stopTimer();
}

void SpectrogramComponent::paint(juce::Graphics& g)
{
    // Background using custom color
    g.fillAll(backgroundColour);
    
    // Get component dimensions
    int width = getWidth();
    int height = getHeight();
    DBG("SpectrogramComponent paint: width=" << width << ", height=" << height); // Debug bounds
    
    if (width <= 0 || height <= 0)
    {
        g.setColour(juce::Colours::red);
        g.drawText("Invalid Size", getLocalBounds(), juce::Justification::centred);
        return;
    }

    // Draw header text with better styling
    g.setColour(juce::Colour(0xFF9C33FF)); // Purple to match theme
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText("Frequency Spectrum", getLocalBounds().withTrimmedBottom(20), juce::Justification::centredTop);
    
    // Draw grid lines and frequency labels with better visibility
    g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
    // Octave grid lines (if using log scale)
    if (useLogFrequency)
    {
        for (int freq = static_cast<int>(kMinFrequency); freq <= kMaxFrequency; freq *= 2)
        {
            float x = frequencyToX(static_cast<float>(freq));
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
            g.setColour(juce::Colours::lightgrey);
            juce::String label = (freq >= 1000) ? juce::String(freq / 1000) + "kHz" : juce::String(freq) + "Hz";
            g.setFont(juce::Font(12.0f, juce::Font::bold));
            g.drawText(label, static_cast<int>(x) - 20, height - 20, 40, 20, juce::Justification::centred, false);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        }
    }
    else
    {
        for (int freq = 0; freq <= kMaxFrequency; freq += 1000)
        {
            float x = frequencyToX(static_cast<float>(freq));
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
            g.setColour(juce::Colours::lightgrey);
            juce::String label = (freq >= 1000) ? juce::String(freq / 1000) + "kHz" : juce::String(freq) + "Hz";
            g.setFont(juce::Font(12.0f, juce::Font::bold));
            g.drawText(label, static_cast<int>(x) - 20, height - 20, 40, 20, juce::Justification::centred, false);
            g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
        }
    }
    
    // Amplitude grid lines with improved visibility
    for (float amp = 0.0f; amp <= 1.0f; amp += 0.2f)
    {
        float y = amplitudeToY(amp);
        g.drawHorizontalLine(static_cast<int>(y), 0.0f, static_cast<float>(width));
        g.setColour(juce::Colours::lightgrey);
        juce::String label = juce::String(static_cast<int>(amp * 100)) + "%";
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        g.drawText(label, 5, static_cast<int>(y) - 10, 40, 20, juce::Justification::left, false);
        g.setColour(juce::Colours::darkgrey.withAlpha(0.6f));
    }
    
    // Draw threshold line if enabled, using theme color
    if (showThreshold)
    {
        g.setColour(foregroundColour.withAlpha(0.7f));
        float thresholdY = amplitudeToY(threshold);
        g.drawHorizontalLine(static_cast<int>(thresholdY), 0.0f, static_cast<float>(width));
        
        // Label the threshold line
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        g.drawText("Threshold", 5, static_cast<int>(thresholdY) - 15, 80, 15, juce::Justification::left, false);
    }
    // Draw current spectrum (force visibility for debug)
    bool drewSomething = false;
    for (int i = 1; i < static_cast<int>(fftData.size()); ++i)
    {
        if (i >= static_cast<int>(frequencyScale.size()))
            break;
        float prevFreq = frequencyScale[static_cast<size_t>(i - 1)];
        float freq = frequencyScale[static_cast<size_t>(i)];
        float prevAmp = fftData[static_cast<size_t>(i - 1)];
        float amp = fftData[static_cast<size_t>(i)];
        if (amp > 0.0f) // Draw even small amplitudes
        {
            drewSomething = true;
            float x1 = frequencyToX(prevFreq);
            float y1 = amplitudeToY(prevAmp);
            float x2 = frequencyToX(freq);
            float y2 = amplitudeToY(amp);
            juce::Colour color = (amp > threshold)
                ? juce::Colour::fromHSV(0.7f - (amp - threshold) * 0.3f, 0.8f, 1.0f, 1.0f)
                : juce::Colour(0xFF1A1A4A).withAlpha(0.5f);
            g.setColour(color);
            g.drawLine(x1, y1, x2, y2, 2.0f);
        }
    }
    if (!drewSomething)
    {
        g.setColour(foregroundColour); // Use theme color
        g.setFont(juce::Font(18.0f, juce::Font::bold));
        g.drawText("Waiting for Audio Input...", getLocalBounds(), juce::Justification::centred);
        
        // Add a helpful hint
        g.setFont(juce::Font(14.0f));
        g.drawText("Play your instrument to see the frequency spectrum", 
                  getLocalBounds().withTrimmedTop(getHeight()/2 + 20), 
                  juce::Justification::centredTop);
    }
    // Draw waterfall history
    {
        int historySize = static_cast<int>(history.size());
        float historyAlphaStep = 0.8f / static_cast<float>(historySize);
        
        for (int h = 0; h < historySize; ++h)
        {
            float alpha = 0.8f - h * historyAlphaStep;
            const auto& historyData = history[static_cast<size_t>(h)];
            
            // Draw spectrum as a series of vertical lines
            for (int i = 1; i < static_cast<int>(historyData.size()); ++i)
            {
                if (i >= static_cast<int>(frequencyScale.size()))
                    break;
                
                float prevFreq = frequencyScale[static_cast<size_t>(i - 1)];
                float freq = frequencyScale[static_cast<size_t>(i)];
                
                float prevAmp = historyData[static_cast<size_t>(i - 1)];
                float amp = historyData[static_cast<size_t>(i)];
                
                float x1 = frequencyToX(prevFreq);
                float y1 = amplitudeToY(prevAmp);
                float x2 = frequencyToX(freq);
                float y2 = amplitudeToY(amp);
                
                // Color based on amplitude
                juce::Colour color;
                if (amp > threshold)
                {
                    // Above threshold - gradient from blue to purple
                    color = juce::Colour::fromHSV(0.7f - (amp - threshold) * 0.3f, 0.8f, 1.0f, alpha);
                }
                else
                {
                    // Below threshold - dark blue
                    color = juce::Colour(0xFF1A1A4A).withAlpha(alpha * 0.5f);
                }
                
                g.setColour(color);
                g.drawLine(x1, y1, x2, y2, 1.0f);
            }
        }
    }
    
    // Draw current spectrum
    {
        // Draw spectrum as a series of vertical lines
        for (int i = 1; i < static_cast<int>(fftData.size()); ++i)
        {
            if (i >= static_cast<int>(frequencyScale.size()))
                break;
                
            float prevFreq = frequencyScale[static_cast<size_t>(i - 1)];
            float freq = frequencyScale[static_cast<size_t>(i)];
            
            float prevAmp = fftData[static_cast<size_t>(i - 1)];
            float amp = fftData[static_cast<size_t>(i)];
            
            float x1 = frequencyToX(prevFreq);
            float y1 = amplitudeToY(prevAmp);
            float x2 = frequencyToX(freq);
            float y2 = amplitudeToY(amp);
            
            // Color based on amplitude
            juce::Colour color;
            if (amp > threshold)
            {
                // Above threshold - gradient from blue to purple
                color = juce::Colour::fromHSV(0.7f - (amp - threshold) * 0.3f, 0.8f, 1.0f, 1.0f);
            }
            else
            {
                // Below threshold - dark blue
                color = juce::Colour(0xFF1A1A4A).withAlpha(0.5f);
            }
            
            g.setColour(color);
            g.drawLine(x1, y1, x2, y2, 2.0f);
        }
    }
    
    // Draw marked frequencies
    for (const auto& markedFreq : markedFrequencies)
    {
        float x = frequencyToX(markedFreq.frequency);
        
        if (markedFreq.active)
        {
            // Active frequency - draw as a vertical green line
            g.setColour(juce::Colour(0xFF33FF99));
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
            
            // Draw a label
            g.drawText(juce::String(markedFreq.frequency, 1) + "Hz",
                       static_cast<int>(x) - 30, 5, 60, 20,
                       juce::Justification::centred, false);
        }
        else
        {
            // Inactive frequency - draw as a vertical grey line
            g.setColour(juce::Colours::grey);
            g.drawVerticalLine(static_cast<int>(x), 0.0f, static_cast<float>(height));
        }
    }
}

void SpectrogramComponent::resized()
{
    // Nothing to do here
}

void SpectrogramComponent::updateFFT(const float* newFFTData, int size)
{
    // This method should now be called from the message thread,
    // so no need for thread synchronization
    
    if (newFFTData == nullptr || size <= 0) {
        juce::Logger::writeToLog("updateFFT received invalid data");
        return;
    }
    
    // Safety check - this should be on the message thread
    jassert(juce::MessageManager::getInstance()->isThisTheMessageThread());
    
    // Safely limit size
    int safeSize = juce::jmin(size, kMaxFFTSize);
    
    // Make sure our vectors are properly sized
    if (fftData.size() < static_cast<size_t>(safeSize)) {
        fftData.resize(static_cast<size_t>(safeSize), 0.0f);
    }
    
    if (peakData.size() < static_cast<size_t>(safeSize)) {
        peakData.resize(static_cast<size_t>(safeSize), 0.0f);
    }
    
    // Copy the new data with bounds checking
    for (int i = 0; i < safeSize; ++i)
    {
        if (i < static_cast<int>(fftData.size())) {
            fftData[static_cast<size_t>(i)] = newFFTData[i];
            
            // Update peak data with decay, also with bounds checking
            if (i < static_cast<int>(peakData.size())) {
                peakData[static_cast<size_t>(i)] = std::max(peakData[static_cast<size_t>(i)] * 0.95f, newFFTData[i]);
            }
        }
    }
    
    // Add current data to history
    if (!fftData.empty()) {
        history.push_front(fftData);
        
        // Limit history size
        while (history.size() > kHistorySize)
        {
            history.pop_back();
        }
    }
    
    // Since we're already on the message thread, repaint directly
    repaint();
}

void SpectrogramComponent::setThreshold(float newThreshold)
{
    threshold = juce::jlimit(0.0f, 1.0f, newThreshold);
    repaint();
}

void SpectrogramComponent::setUseLogFrequency(bool shouldUseLogScale)
{
    if (useLogFrequency != shouldUseLogScale)
    {
        useLogFrequency = shouldUseLogScale;
        
        // Update frequency scale
        for (int i = 0; i < kMaxFFTSize; ++i)
        {
            float proportion = static_cast<float>(i) / static_cast<float>(kMaxFFTSize - 1);
            frequencyScale[static_cast<size_t>(i)] = (useLogFrequency)
                ? kMinFrequency * std::pow(kMaxFrequency / kMinFrequency, proportion)
                : kMinFrequency + (kMaxFrequency - kMinFrequency) * proportion;
        }
        
        repaint();
    }
}

void SpectrogramComponent::setShowThreshold(bool shouldShow)
{
    showThreshold = shouldShow;
    repaint();
}

void SpectrogramComponent::markFrequency(float freqHz, bool isActive)
{
    // Check if this frequency is already marked
    for (auto& markedFreq : markedFrequencies)
    {
        if (std::abs(markedFreq.frequency - freqHz) < 0.1f)
        {
            markedFreq.active = isActive;
            repaint();
            return;
        }
    }
    
    // Add new marked frequency
    markedFrequencies.push_back({freqHz, isActive});
    repaint();
}

void SpectrogramComponent::clearMarkedFrequencies()
{
    markedFrequencies.clear();
    repaint();
}

void SpectrogramComponent::setColours(juce::Colour background, juce::Colour foreground)
{
    backgroundColour = background;
    foregroundColour = foreground;
    repaint();
}

void SpectrogramComponent::timerCallback()
{
    // Trigger a repaint to update any animations
    repaint();
}

float SpectrogramComponent::frequencyToX(float frequency) const
{
    float normX;
    
    if (useLogFrequency)
    {
        // Logarithmic scale
        normX = std::log(frequency / kMinFrequency) / std::log(kMaxFrequency / kMinFrequency);
    }
    else
    {
        // Linear scale
        normX = (frequency - kMinFrequency) / (kMaxFrequency - kMinFrequency);
    }
    
    return normX * static_cast<float>(getWidth());
}

float SpectrogramComponent::amplitudeToY(float amplitude) const
{
    // Invert Y coordinate (0 amplitude at bottom)
    float normY = 1.0f - amplitude / kMaxAmplitude;
    return normY * static_cast<float>(getHeight());
}