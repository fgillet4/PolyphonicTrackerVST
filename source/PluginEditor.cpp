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
    
    // CRITICAL FIX: Use JUCE's direct implementation instead of CustomPanel's version
    mainPanel->Component::addAndMakeVisible(learningModeToggle);
    
    // Ensure it's visible and properly sized
    learningModeToggle.setVisible(true);
    
    // Set minimum size to ensure visibility
    learningModeToggle.setBounds(20, 60, 300, 30);
    
    // Log component visibility to debug
    juce::Logger::writeToLog("Adding learningModeToggle to mainPanel - visible: " + 
                            juce::String(learningModeToggle.isVisible() ? "YES" : "NO"));
    
    // Current note controls
    currentNoteLabel.setText("Current Note:", juce::dontSendNotification);
    currentNoteLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    currentNoteLabel.setFont(juce::Font(14.0f).boldened());
    currentNoteLabel.setJustificationType(juce::Justification::left);
    
    // CRITICAL FIX: Use JUCE's direct method
    mainPanel->Component::addAndMakeVisible(currentNoteLabel);
    
    // Direct add to editor
    addAndMakeVisible(currentNoteLabel);
    
    currentNoteLabel.setVisible(true);
    currentNoteLabel.setAlwaysOnTop(true);
    
    // Set minimum size to ensure visibility  
    currentNoteLabel.setBounds(20, 60, 150, 30);
    
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
    
    // CRITICAL FIX: Use JUCE's direct method
    mainPanel->Component::addAndMakeVisible(currentNoteSlider);
    
    // Direct add to editor
    addAndMakeVisible(currentNoteSlider);
    
    currentNoteSlider.setVisible(true);
    currentNoteSlider.setAlwaysOnTop(true);
    
    // Set an initial size to ensure visibility
    currentNoteSlider.setBounds(180, 60, 300, 30);
    
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
    
    // Create the spectrogram component for visualization tab
    spectrogramComponent = std::make_unique<SpectrogramComponent>();
    // We'll set its bounds in resized() method

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
    
    // Draw debug information
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawText("Polyphonic Tracker VST", 10, 5, getWidth() - 20, 20, juce::Justification::topLeft, true);
    
    // EXTREME EMERGENCY DEBUG - Directly draw component boundaries on main editor
    g.setColour(juce::Colours::red);
    g.fillRect(10, 40, 100, 30);
    g.setColour(juce::Colours::white);
    g.drawText("DEBUG MARKER", 15, 45, 90, 20, juce::Justification::centredLeft);
    
    // Draw component boundaries for debug
    if (mainPanel) {
        g.setColour(juce::Colours::green.withAlpha(0.5f));
        g.drawRect(mainPanel->getBounds(), 3);
    }
    
    if (guitarPanel) {
        g.setColour(juce::Colours::blue.withAlpha(0.5f));
        g.drawRect(guitarPanel->getBounds(), 3);
    }
    
    if (visualPanel) {
        g.setColour(juce::Colours::yellow.withAlpha(0.5f));
        g.drawRect(visualPanel->getBounds(), 3);
    }
    
    // Draw actual component bounds for debugging
    if (learningModeToggle.isVisible()) {
        auto bounds = learningModeToggle.getBounds();
        g.setColour(juce::Colours::red.withAlpha(0.5f));
        g.drawRect(bounds, 2);
        g.drawText("Toggle: " + bounds.toString(), bounds.getX(), bounds.getBottom() + 5, 
                   bounds.getWidth(), 20, juce::Justification::centredLeft);
    }
    
    // EMERGENCY DEBUG MODE - Draw all components directly in the main paint method
    // for visibility testing
    if (mainPanel && tabbedComponent.getCurrentTabIndex() == 0)
    {
        const int margin = 20;
        // Label width not needed in emergency debug drawing
        const int controlHeight = 30;
        int y = 80; // Start below the tab bar
        
        // Draw emergency button overlay
        g.setColour(juce::Colours::red.withAlpha(0.7f));
        g.fillRect(margin, y, 300, controlHeight);
        g.setColour(juce::Colours::white);
        g.drawText("Learning Mode Toggle (Emergency Draw)", margin + 5, y, 280, controlHeight, juce::Justification::centredLeft);
        y += controlHeight + margin;
        
        // Draw emergency slider overlay
        g.setColour(juce::Colours::blue.withAlpha(0.7f));
        g.fillRect(margin, y, 300, controlHeight);
        g.setColour(juce::Colours::white);
        g.drawText("Current Note Slider (Emergency Draw)", margin + 5, y, 280, controlHeight, juce::Justification::centredLeft);
        y += controlHeight + margin;
    }
    else if (guitarPanel && tabbedComponent.getCurrentTabIndex() == 1)
    {
        const int margin = 20;
        const int controlHeight = 30;
        int y = 80; // Start below the tab bar
        
        // Draw emergency guitar string combo overlay
        g.setColour(juce::Colours::green.withAlpha(0.7f));
        g.fillRect(margin, y, 300, controlHeight);
        g.setColour(juce::Colours::white);
        g.drawText("Guitar String Combo (Emergency Draw)", margin + 5, y, 280, controlHeight, juce::Justification::centredLeft);
        y += controlHeight + margin;
        
        // Draw emergency fret slider overlay
        g.setColour(juce::Colours::orange.withAlpha(0.7f));
        g.fillRect(margin, y, 300, controlHeight);
        g.setColour(juce::Colours::white);
        g.drawText("Guitar Fret Slider (Emergency Draw)", margin + 5, y, 280, controlHeight, juce::Justification::centredLeft);
        y += controlHeight + margin;
        
        // Draw emergency learn button overlay
        g.setColour(juce::Colours::magenta.withAlpha(0.7f));
        g.fillRect(margin, y, 300, 40);
        g.setColour(juce::Colours::white);
        g.drawText("Learn Button (Emergency Draw)", margin + 5, y, 280, 40, juce::Justification::centredLeft);
    }
    
    // Show controls visibility stats
    g.setColour(juce::Colours::yellow);
    g.setFont(13.0f);
    g.drawText("Main Panel: " + (mainPanel ? juce::String(mainPanel->getNumChildComponents()) : "null") + 
              " Guitar: " + (guitarPanel ? juce::String(guitarPanel->getNumChildComponents()) : "null"),
              10, getHeight() - 40, getWidth() - 20, 20, juce::Justification::bottomLeft);
    
    // Add debug log to check if paint is being called
    juce::Logger::writeToLog("Main editor paint called (tab: " + juce::String(tabbedComponent.getCurrentTabIndex()) + ")");
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
    
    // Periodically update debug info and force components to be visible
    if (counter % 10 == 0) {
        juce::Logger::writeToLog("Timer: Components visible check:");
        juce::Logger::writeToLog(" - Learning toggle visible: " + juce::String(learningModeToggle.isVisible() ? "Yes" : "No"));
        juce::Logger::writeToLog(" - Current tab: " + juce::String(tabbedComponent.getCurrentTabIndex()));
        
        // FORCE DIRECT PAINTING ON MAIN PANEL to try to get the components visible
        if (mainPanel) {
            juce::Logger::writeToLog("Forcing bounds check for main panel controls");
            
            // Add debug bounds information
            juce::Logger::writeToLog(" - learningModeToggle: " + learningModeToggle.getBounds().toString());
            juce::Logger::writeToLog(" - currentNoteSlider: " + currentNoteSlider.getBounds().toString());
            juce::Logger::writeToLog(" - mainPanel: " + mainPanel->getBounds().toString());
            
            // Attempt to force visibility of components by bringing to front
            learningModeToggle.toFront(false);
            currentNoteSlider.toFront(false);
            maxPolyphonySlider.toFront(false);
            midiChannelSlider.toFront(false);
            midiVelocitySlider.toFront(false);
            noteOnDelaySlider.toFront(false);
            noteOffDelaySlider.toFront(false);
        }
        
        // Force guitar panel controls to front
        if (guitarPanel) {
            juce::Logger::writeToLog("Forcing bounds check for guitar panel controls");
            juce::Logger::writeToLog(" - learnFretButton: " + learnFretButton.getBounds().toString());
            juce::Logger::writeToLog(" - guitarStringCombo: " + guitarStringCombo.getBounds().toString());
            juce::Logger::writeToLog(" - guitarPanel: " + guitarPanel->getBounds().toString());
            
            // Bring guitar controls to front
            guitarStringCombo.toFront(false);
            guitarFretSlider.toFront(false);
            learnFretButton.toFront(false);
            learningStatusLabel.toFront(false);
        }
        
        // Trigger a repaint throughout the GUI
        this->repaint(); 
    }
    
    // Force complete repaint every 2 seconds
    if (counter % 60 == 0) {
        // Special debug to show bounds of all controls in the log
        juce::Logger::writeToLog("*** CONTROL VISIBILITY REPORT ***");
        juce::Logger::writeToLog("Main toggle: " + learningModeToggle.getBounds().toString() + 
                               " visible: " + juce::String(learningModeToggle.isVisible() ? "YES" : "NO"));
        juce::Logger::writeToLog("Current note slider: " + currentNoteSlider.getBounds().toString() + 
                               " visible: " + juce::String(currentNoteSlider.isVisible() ? "YES" : "NO"));
        juce::Logger::writeToLog("Button: " + learnFretButton.getBounds().toString() + 
                               " visible: " + juce::String(learnFretButton.isVisible() ? "YES" : "NO"));
                               
        // Emergency hack - recreate buttons
        if (mainPanel) {
            // The extreme fallback approach - recreate the button and add it directly
            juce::Logger::writeToLog("EMERGENCY: Creating direct button on main panel");
            mainPanel->removeChildComponent(&learningModeToggle);
            
            // Create a direct button bypass
            emergencyButton = std::make_unique<juce::TextButton>("Emergency Button");
            emergencyButton->setButtonText("Learning Mode (Direct)");
            emergencyButton->setColour(juce::TextButton::buttonColourId, juce::Colours::red);
            emergencyButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
            mainPanel->addAndMakeVisible(*emergencyButton);
            emergencyButton->setBounds(20, 80, 300, 30);
            emergencyButton->setVisible(true);
            emergencyButton->toFront(false);
        }
        
        // Force repaint everything
        tabbedComponent.repaint();
        if (mainPanel) mainPanel->repaint();
        if (guitarPanel) guitarPanel->repaint();
        if (visualPanel) visualPanel->repaint();
        this->repaint();
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
    
    // Common constants for consistent layout - use relative sizing!
    const int margin = 20;
    const int smallMargin = 15;
    const int topOffset = 40; // Space below tab bar
    
    // Use proportional sizing for better layout
    const int labelWidth = juce::jmin(150, getWidth() / 5);
    const int controlHeight = juce::jmin(32, getHeight() / 15); // Based on screen height
    const int buttonHeight = juce::jmin(36, getHeight() / 14);  // Slightly taller
    
    // Calculate effective component width based on actual screen size
    const float widthRatio = 0.85f; // Use 85% of available width
    const int effectiveSliderWidth = static_cast<int>((getWidth() * widthRatio) - labelWidth - margin);
    
    // EMERGENCY DIRECT COMPONENT PLACEMENT
    // Only visible when debugging, provide a safety net for interactions
    int directY = topOffset;
    
    if (emergencyButton == nullptr) {
        // Set up emergency button with improved styling
        emergencyButton = std::make_unique<juce::TextButton>("EMERGENCY");
        emergencyButton->setButtonText("LEARN MODE");
        emergencyButton->setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF552266)); // Dark purple
        emergencyButton->setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF9C33FF)); // Bright purple when on
        emergencyButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        emergencyButton->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        emergencyButton->setLookAndFeel(&darkTheme);
        addAndMakeVisible(*emergencyButton);
        
        // Create an emergency slider with consistent styling
        emergencySlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight);
        emergencySlider->setRange(21, 108, 1);
        emergencySlider->setValue(60);
        emergencySlider->setColour(juce::Slider::thumbColourId, juce::Colour(0xFF9C33FF)); // Purple thumb
        emergencySlider->setColour(juce::Slider::trackColourId, juce::Colour(0xFF333333)); // Dark track
        emergencySlider->setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF191919));
        emergencySlider->setLookAndFeel(&darkTheme);
        addAndMakeVisible(*emergencySlider);
        
        // Set up emergency button callback
        emergencyButton->onClick = [this]() {
            bool newState = !audioProcessor.isLearningModeActive();
            audioProcessor.setLearningModeActive(newState);
            emergencyButton->setButtonText(newState ? "LEARNING: ON" : "LEARNING: OFF");
            repaint();
        };
        
        // Set up emergency slider callback
        emergencySlider->onValueChange = [this]() {
            int newNote = static_cast<int>(emergencySlider->getValue());
            audioProcessor.setCurrentLearningNote(newNote);
            juce::String noteName = juce::MidiMessage::getMidiNoteName(newNote, true, true, 3);
            juce::Logger::writeToLog("Emergency slider changed: " + juce::String(newNote) + " (" + noteName + ")");
        };
    }
    
    // Place emergency controls directly on the editor - use proportional width
    const int emergencyControlWidth = static_cast<int>(getWidth() * 0.4f); // 40% of window width
    emergencyButton->setBounds(margin, directY, emergencyControlWidth, buttonHeight);
    directY += buttonHeight + smallMargin;
    emergencySlider->setBounds(margin, directY, emergencyControlWidth, controlHeight);
    directY += controlHeight + margin;
    
    // Add direct guitar controls with improved styling
    if (emergencyGuitarControls == nullptr) {
        // Create a direct guitar string combo box
        emergencyGuitarControls = std::make_unique<juce::ComboBox>("Guitar String");
        emergencyGuitarControls->addItemList({"E (low)", "A", "D", "G", "B", "E (high)"}, 1);
        emergencyGuitarControls->setSelectedItemIndex(0);
        emergencyGuitarControls->setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF222222));
        emergencyGuitarControls->setColour(juce::ComboBox::textColourId, juce::Colours::white);
        emergencyGuitarControls->setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF9C33FF));
        emergencyGuitarControls->setLookAndFeel(&darkTheme);
        addAndMakeVisible(*emergencyGuitarControls);
        
        // Create a direct learn button with improved styling
        emergencyLearnButton = std::make_unique<juce::TextButton>("Guitar Learn");
        emergencyLearnButton->setButtonText("LEARN GUITAR POSITION");
        emergencyLearnButton->setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF552266));
        emergencyLearnButton->setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF9C33FF));
        emergencyLearnButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        emergencyLearnButton->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        emergencyLearnButton->setLookAndFeel(&darkTheme);
        addAndMakeVisible(*emergencyLearnButton);
        
        // Create the fret slider with consistent styling
        emergencyFretSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight);
        emergencyFretSlider->setRange(0, 24, 1);
        emergencyFretSlider->setValue(0);
        emergencyFretSlider->setColour(juce::Slider::thumbColourId, juce::Colour(0xFF9C33FF));
        emergencyFretSlider->setColour(juce::Slider::trackColourId, juce::Colour(0xFF333333));
        emergencyFretSlider->setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF191919));
        emergencyFretSlider->setLookAndFeel(&darkTheme);
        addAndMakeVisible(*emergencyFretSlider);
        
        // Set up learn button callback
        emergencyLearnButton->onClick = [this]() {
            int string = emergencyGuitarControls->getSelectedItemIndex();
            int fret = static_cast<int>(emergencyFretSlider->getValue());
            int midiNote = audioProcessor.setCurrentGuitarPosition(string, fret);
            audioProcessor.setLearningModeActive(true);
            audioProcessor.setCurrentLearningNote(midiNote);
            
            emergencyLearnButton->setButtonText("Learning: String " + juce::String(string + 1) + 
                                            " Fret " + juce::String(fret) + 
                                            " (MIDI: " + juce::String(midiNote) + ")");
        };
    }
    
    // Position direct guitar controls with proportional sizes
    const int comboWidth = std::min(200, static_cast<int>(getWidth() * 0.3f)); // Max 30% of window width
    emergencyGuitarControls->setBounds(margin, directY, comboWidth, controlHeight);
    directY += controlHeight + smallMargin;
    emergencyFretSlider->setBounds(margin, directY, emergencyControlWidth, controlHeight);
    directY += controlHeight + smallMargin;
    emergencyLearnButton->setBounds(margin, directY, emergencyControlWidth, buttonHeight);
    directY += buttonHeight + margin;
    
    // Place a debug text area at the bottom of the screen
    const int debugAreaHeight = 120;
    directY = getHeight() - debugAreaHeight - margin;
    m_debugTextEditor.setBounds(margin, directY, getWidth() - margin*2, debugAreaHeight);
    addAndMakeVisible(m_debugTextEditor);
    
    // ==========================================
    // MAIN PANEL LAYOUT - Controls Tab
    // ==========================================
    if (mainPanel != nullptr)
    {
        juce::Logger::writeToLog("Positioning controls in main panel: " + mainPanel->getBounds().toString());
        
        // Get the full bounds of the panel
        auto fullBounds = mainPanel->getLocalBounds();
        juce::Logger::writeToLog("Main panel full bounds: " + fullBounds.toString());
        
        // Calculate usable content area with margins
        auto contentBounds = fullBounds.reduced(margin);
        
        // Leave space at top for the title
        contentBounds.removeFromTop(topOffset);
        
        // Use proportional control spacing, evenly distributed through available height
        const int availableHeight = contentBounds.getHeight();
        const int controlCount = 7; // Toggle + 6 sliders
        const int controlSpacing = (availableHeight - (controlCount * controlHeight)) / (controlCount + 2);
        const int effectiveWidth = contentBounds.getWidth();
        const int sliderWidth = effectiveWidth - labelWidth - margin;
        
        // CRITICAL FIX - DON'T clear panel and start fresh, as this breaks event handlers
        // mainPanel->removeAllChildren();

        // IMPORTANT: Hide other tab's components when not in this tab
        // This ensures tab-specific components don't appear in other tabs
        // Hide guitar panel components in main panel
        guitarStringLabel.setVisible(false);
        guitarStringCombo.setVisible(false);
        guitarFretLabel.setVisible(false);
        guitarFretSlider.setVisible(false);
        learnFretButton.setVisible(false);
        learningStatusLabel.setVisible(false);
        
        // Hide direct visualization if any
        if (spectrogramComponent)
            spectrogramComponent->setVisible(false);
        if (jayImageComponent)
            jayImageComponent->setVisible(false);
        
        // EXPLICIT VERTICAL DISTRIBUTION:
        // Create a consistent vertical position tracker that increases with each component
        int yPos = contentBounds.getY();
        
        // Learning mode toggle - positioned at the top with good width
        const int toggleWidth = std::min(300, static_cast<int>(effectiveWidth * 0.5f));
        learningModeToggle.setBounds(contentBounds.getX(), yPos, toggleWidth, buttonHeight);
        learningModeToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
        learningModeToggle.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xFF9C33FF)); // Purple
        learningModeToggle.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xFF666666));
        
        // CRITICAL FIX: Make this component visible only in this panel
        learningModeToggle.setVisible(true);
        
        // Force component to front for proper stacking
        learningModeToggle.toFront(false);
        
        // CRITICAL: Increment vertical position for next component
        yPos += buttonHeight + controlSpacing;
        
        // Set up control bounds for the first slider - ensure good vertical spacing
        auto controlBounds = contentBounds.withTrimmedTop(buttonHeight + controlSpacing);
        
        // Log the actual spacing we're using for debugging
        juce::Logger::writeToLog("Main panel control spacing: " + juce::String(controlSpacing) + 
                              ", Available height: " + juce::String(availableHeight));
        
        // Layout helper function for consistent control positioning using explicit yPos
        auto positionControl = [&](juce::Label& label, juce::Slider& slider) {
            // CRITICAL DEBUG: Log current bounds before changing
            juce::Logger::writeToLog("Before positioning: Label bounds: " + label.getBounds().toString() + 
                               ", Slider bounds: " + slider.getBounds().toString());
        
            // Position label with UNIQUE vertical position using yPos - CRUCIAL FIX
            label.setBounds(contentBounds.getX(), yPos, 
                          labelWidth, controlHeight);
            
            // Use boldened() method for text styling
            label.setFont(juce::Font(14.0f).boldened());
            label.setColour(juce::Label::textColourId, juce::Colours::white);
            
            // CRITICAL FIX: Don't call addAndMakeVisible repeatedly, just ensure visibility
            if (!label.isVisible())
                label.setVisible(true);
            
            // Position slider with UNIQUE vertical position using yPos - CRUCIAL FIX
            slider.setBounds(contentBounds.getX() + labelWidth, yPos, 
                           sliderWidth, controlHeight);
            slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xFF9C33FF));
            slider.setColour(juce::Slider::trackColourId, juce::Colour(0xFF333333));
            slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF191919));
            slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
            slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF444444));
            slider.setLookAndFeel(&darkTheme);
            
            // CRITICAL FIX: Don't call addAndMakeVisible repeatedly, just ensure visibility
            if (!slider.isVisible())
                slider.setVisible(true);
            
            // FORCE COMPONENTS TO FRONT for visibility
            label.toFront(false);
            slider.toFront(false);
            
            // Log the bounds for debugging
            juce::Logger::writeToLog("Control: " + label.getText() + " - Label: " + label.getBounds().toString() + 
                                  ", Slider: " + slider.getBounds().toString());
            
            // CRITICAL: Increment yPos for next component to avoid all controls having same position
            yPos += controlHeight + controlSpacing;
        };
        
        // Position all controls with consistent styling and vertical distribution
        // Each control automatically gets properly incremented Y position
        positionControl(currentNoteLabel, currentNoteSlider);
        positionControl(maxPolyphonyLabel, maxPolyphonySlider);
        positionControl(midiChannelLabel, midiChannelSlider);
        positionControl(midiVelocityLabel, midiVelocitySlider);
        positionControl(noteOnDelayLabel, noteOnDelaySlider);
        positionControl(noteOffDelayLabel, noteOffDelaySlider);
        
        // Debug text editor at the bottom - use remaining space
        // Calculate how much height is left after controls
        const int totalControlsHeight = controlCount * controlHeight + (controlCount - 1) * controlSpacing;
        const int remainingHeight = contentBounds.getHeight() - totalControlsHeight - buttonHeight - controlSpacing;
        const int debugPanelHeight = juce::jmax(100, remainingHeight - (margin * 2));
        
        // Position at the bottom
        const int debugY = contentBounds.getBottom() - debugPanelHeight - margin;
        m_debugTextEditor.setBounds(contentBounds.getX(), debugY, effectiveWidth, debugPanelHeight);
        m_debugTextEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF111111));
        m_debugTextEditor.setColour(juce::TextEditor::textColourId, juce::Colours::white);
        mainPanel->addAndMakeVisible(m_debugTextEditor);
        
        juce::Logger::writeToLog("Debug editor positioned at: " + m_debugTextEditor.getBounds().toString() +
                              ", remainingHeight: " + juce::String(remainingHeight));
        
        // Force repaint for visibility
        mainPanel->repaint();
    }
    else
    {
        juce::Logger::writeToLog("ERROR: Main panel is null in resized()");
    }

    // ==========================================
    // GUITAR PANEL LAYOUT - Guitar Mode Tab
    // ==========================================
    if (guitarPanel != nullptr)
    {
        juce::Logger::writeToLog("Positioning controls in guitar panel");
        
        // Get the full bounds of the panel
        auto fullBounds = guitarPanel->getLocalBounds();
        juce::Logger::writeToLog("Guitar panel full bounds: " + fullBounds.toString());
        
        // Calculate usable content area with margins
        auto contentBounds = fullBounds.reduced(margin);
        
        // Leave space at top for the title
        contentBounds.removeFromTop(topOffset);
        
        // Calculate proportional spacing and sizing
        const int availableHeight = contentBounds.getHeight();
        const int controlCount = 4; // string combo, fret slider, learn button, status label
        const int controlSpacing = availableHeight / 12; // Distribute controls evenly
        const int effectiveWidth = contentBounds.getWidth();
        const int sliderWidth = effectiveWidth - labelWidth - margin;
        const int comboWidth = std::min(300, static_cast<int>(effectiveWidth * 0.5f));
        
        // Log available space for debugging
        juce::Logger::writeToLog("Guitar panel available height: " + juce::String(availableHeight) + 
                              ", controlSpacing: " + juce::String(controlSpacing));
        
        // CRITICAL FIX - DO NOT clear existing children
        // guitarPanel->removeAllChildren();
        
        // IMPORTANT: Hide other tab's components when not in this tab
        // Hide main panel controls in guitar panel
        learningModeToggle.setVisible(false);
        currentNoteLabel.setVisible(false);
        currentNoteSlider.setVisible(false);
        maxPolyphonyLabel.setVisible(false);
        maxPolyphonySlider.setVisible(false);
        midiChannelLabel.setVisible(false);
        midiChannelSlider.setVisible(false);
        midiVelocityLabel.setVisible(false);
        midiVelocitySlider.setVisible(false);
        noteOnDelayLabel.setVisible(false);
        noteOnDelaySlider.setVisible(false);
        noteOffDelayLabel.setVisible(false);
        noteOffDelaySlider.setVisible(false);
        
        // Hide visualization components
        if (spectrogramComponent)
            spectrogramComponent->setVisible(false);
        if (jayImageComponent)
            jayImageComponent->setVisible(false);
        
        // EXPLICIT VERTICAL DISTRIBUTION:
        // Create a consistent vertical position tracker that increases with each component
        int yPos = contentBounds.getY() + static_cast<int>(availableHeight * 0.1f); // Start at 10% down
        
        // Guitar string controls in a row
        guitarStringLabel.setBounds(contentBounds.getX(), yPos, labelWidth, controlHeight);
        guitarStringLabel.setFont(juce::Font(14.0f).boldened());
        guitarStringLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        
        // CRITICAL FIX: Don't call addAndMakeVisible repeatedly, just ensure visibility
        if (!guitarStringLabel.isVisible())
            guitarStringLabel.setVisible(true);
        
        // Force component to front
        guitarStringLabel.toFront(false);
        
        // EMERGENCY DEBUG - log bounds
        juce::Logger::writeToLog("Guitar string label bounds: " + guitarStringLabel.getBounds().toString());
        
        // Set up the combo box - position on same row as label using same Y
        guitarStringCombo.setBounds(contentBounds.getX() + labelWidth, yPos, comboWidth, controlHeight);
        guitarStringCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF222222));
        guitarStringCombo.setColour(juce::ComboBox::textColourId, juce::Colours::white);
        guitarStringCombo.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF9C33FF));
        guitarStringCombo.setLookAndFeel(&darkTheme);
        
        // CRITICAL: Increment Y position for next row
        yPos += controlHeight + controlSpacing;
        
        // CRITICAL FIX: Don't call addAndMakeVisible repeatedly, just ensure visibility
        if (!guitarStringCombo.isVisible())
            guitarStringCombo.setVisible(true);
        
        // Force component to front
        guitarStringCombo.toFront(false);
        
        // EMERGENCY DEBUG - log bounds
        juce::Logger::writeToLog("Guitar string combo bounds: " + guitarStringCombo.getBounds().toString());
        
        juce::Logger::writeToLog("Guitar string combo bounds: " + guitarStringCombo.getBounds().toString());
        
        // Guitar fret controls in the next row - use current yPos
        guitarFretLabel.setBounds(contentBounds.getX(), yPos, labelWidth, controlHeight);
        guitarFretLabel.setFont(juce::Font(14.0f).boldened());
        guitarFretLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        
        // CRITICAL FIX: Don't call addAndMakeVisible repeatedly, just ensure visibility
        if (!guitarFretLabel.isVisible())
            guitarFretLabel.setVisible(true);
            
        // Force component to front
        guitarFretLabel.toFront(false);
        
        // EMERGENCY DEBUG - log bounds
        juce::Logger::writeToLog("Guitar fret label bounds: " + guitarFretLabel.getBounds().toString());
        
        guitarFretSlider.setBounds(contentBounds.getX() + labelWidth, yPos, sliderWidth, controlHeight);
        guitarFretSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xFF9C33FF));
        guitarFretSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xFF333333));
        guitarFretSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xFF191919));
        guitarFretSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        guitarFretSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF444444));
        guitarFretSlider.setLookAndFeel(&darkTheme);
        
        // CRITICAL: Increment Y position for next row
        yPos += controlHeight + controlSpacing * 2; // Extra spacing before button
        
        // CRITICAL FIX: Don't call addAndMakeVisible repeatedly, just ensure visibility
        if (!guitarFretSlider.isVisible())
            guitarFretSlider.setVisible(true);
            
        // Force component to front
        guitarFretSlider.toFront(false);
        
        // EMERGENCY DEBUG - log bounds
        juce::Logger::writeToLog("Guitar fret slider bounds: " + guitarFretSlider.getBounds().toString());
        
        juce::Logger::writeToLog("Guitar fret slider bounds: " + guitarFretSlider.getBounds().toString());
        
        // Learn button centered with proper sizing - make it large and prominent
        // Place the button using calculated yPos for better vertical distribution
        const int learnButtonWidth = std::min(350, static_cast<int>(effectiveWidth * 0.7f)); // Use up to 70% of width
        const int learnButtonHeight = buttonHeight + 10; // Make it taller for emphasis
        const int learnButtonX = contentBounds.getX() + (effectiveWidth - learnButtonWidth) / 2; // Center horizontally
        
        learnFretButton.setBounds(learnButtonX, yPos, learnButtonWidth, learnButtonHeight);
        learnFretButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF552266));
        learnFretButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF9C33FF));
        learnFretButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        learnFretButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        learnFretButton.setLookAndFeel(&darkTheme);
        
        // CRITICAL FIX: Don't call addAndMakeVisible repeatedly, just ensure visibility
        if (!learnFretButton.isVisible())
            learnFretButton.setVisible(true);
            
        // Force component to front
        learnFretButton.toFront(false);
        
        juce::Logger::writeToLog("Learn button bounds: " + learnFretButton.getBounds().toString());
        
        // CRITICAL: Increment Y position for next control
        yPos += learnButtonHeight + controlSpacing;
        
        // Status label below button - make it large and clear
        learningStatusLabel.setBounds(contentBounds.getX(), yPos, effectiveWidth, 
                                    juce::jmin(50, static_cast<int>(availableHeight * 0.15f))); // Up to 15% of available height
        learningStatusLabel.setJustificationType(juce::Justification::centred);
        learningStatusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        learningStatusLabel.setFont(juce::Font(16.0f).boldened());
        
        // CRITICAL FIX: Don't call addAndMakeVisible repeatedly, just ensure visibility
        if (!learningStatusLabel.isVisible())
            learningStatusLabel.setVisible(true);
            
        // Force component to front
        learningStatusLabel.toFront(false);
        
        juce::Logger::writeToLog("Status label bounds: " + learningStatusLabel.getBounds().toString());
        
        // Calculate spacing to verify layout
        const int usedHeight = (controlHeight * 2) + learnButtonHeight + 50 + (controlSpacing * 3);
        const int percentUsed = static_cast<int>((usedHeight * 100.0f) / availableHeight);
        juce::Logger::writeToLog("Guitar panel layout - Used height: " + juce::String(usedHeight) + 
                              " (" + juce::String(percentUsed) + "% of available)");
        
        juce::Logger::writeToLog("Status label bounds: " + learningStatusLabel.getBounds().toString());
        
        // Force repaint for visibility
        guitarPanel->repaint();
    }
    else
    {
        juce::Logger::writeToLog("ERROR: Guitar panel is null in resized()");
    }

    // ==========================================
    // VISUALIZATION PANEL LAYOUT
    // ==========================================
    if (visualPanel != nullptr)
    {
        juce::Logger::writeToLog("Positioning controls in visualization panel");
        
        // Get the full bounds of the panel
        auto fullBounds = visualPanel->getLocalBounds();
        juce::Logger::writeToLog("Visualization panel full bounds: " + fullBounds.toString());
        
        // Calculate usable content area with margins - use smaller margins for visualization
        auto contentBounds = fullBounds.reduced(margin);
        const int availableHeight = contentBounds.getHeight();
        const int availableWidth = contentBounds.getWidth();
        
        // Leave space at top for the title
        contentBounds.removeFromTop(topOffset);
        
        // CRITICAL FIX - DO NOT clear existing children
        // visualPanel->removeAllChildren();
        
        // IMPORTANT: Hide other tab's components when not in this tab
        // Hide main panel controls in visualization panel
        learningModeToggle.setVisible(false);
        currentNoteLabel.setVisible(false);
        currentNoteSlider.setVisible(false);
        maxPolyphonyLabel.setVisible(false);
        maxPolyphonySlider.setVisible(false);
        midiChannelLabel.setVisible(false);
        midiChannelSlider.setVisible(false);
        midiVelocityLabel.setVisible(false);
        midiVelocitySlider.setVisible(false);
        noteOnDelayLabel.setVisible(false);
        noteOnDelaySlider.setVisible(false);
        noteOffDelayLabel.setVisible(false);
        noteOffDelaySlider.setVisible(false);
        
        // Hide guitar panel components
        guitarStringLabel.setVisible(false);
        guitarStringCombo.setVisible(false);
        guitarFretLabel.setVisible(false);
        guitarFretSlider.setVisible(false);
        learnFretButton.setVisible(false);
        learningStatusLabel.setVisible(false);
        
        // For the visualization panel, use a balanced split between image and spectrum
        // The visualization should use most of the space since it's more important
        
        // Use 25% for image, 75% for spectrogram - gives better proportions
        const float jayImageHeightRatio = 0.25f; 
        auto imageHeight = static_cast<int>(contentBounds.getHeight() * jayImageHeightRatio);
        
        // Reserve the top portion for the image with a gap below
        auto imageBounds = contentBounds.removeFromTop(imageHeight);
        contentBounds.removeFromTop(10); // Add a small gap between components
        
        // The remaining ~75% of space is for the spectrogram - critical component
        auto spectrogramBounds = contentBounds;
        
        // CRITICAL FIX - Ensure spectrogramComponent exists
        if (spectrogramComponent == nullptr) {
            spectrogramComponent = std::make_unique<SpectrogramComponent>();
            juce::Logger::writeToLog("Created missing spectrogramComponent in resized()");
        }
        
        // Log bounds for debugging
        juce::Logger::writeToLog("Visualization layout - availableHeight: " + juce::String(availableHeight) + 
                              ", imageHeight: " + juce::String(imageHeight) + 
                              ", spectrogramHeight: " + juce::String(spectrogramBounds.getHeight()));
        
        juce::Logger::writeToLog("Image bounds: " + imageBounds.toString());
        juce::Logger::writeToLog("Spectrogram bounds: " + spectrogramBounds.toString());
        
        // Jay image on top portion
        if (jayImageComponent)
        {
            // Use the full width and calculated height
            jayImageComponent->setBounds(imageBounds);
            
            // CRITICAL FIX: Check if already visible before adding
            if (!jayImageComponent->isVisible()) {
                visualPanel->Component::addAndMakeVisible(*jayImageComponent);
                jayImageComponent->setVisible(true);
            }
            
            // Force component to front
            jayImageComponent->toFront(false);
            
            juce::Logger::writeToLog("Set Jay image bounds: " + jayImageComponent->getBounds().toString());
        }

        // Spectrogram on bottom portion - this should be the MAIN element
        if (spectrogramComponent)
        {
            // EXTREMELY CRITICAL: Use absolute coordinates, not relative to parent
            // Make sure to use FULL PANEL HEIGHT (minus small margins)
            // This ensures the visualization takes up most of the available space
            auto absoluteBounds = juce::Rectangle<int>(
                contentBounds.getX(), 
                contentBounds.getY() + 40,  // Leave space at top for title
                contentBounds.getWidth(),  
                contentBounds.getHeight() - 60  // Leave small margin at bottom
            );
            juce::Logger::writeToLog("Setting spectrogram to absolute bounds: " + absoluteBounds.toString());
            
            // This is crucial - use non-zero bounds bigger than 100x100 for visibility
            if (absoluteBounds.getWidth() < 100 || absoluteBounds.getHeight() < 100) {
                absoluteBounds = juce::Rectangle<int>(20, 100, visualPanel->getWidth() - 40, visualPanel->getHeight() - 150);
                juce::Logger::writeToLog("Spectrogram bounds too small - using minimum size");
            }
            
            // Set the bounds BEFORE making visible - force large size
            spectrogramComponent->setBounds(absoluteBounds);
            
            // Configure spectrogram for better visibility
            spectrogramComponent->setColours(juce::Colours::black, juce::Colour(0xFF9C33FF));
            
            // CRITICAL FIX: Check if already visible before adding
            if (!spectrogramComponent->isVisible()) {
                visualPanel->Component::addAndMakeVisible(*spectrogramComponent);
                spectrogramComponent->setVisible(true);
            }
            
            // Force the component to be on top and visible
            spectrogramComponent->setAlwaysOnTop(true);
            spectrogramComponent->toFront(false);
            
            // Also add directly to editor for extreme debug
            addAndMakeVisible(*spectrogramComponent);
            
            juce::Logger::writeToLog("Spectrogram actual bounds: " + spectrogramComponent->getBounds().toString() +
                                  ", visible: " + juce::String(spectrogramComponent->isVisible() ? "YES" : "NO"));
            
            // Force spectrogram to repaint immediately
            spectrogramComponent->repaint();
        }
        
        // Force repaint for visibility
        visualPanel->repaint();
    }
    else
    {
        juce::Logger::writeToLog("ERROR: Visual panel is null in resized()");
    }
    
    // Force all components to repaint
    repaint();
}