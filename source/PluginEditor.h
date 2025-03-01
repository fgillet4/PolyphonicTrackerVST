#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "gui/SpectrogramComponent.h"
#include "gui/CustomPanel.h"


// Custom dark theme with high-contrast controls
class DarkPurpleTheme : public juce::LookAndFeel_V4
{
public:
    DarkPurpleTheme()
    {
        // Window and background colors
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xFF000000)); // Pure black
        setColour(juce::TabbedComponent::backgroundColourId, juce::Colour(0xFF0C0C0C)); // Dark gray
        setColour(juce::TabbedButtonBar::tabOutlineColourId, juce::Colour(0xFF333333)); // Light gray border
        setColour(juce::TabbedButtonBar::frontOutlineColourId, juce::Colour(0xFFAA33FF)); // Purple highlight
        
        // ComboBox colors
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF222222));
        setColour(juce::ComboBox::textColourId, juce::Colours::white);
        setColour(juce::ComboBox::arrowColourId, juce::Colour(0xFF9C33FF));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF9C33FF)); // Brighter outline
        setColour(juce::ComboBox::buttonColourId, juce::Colour(0xFF444444)); // Visible button
        
        // Slider colors - very high contrast
        setColour(juce::Slider::thumbColourId, juce::Colour(0xFFDD66FF)); // Brighter purple thumb
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFFDD66FF)); // For rotary sliders
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xFF444444)); // For rotary sliders
        setColour(juce::Slider::trackColourId, juce::Colour(0xFF444444)); // Lighter track
        setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF222222)); // Visible dark gray
        
        // TextBox colors
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF333333)); // Lighter for visibility
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF9C33FF)); // Purple outline
        setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0xFFDD66FF).withAlpha(0.5f));
        
        // Text editor colors
        setColour(juce::TextEditor::textColourId, juce::Colours::white);
        setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF333333)); // Lighter background
        setColour(juce::TextEditor::highlightColourId, juce::Colour(0xFFDD66FF).withAlpha(0.3f));
        setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFF9C33FF)); // Purple outline
        
        // Label colors
        setColour(juce::Label::textColourId, juce::Colours::white);
        setColour(juce::Label::outlineColourId, juce::Colours::transparentWhite);
        setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        
        // Button colors - much more visible
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF333333)); // Lighter button
        setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFFAA33FF)); // Bright when on
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        
        // Toggle button colors - high contrast
        setColour(juce::ToggleButton::tickColourId, juce::Colour(0xFFDD66FF)); // Bright purple tick
        setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xFF999999)); // Lighter when disabled
    }
    
    // Override to make sliders more visible
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        // Draw a more visible track
        g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        g.fillRect(x, y + height / 2 - 3, width, 6);
        
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        if (style == juce::Slider::LinearHorizontal)
        {
            g.fillRect(x, y + height / 2 - 3, (int)sliderPos - x, 6);
        }
        
        // Draw a larger thumb
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillEllipse(sliderPos - 8, y + height / 2 - 8, 16, 16);
        
        // Draw outline
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.drawEllipse(sliderPos - 8, y + height / 2 - 8, 16, 16, 1.0f);
    }
};

// Custom image component
class JayImageComponent : public juce::Component
{
public:
    JayImageComponent(const juce::Image& img) : image(img) {}
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.drawImage(image, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
    }
    
private:
    juce::Image image;
};

//==============================================================================
class PolyphonicTrackerAudioProcessorEditor : public juce::AudioProcessorEditor,
private juce::Timer
{
public:
    explicit PolyphonicTrackerAudioProcessorEditor(PolyphonicTrackerAudioProcessor&);
    ~PolyphonicTrackerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;


private:
    // UI mode update method
    void updateUIMode();
    
    // Reference to the processor
    PolyphonicTrackerAudioProcessor& audioProcessor;

    // Custom theme
    DarkPurpleTheme darkTheme;
    
    // Image of Jay
    juce::Image jayImage;
    std::unique_ptr<JayImageComponent> jayImageComponent;
    
    // UI Components
    juce::TabbedComponent tabbedComponent {juce::TabbedButtonBar::TabsAtTop};
    
    // Panel components
    CustomPanel* mainPanel = nullptr;
    CustomPanel* guitarPanel = nullptr;
    CustomPanel* visualPanel = nullptr;
    
    // Emergency components for direct testing
    std::unique_ptr<juce::TextButton> emergencyButton;
    std::unique_ptr<juce::Slider> emergencySlider;
    std::unique_ptr<juce::ComboBox> emergencyGuitarControls;
    std::unique_ptr<juce::Slider> emergencyFretSlider;
    std::unique_ptr<juce::TextButton> emergencyLearnButton;
    
    // Debug text editor
    juce::TextEditor m_debugTextEditor;
    
    // Learning mode controls
    juce::ToggleButton learningModeToggle {"Learning Mode"};
    juce::Slider currentNoteSlider;
    juce::Label currentNoteLabel {"", "Current Note:"};
    
    // Guitar mode controls
    juce::ComboBox guitarStringCombo;
    juce::Label guitarStringLabel {"", "String:"};
    juce::Slider guitarFretSlider;
    juce::Label guitarFretLabel {"", "Fret:"};
    juce::TextButton learnFretButton {"Learn This Position"};
    juce::Label learningStatusLabel {"", ""};
    
    // Spectrogram component
    std::unique_ptr<SpectrogramComponent> spectrogramComponent;

    // MIDI output controls
    juce::Slider maxPolyphonySlider;
    juce::Label maxPolyphonyLabel {"", "Max Polyphony:"};
    
    juce::Slider midiChannelSlider;
    juce::Label midiChannelLabel {"", "MIDI Channel:"};
    
    juce::Slider midiVelocitySlider;
    juce::Label midiVelocityLabel {"", "MIDI Velocity:"};
    
    juce::Slider noteOnDelaySlider;
    juce::Label noteOnDelayLabel {"", "Note On Delay (ms):"};
    
    juce::Slider noteOffDelaySlider;
    juce::Label noteOffDelayLabel {"", "Note Off Delay (ms):"};
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> learningModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> currentNoteAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> maxPolyphonyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midiChannelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midiVelocityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noteOnDelayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noteOffDelayAttachment;

    // Parameter attachments for guitar mode
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> guitarStringAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> guitarFretAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolyphonicTrackerAudioProcessorEditor)
};