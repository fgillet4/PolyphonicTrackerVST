#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Custom panel class with a dark background
class CustomPanel : public juce::Component
{
public:
    CustomPanel(const juce::String& panelTitle = "Panel") 
        : title(panelTitle)
    { 
        setOpaque(true);
        
        // Important to set a z-order that allows child components to be visible
        setInterceptsMouseClicks(false, true); // Don't intercept mouse clicks but children do
        
        juce::Logger::writeToLog("CustomPanel created: " + title);
    }
    
    void paint(juce::Graphics& g) override
    {
        // Use a dark but visible background with gradient
        juce::ColourGradient gradient(
            juce::Colour(0xFF252525), 0.0f, 0.0f,
            juce::Colour(0xFF1A1A1A), 0.0f, static_cast<float>(getHeight()),
            false);
        g.setGradientFill(gradient);
        g.fillAll();
        
        // Draw a grid pattern to show panel areas
        g.setColour(juce::Colour(0xFF333333));
        for (int x = 0; x < getWidth(); x += 50)
            g.drawVerticalLine(x, 0, getHeight());
        for (int y = 0; y < getHeight(); y += 50)
            g.drawHorizontalLine(y, 0, getWidth());
        
        // Add a more visible border
        g.setColour(juce::Colour(0xFF444444));
        g.drawRect(getLocalBounds(), 2);
        
        // Add title with purple highlight
        g.setColour(juce::Colour(0xFF9C33FF)); // Purple
        g.setFont(14.0f);
        g.drawText(title, 10, 10, getWidth() - 20, 24, juce::Justification::left, true);
        
        // Add control box outline to show where controls should be
        g.setColour(juce::Colour(0xFF9C33FF).withAlpha(0.3f));
        g.drawRect(20, 60, getWidth() - 40, getHeight() - 80, 1);
        
        // Add control info text
        g.setColour(juce::Colours::white);
        g.setFont(12.0f);
        g.drawText("Controls Active", 10, 34, 120, 20, juce::Justification::left, true);
        
        // Child count display
        int childCount = getNumChildComponents();
        g.drawText("Child Components: " + juce::String(childCount), 
                  getWidth() - 150, 10, 140, 20, juce::Justification::right, true);
        
        // Debug - show panel is working
        juce::Logger::writeToLog("CustomPanel painted: " + title + " " + getBounds().toString());
    }
    
private:
    juce::String title;
    
    // Override to log but not paint over children
    void paintOverChildren(juce::Graphics& /*g*/) override
    {
        // Just log the child count - don't do any painting here that could cover children
        auto childComponents = getNumChildComponents();
        juce::Logger::writeToLog("CustomPanel " + title + " has " + juce::String(childComponents) + " child components");
        
        // List all child components for debugging
        for (int i = 0; i < childComponents; ++i) {
            if (auto* child = getChildComponent(i)) {
                juce::Logger::writeToLog(" - Child " + juce::String(i) + ": " + 
                                       child->getComponentID() + 
                                       " visible: " + juce::String(child->isVisible() ? "YES" : "NO") +
                                       " bounds: " + child->getBounds().toString());
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomPanel)
};