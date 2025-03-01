#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "gui/CustomPanel.h"

PolyphonicTrackerAudioProcessorEditor::PolyphonicTrackerAudioProcessorEditor(PolyphonicTrackerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set up logger for debugging
    juce::Logger::setCurrentLogger(new juce::FileLogger(
        juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
            .getChildFile("PolyphonicTracker_Debug.log"),
        "Polyphonic Tracker Debug Log", 0));
    
    juce::Logger::writeToLog("Editor constructor started");
    
    // Set size and look and feel - make sure it's applied globally
    setSize(800, 600);
    juce::LookAndFeel::setDefaultLookAndFeel(&darkTheme);
    setLookAndFeel(&darkTheme);
    
    // Load Jay image if available
    jayImage = juce::ImageCache::getFromFile(juce::File("resources/images/jay.jpg"));
    if (jayImage.isNull())
        juce::Logger::writeToLog("Failed to load jay.jpg");
    else
        juce::Logger::writeToLog("Successfully loaded jay.jpg");

    // Create image component
    if (!jayImage.isNull())
        jayImageComponent = std::make_unique<JayImageComponent>(jayImage);
    else
        jayImageComponent = std::make_unique<JayImageComponent>(juce::Image(juce::Image::RGB, 200, 200, true));

    // Set up tabbed component
    tabbedComponent.setOutline(0);
    tabbedComponent.setTabBarDepth(35);
    tabbedComponent.setIndent(8); // Add some indent for better visibility
    tabbedComponent.setColour(juce::TabbedComponent::backgroundColourId, juce::Colour(0xFF0A0A0A));
    tabbedComponent.setColour(juce::TabbedComponent::outlineColourId, juce::Colour(0xFF9C33FF));
    
    // Make sure the tabbed component is visible
    addAndMakeVisible(tabbedComponent);

    // Create panels for tabs with ownership and custom titles
    mainPanel = new CustomPanel("Main Control Panel");
    guitarPanel = new CustomPanel("Guitar Mode Panel");
    visualPanel = new CustomPanel("Visualization Panel");
    
    // Add tabs with panels but don't take ownership (we'll do it ourselves)
    tabbedComponent.addTab("Controls", juce::Colour(0xFF111111), mainPanel, true);
    tabbedComponent.addTab("Guitar Mode", juce::Colour(0xFF111111), guitarPanel, true);
    tabbedComponent.addTab("Visualization", juce::Colour(0xFF111111), visualPanel, true);
    
    // Make sure tabs are visible
    tabbedComponent.setCurrentTabIndex(0);
    juce::Logger::writeToLog("Added tabs to tabbed component");

    // Set up Main Panel controls
    learningModeToggle.setButtonText("Learning Mode");
    learningModeToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    learningModeToggle.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xFF9C33FF)); // Purple
    learningModeToggle.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xFF666666));
    mainPanel->addAndMakeVisible(learningModeToggle);
    
    // Ensure it's visible and properly sized
    learningModeToggle.setVisible(true);
    learningModeToggle.setAlwaysOnTop(true);
    
    // Log component visibility to debug
    juce::Logger::writeToLog("Adding learningModeToggle to mainPanel - visible: " + 
                            juce::String(learningModeToggle.isVisible() ? "YES" : "NO"));
    
    // Current note controls
    currentNoteLabel.setText("Current Note:", juce::dontSendNotification);
    currentNoteLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    currentNoteLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    currentNoteLabel.setJustificationType(juce::Justification::left);
    mainPanel->addAndMakeVisible(currentNoteLabel);
    currentNoteLabel.setVisible(true);
    currentNoteLabel.setAlwaysOnTop(true);
    
    // Configure slider with obvious appearance
    currentNoteSlider.setRange(21, 108, 1);
    currentNoteSlider.setValue(60);
    currentNoteSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    currentNoteSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    
    // Make the thumb and track MUCH bigger and more colorful
    currentNoteSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xFF9C33FF)); // Purple thumb
    currentNoteSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xFF333333)); // Dark track
    currentNoteSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF191919)); // Very dark track background
    
    // Text box colors
    currentNoteSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    currentNoteSlider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xFF222222));
    currentNoteSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF444444));
    currentNoteSlider.setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0xFF9C33FF).withAlpha(0.3f));
    
    // Ensure it's added to the panel and visible
    mainPanel->addAndMakeVisible(currentNoteSlider);
    currentNoteSlider.setVisible(true);
    currentNoteSlider.setAlwaysOnTop(true);
    
    // Manually set slider height to make it more visible
    currentNoteSlider.setLookAndFeel(&darkTheme); // Use our custom look and feel
    
    juce::Logger::writeToLog("Adding currentNoteSlider to mainPanel - visible: " + 
                           juce::String(currentNoteSlider.isVisible() ? "YES" : "NO"));

    // Max polyphony controls
    maxPolyphonyLabel.setText("Max Polyphony:", juce::dontSendNotification);
    maxPolyphonyLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mainPanel->addAndMakeVisible(maxPolyphonyLabel);
    
    maxPolyphonySlider.setRange(1, 8, 1);
    maxPolyphonySlider.setValue(6);
    maxPolyphonySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    maxPolyphonySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(maxPolyphonySlider);

    // MIDI channel controls
    midiChannelLabel.setText("MIDI Channel:", juce::dontSendNotification);
    midiChannelLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mainPanel->addAndMakeVisible(midiChannelLabel);
    
    midiChannelSlider.setRange(1, 16, 1);
    midiChannelSlider.setValue(1);
    midiChannelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    midiChannelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(midiChannelSlider);

    // MIDI velocity controls
    midiVelocityLabel.setText("MIDI Velocity:", juce::dontSendNotification);
    midiVelocityLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mainPanel->addAndMakeVisible(midiVelocityLabel);
    
    midiVelocitySlider.setRange(0, 127, 1);
    midiVelocitySlider.setValue(100);
    midiVelocitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    midiVelocitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(midiVelocitySlider);

    // Note on delay controls
    noteOnDelayLabel.setText("Note On Delay (ms):", juce::dontSendNotification);
    noteOnDelayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mainPanel->addAndMakeVisible(noteOnDelayLabel);
    
    noteOnDelaySlider.setRange(0, 500, 1);
    noteOnDelaySlider.setValue(50);
    noteOnDelaySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noteOnDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(noteOnDelaySlider);

    // Note off delay controls
    noteOffDelayLabel.setText("Note Off Delay (ms):", juce::dontSendNotification);
    noteOffDelayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    mainPanel->addAndMakeVisible(noteOffDelayLabel);
    
    noteOffDelaySlider.setRange(0, 500, 1);
    noteOffDelaySlider.setValue(100);
    noteOffDelaySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    noteOffDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    mainPanel->addAndMakeVisible(noteOffDelaySlider);

    // Set up Guitar Panel controls
    guitarStringLabel.setText("String:", juce::dontSendNotification);
    guitarStringLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    guitarPanel->addAndMakeVisible(guitarStringLabel);
    
    guitarStringCombo.addItemList({"E (low)", "A", "D", "G", "B", "E (high)"}, 1);
    guitarStringCombo.setSelectedItemIndex(0);
    guitarStringCombo.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    guitarStringCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF222222));
    guitarStringCombo.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xFF9C33FF));
    guitarPanel->addAndMakeVisible(guitarStringCombo);

    guitarFretLabel.setText("Fret:", juce::dontSendNotification);
    guitarFretLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    guitarPanel->addAndMakeVisible(guitarFretLabel);
    
    guitarFretSlider.setRange(0, 24, 1);
    guitarFretSlider.setValue(0);
    guitarFretSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    guitarFretSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    guitarPanel->addAndMakeVisible(guitarFretSlider);

    // Make the Learn button extra visible
    learnFretButton.setButtonText("Learn This Position");
    learnFretButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF552266)); // Darker purple
    learnFretButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFFDD66FF)); // Bright purple when on
    learnFretButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    learnFretButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    guitarPanel->addAndMakeVisible(learnFretButton);
    learnFretButton.setVisible(true);
    learnFretButton.setAlwaysOnTop(true);
    
    // Apply our custom look and feel to the button
    learnFretButton.setLookAndFeel(&darkTheme);

    learningStatusLabel.setText("Ready", juce::dontSendNotification);
    learningStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    guitarPanel->addAndMakeVisible(learningStatusLabel);

    // Set up Visualization Panel
    visualPanel->addAndMakeVisible(*jayImageComponent);

    spectrogramComponent = std::make_unique<SpectrogramComponent>();
    visualPanel->addAndMakeVisible(*spectrogramComponent);

    // Set up debug text editor
    m_debugTextEditor.setMultiLine(true);
    m_debugTextEditor.setReadOnly(true);
    m_debugTextEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF111111));
    m_debugTextEditor.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    mainPanel->addAndMakeVisible(m_debugTextEditor);

    // Set up FFT data callback with thread safety - use a weak reference to prevent crashes
    std::weak_ptr<SpectrogramComponent> weakSpectrogram = std::shared_ptr<SpectrogramComponent>(spectrogramComponent.get(), 
        [](SpectrogramComponent*){});  // Non-owning shared_ptr
        
    audioProcessor.setFFTDataCallback([weakSpectrogram](const float* data, int size) {
        if (data == nullptr || size <= 0) return;
        
        // Make a safe copy of data before accessing any objects
        std::vector<float> dataCopy(data, data + size);
        
        // Post to the message thread for safety - NEVER lock across threads
        juce::MessageManager::callAsync([weakSpectrogram, dataCopy, size]() {
            if (auto spectrogram = weakSpectrogram.lock()) {
                spectrogram->updateFFT(dataCopy.data(), size);
            }
        });
    });

    // Set up button callbacks
    learnFretButton.onClick = [this]() {
        int string = guitarStringCombo.getSelectedItemIndex();
        int fret = static_cast<int>(guitarFretSlider.getValue());
        int midiNote = audioProcessor.setCurrentGuitarPosition(string, fret);
        audioProcessor.setLearningModeActive(true);
        audioProcessor.setCurrentLearningNote(midiNote);
        learningStatusLabel.setText("Learning: String " + juce::String(string + 1) + ", Fret " + juce::String(fret) +
                                   " (MIDI: " + juce::String(midiNote) + ")",
                                   juce::dontSendNotification);
        
        // Force repaint when button is clicked
        if (mainPanel) mainPanel->repaint();
        if (guitarPanel) guitarPanel->repaint();
        if (visualPanel) visualPanel->repaint();
        
        // Also repaint child components for debugging
        juce::Logger::writeToLog("Button clicked - forcing repaint of all components");
    };

    learningModeToggle.onClick = [this]() {
        bool isLearningMode = learningModeToggle.getToggleState();
        bool isGuitarMode = tabbedComponent.getCurrentTabIndex() == 1;

        if (isLearningMode && isGuitarMode) {
            if (spectrogramComponent)
                spectrogramComponent->clearMarkedFrequencies();

            int string = guitarStringCombo.getSelectedItemIndex();
            int fret = static_cast<int>(guitarFretSlider.getValue());
            learningStatusLabel.setText("Learning Guitar: String " + juce::String(string + 1) + ", Fret " + juce::String(fret),
                                       juce::dontSendNotification);
        } else if (isLearningMode) {
            learningStatusLabel.setText("Learning Mode: Play single notes", juce::dontSendNotification);
        } else {
            learningStatusLabel.setText("Detection Mode: Play normally", juce::dontSendNotification);
        }
    };

    // Set up parameter attachments
    auto& params = audioProcessor.getParameterTree();

    learningModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        params, "learningMode", learningModeToggle);

    currentNoteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "currentNote", currentNoteSlider);

    maxPolyphonyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "maxPolyphony", maxPolyphonySlider);

    midiChannelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "midiChannel", midiChannelSlider);

    midiVelocityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "midiVelocity", midiVelocitySlider);

    noteOnDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "noteOnDelay", noteOnDelaySlider);

    noteOffDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "noteOffDelay", noteOffDelaySlider);

    guitarStringAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        params, "guitarString", guitarStringCombo);

    guitarFretAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        params, "guitarFret", guitarFretSlider);

    // Start timer for updating debug display
    startTimer(500);
    
    juce::Logger::writeToLog("Editor constructor completed");
}

PolyphonicTrackerAudioProcessorEditor::~PolyphonicTrackerAudioProcessorEditor()
{
    // Stop timer before any other cleanup
    stopTimer();
    
    // Clean up look and feel
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    setLookAndFeel(nullptr);
    
    // Clean up component references before destruction
    // The tabbed component owns these and will delete them
    mainPanel = nullptr;
    guitarPanel = nullptr;
    visualPanel = nullptr;
    
    // Log cleanup and remove logger
    juce::Logger::writeToLog("Editor destructor called");
    juce::Logger::setCurrentLogger(nullptr);
}

void PolyphonicTrackerAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Fill with dark background
    g.fillAll(juce::Colour(0xFF000000));
    
    // Draw some debug text to verify the paint method is working
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("Polyphonic Tracker VST", getLocalBounds(), juce::Justification::topLeft, true);
    
    // Add debug log to check if paint is being called
    juce::Logger::writeToLog("Main editor paint called");
}

void PolyphonicTrackerAudioProcessorEditor::timerCallback()
{
    static int counter = 0;
    counter++;
    
    // Update debug text
    if (juce::FileLogger* logger = dynamic_cast<juce::FileLogger*>(juce::Logger::getCurrentLogger()))
    {
        juce::String logContent = logger->getLogFile().loadFileAsString();
        // Show the last 20 lines
        auto lines = juce::StringArray::fromLines(logContent);
        juce::String lastLines;
        int numLines = juce::jmin(20, lines.size());
        for (int i = lines.size() - numLines; i < lines.size(); ++i)
        {
            if (i >= 0 && i < lines.size())
                lastLines += lines[i] + "\n";
        }
        m_debugTextEditor.setText(lastLines);
    }
    
    // Periodically log component visibility
    if (counter % 10 == 0) {
        juce::Logger::writeToLog("Timer: Components visible check:");
        juce::Logger::writeToLog(" - Learning toggle visible: " + juce::String(learningModeToggle.isVisible() ? "Yes" : "No"));
        juce::Logger::writeToLog(" - Current tab: " + juce::String(tabbedComponent.getCurrentTabIndex()));
    }
    
    // Force repaint of components to ensure they're visible
    for (int i = 0; i < tabbedComponent.getNumTabs(); ++i)
    {
        if (auto* comp = tabbedComponent.getTabContentComponent(i))
        {
            comp->repaint();
        }
    }
}

void PolyphonicTrackerAudioProcessorEditor::resized()
{
    // Set bounds for the tabbed component to fill the entire editor
    auto area = getLocalBounds();
    tabbedComponent.setBounds(area);

    juce::Logger::writeToLog("Editor resized: " + area.toString());
    
    // Main panel layout - use our stored pointer instead of casting
    if (mainPanel != nullptr)
    {
        juce::Logger::writeToLog("Positioning controls in main panel: " + mainPanel->getBounds().toString());
        const int margin = 20;
        const int labelWidth = 150;
        const int controlHeight = 30;
        int y = margin;

        // Learning mode toggle
        learningModeToggle.setBounds(margin, y, mainPanel->getWidth() - margin * 2, controlHeight);
        juce::Logger::writeToLog("- learningModeToggle: " + learningModeToggle.getBounds().toString());
        y += controlHeight + margin;

        // Current note slider
        currentNoteLabel.setBounds(margin, y, labelWidth, controlHeight);
        currentNoteSlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        juce::Logger::writeToLog("- currentNoteSlider: " + currentNoteSlider.getBounds().toString());
        y += controlHeight + margin;

        // Max polyphony slider
        maxPolyphonyLabel.setBounds(margin, y, labelWidth, controlHeight);
        maxPolyphonySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        juce::Logger::writeToLog("- maxPolyphonySlider: " + maxPolyphonySlider.getBounds().toString());
        y += controlHeight + margin;

        // MIDI channel slider
        midiChannelLabel.setBounds(margin, y, labelWidth, controlHeight);
        midiChannelSlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        juce::Logger::writeToLog("- midiChannelSlider: " + midiChannelSlider.getBounds().toString());
        y += controlHeight + margin;

        // MIDI velocity slider
        midiVelocityLabel.setBounds(margin, y, labelWidth, controlHeight);
        midiVelocitySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        juce::Logger::writeToLog("- midiVelocitySlider: " + midiVelocitySlider.getBounds().toString());
        y += controlHeight + margin;

        // Note on delay slider
        noteOnDelayLabel.setBounds(margin, y, labelWidth, controlHeight);
        noteOnDelaySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        juce::Logger::writeToLog("- noteOnDelaySlider: " + noteOnDelaySlider.getBounds().toString());
        y += controlHeight + margin;

        // Note off delay slider
        noteOffDelayLabel.setBounds(margin, y, labelWidth, controlHeight);
        noteOffDelaySlider.setBounds(margin + labelWidth, y, mainPanel->getWidth() - margin * 2 - labelWidth, controlHeight);
        juce::Logger::writeToLog("- noteOffDelaySlider: " + noteOffDelaySlider.getBounds().toString());
        y += controlHeight + margin;
        
        // Debug text editor at the bottom
        m_debugTextEditor.setBounds(margin, mainPanel->getHeight() - 150 - margin, 
                                 mainPanel->getWidth() - margin*2, 150);
        juce::Logger::writeToLog("- debugTextEditor: " + m_debugTextEditor.getBounds().toString());
        
        // Force repaint
        mainPanel->repaint();
    }
    else
    {
        juce::Logger::writeToLog("ERROR: Main panel is null in resized()");
    }

    // Guitar panel layout - use our stored pointer instead of casting
    if (guitarPanel != nullptr)
    {
        juce::Logger::writeToLog("Positioning controls in guitar panel");
        const int margin = 20;
        const int labelWidth = 150;
        const int controlHeight = 30;
        int y = margin * 2;

        // Guitar string controls
        guitarStringLabel.setBounds(margin, y, labelWidth, controlHeight);
        guitarStringCombo.setBounds(margin + labelWidth, y, 200, controlHeight);
        juce::Logger::writeToLog("- guitarStringCombo: " + guitarStringCombo.getBounds().toString());
        y += controlHeight + margin;

        // Guitar fret controls
        guitarFretLabel.setBounds(margin, y, labelWidth, controlHeight);
        guitarFretSlider.setBounds(margin + labelWidth, y, guitarPanel->getWidth() - margin * 3 - labelWidth, controlHeight);
        juce::Logger::writeToLog("- guitarFretSlider: " + guitarFretSlider.getBounds().toString());
        y += controlHeight + margin * 2;

        // Learn button
        learnFretButton.setBounds(margin, y, 200, 40);
        juce::Logger::writeToLog("- learnFretButton: " + learnFretButton.getBounds().toString());
        y += 50;

        // Status label
        learningStatusLabel.setBounds(margin, y, guitarPanel->getWidth() - margin * 2, 40);
        juce::Logger::writeToLog("- learningStatusLabel: " + learningStatusLabel.getBounds().toString());
        
        // Force repaint
        guitarPanel->repaint();
    }
    else
    {
        juce::Logger::writeToLog("ERROR: Guitar panel is null in resized()");
    }

    // Visualization panel layout - use our stored pointer instead of casting
    if (visualPanel != nullptr)
    {
        juce::Logger::writeToLog("Positioning controls in visualization panel");
        auto bounds = visualPanel->getLocalBounds().reduced(20);
        auto topSection = bounds.removeFromTop(bounds.getHeight() / 2);

        // Jay image
        if (jayImageComponent)
        {
            jayImageComponent->setBounds(topSection);
            juce::Logger::writeToLog("- jayImageComponent: " + jayImageComponent->getBounds().toString());
        }

        // Spectrogram
        if (spectrogramComponent)
        {
            spectrogramComponent->setBounds(bounds);
            juce::Logger::writeToLog("- spectrogramComponent: " + spectrogramComponent->getBounds().toString());
        }
        
        // Force repaint
        visualPanel->repaint();
    }
    else
    {
        juce::Logger::writeToLog("ERROR: Visual panel is null in resized()");
    }
    
    // Force all components to repaint
    learningModeToggle.repaint();
    currentNoteSlider.repaint();
    maxPolyphonySlider.repaint();
    midiChannelSlider.repaint();
    midiVelocitySlider.repaint();
    noteOnDelaySlider.repaint();
    noteOffDelaySlider.repaint();
    m_debugTextEditor.repaint();
    
    guitarStringCombo.repaint();
    guitarFretSlider.repaint();
    learnFretButton.repaint();
    learningStatusLabel.repaint();
    
    if (jayImageComponent) jayImageComponent->repaint();
    if (spectrogramComponent) spectrogramComponent->repaint();
}