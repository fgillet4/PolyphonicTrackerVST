#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Custom panel class with a dark background
class CustomPanel : public juce::Component
{
public:
    CustomPanel(const juce::String& panelTitle = "Panel") 
        : title(panelTitle)
    { 
        setOpaque(false); // Make non-opaque to allow components to show through
        
        // Important to set a z-order that allows child components to be visible
        setInterceptsMouseClicks(false, true); // Don't intercept mouse clicks but children do
        
        juce::Logger::writeToLog("CustomPanel created: " + title);
    }
    
    // Custom implementation to make children really work
    void addAndMakeVisible(juce::Component& childComponent)
    {
        // Use the standard JUCE implementation 
        Component::addAndMakeVisible(childComponent);
        
        // Extra steps to ensure visibility
        childComponent.setVisible(true);
        
        // CRITICAL FIX: Set minimum bounds if component has zero-sized bounds
        // This ensures components have at least some size and position
        if (childComponent.getWidth() == 0 || childComponent.getHeight() == 0) {
            // CRITICAL FIX: Calculate a unique Y position based on child count to avoid overlap
            int x = childComponent.getX() == 0 ? 20 : childComponent.getX();
            
            // CRITICAL FIX: Use component count to stagger Y positions
            // This ensures components don't stack on top of each other
            int childCount = getNumChildComponents();
            int y = childComponent.getY() == 0 ? (20 + (childCount * 40)) : childComponent.getY();
            
            // Use a minimum width/height for visibility
            int width = childComponent.getWidth() == 0 ? 200 : childComponent.getWidth();
            int height = childComponent.getHeight() == 0 ? 30 : childComponent.getHeight();
            
            childComponent.setBounds(x, y, width, height);
            juce::Logger::writeToLog("CustomPanel: Fixed zero-sized bounds for component in " + title + 
                                 " - new bounds: " + childComponent.getBounds().toString() +
                                 " (child " + juce::String(childCount) + ")");
        }
        
        // Ensure component is on top
        childComponent.toFront(false);
        
        // Log for debugging
        juce::Logger::writeToLog("CustomPanel: Added child to " + title + 
                             " - visible: " + juce::String(childComponent.isVisible() ? "YES" : "NO") +
                             " bounds: " + childComponent.getBounds().toString());
                             
        // Force repaint
        repaint();
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
        g.setFont(juce::Font(16.0f).boldened());
        g.drawText(title, 20, 10, getWidth() - 40, 24, juce::Justification::left, true);
        
        // Add control box outline to show where controls should be
        g.setColour(juce::Colour(0xFF9C33FF).withAlpha(0.3f));
        g.drawRect(20, 40, getWidth() - 40, getHeight() - 60, 1);
        
        // Debug - show panel size
        juce::Logger::writeToLog("CustomPanel painted: " + title + " " + getBounds().toString());
        
        // For debugging only - show component bounds (can be disabled for production)
        #ifdef DEBUG_LAYOUT
        int childCount = getNumChildComponents();
        for (int i = 0; i < childCount; ++i) {
            if (auto* child = getChildComponent(i)) {
                // Only draw if child has non-zero size
                if (!child->getBounds().isEmpty()) {
                    g.setColour(juce::Colours::red.withAlpha(0.2f));
                    g.fillRect(child->getBounds());
                    g.setColour(juce::Colours::red);
                    g.drawRect(child->getBounds(), 1);
                }
                
                // If a slider or button, draw a more visible highlight
                if (dynamic_cast<juce::Slider*>(child) || dynamic_cast<juce::Button*>(child) || 
                    dynamic_cast<juce::ComboBox*>(child)) {
                    g.setColour(juce::Colours::yellow.withAlpha(0.3f));
                    g.drawRect(child->getBounds(), 2);
                }
            }
        }
        #endif
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