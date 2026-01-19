#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
 * Custom LookAndFeel for the OpenGuitar Pedal Board.
 * Provides a vintage, skeuomorphic design matching real guitar pedals.
 */
class PedalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PedalLookAndFeel();
    ~PedalLookAndFeel() override;
    
    //==============================================================================
    // Rotary Slider (Knobs)
    
    void drawRotarySlider(juce::Graphics& g,
                         int x, int y, int width, int height,
                         float sliderPosProportional,
                         float rotaryStartAngle,
                         float rotaryEndAngle,
                         juce::Slider& slider) override;
    
    juce::Label* createSliderTextBox(juce::Slider& slider) override;
    
    //==============================================================================
    // Toggle Buttons (Bypass switches)
    
    void drawToggleButton(juce::Graphics& g,
                         juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted,
                         bool shouldDrawButtonAsDown) override;
    
    //==============================================================================
    // Text Buttons
    
    void drawButtonBackground(juce::Graphics& g,
                             juce::Button& button,
                             const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;
    
    //==============================================================================
    // Custom colors
    
    static juce::Colour getPedalBackgroundColour() { return juce::Colour(0xff3a3a3a); }
    static juce::Colour getKnobColour() { return juce::Colour(0xff2a2a2a); }
    static juce::Colour getKnobPointerColour() { return juce::Colour(0xffff9900); }
    static juce::Colour getLEDOnColour() { return juce::Colour(0xffff0000); }
    static juce::Colour getLEDOffColour() { return juce::Colour(0xff3a0000); }
    static juce::Colour getMetalColour() { return juce::Colour(0xff8a8a8a); }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalLookAndFeel)
};
