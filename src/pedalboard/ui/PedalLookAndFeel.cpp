#include "PedalLookAndFeel.h"

PedalLookAndFeel::PedalLookAndFeel()
{
    // Set default colors
    setColour(juce::Slider::thumbColourId, getKnobColour());
    setColour(juce::Slider::rotarySliderFillColourId, getKnobPointerColour());
    setColour(juce::Slider::rotarySliderOutlineColourId, getMetalColour());
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::black);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::grey);
    
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4a4a4a));
    setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    setColour(juce::TextButton::textColourOnId, juce::Colours::white);
}

PedalLookAndFeel::~PedalLookAndFeel()
{
}

//==============================================================================
void PedalLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                        int x, int y, int width, int height,
                                        float sliderPosProportional,
                                        float rotaryStartAngle,
                                        float rotaryEndAngle,
                                        juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = juce::jmin(8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;
    
    // Draw knob body (dark with metallic rim)
    g.setColour(getMetalColour());
    g.fillEllipse(bounds);
    
    g.setColour(getKnobColour());
    g.fillEllipse(bounds.reduced(3));
    
    // Draw value arc
    juce::Path valueArc;
    valueArc.addCentredArc(bounds.getCentreX(),
                          bounds.getCentreY(),
                          arcRadius,
                          arcRadius,
                          0.0f,
                          rotaryStartAngle,
                          toAngle,
                          true);
    
    g.setColour(getKnobPointerColour());
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Draw pointer line
    juce::Path pointer;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = 3.0f;
    pointer.addRectangle(-pointerThickness * 0.5f, -radius + 10, pointerThickness, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));
    
    g.setColour(getKnobPointerColour());
    g.fillPath(pointer);
    
    // Draw center circle
    auto centerRadius = radius * 0.15f;
    g.setColour(getMetalColour());
    g.fillEllipse(bounds.getCentreX() - centerRadius,
                 bounds.getCentreY() - centerRadius,
                 centerRadius * 2.0f,
                 centerRadius * 2.0f);
}

juce::Label* PedalLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    auto* l = LookAndFeel_V4::createSliderTextBox(slider);
    l->setFont(juce::FontOptions(12.0f));
    l->setJustificationType(juce::Justification::centred);
    return l;
}

//==============================================================================
void PedalLookAndFeel::drawToggleButton(juce::Graphics& g,
                                        juce::ToggleButton& button,
                                        bool shouldDrawButtonAsHighlighted,
                                        bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto ledSize = 16.0f;
    auto ledBounds = bounds.removeFromLeft(ledSize + 10).withSizeKeepingCentre(ledSize, ledSize);
    
    // Draw LED indicator
    g.setColour(button.getToggleState() ? getLEDOffColour() : getLEDOnColour());
    g.fillEllipse(ledBounds);
    
    // Add glow effect when ON
    if (!button.getToggleState()) // Not bypassed = LED ON
    {
        g.setColour(getLEDOnColour().withAlpha(0.3f));
        g.fillEllipse(ledBounds.expanded(3));
    }
    
    // Draw LED rim
    g.setColour(juce::Colours::black);
    g.drawEllipse(ledBounds, 1.0f);
    
    // Draw text
    g.setColour(button.findColour(juce::ToggleButton::textColourId));
    g.setFont(juce::FontOptions(14.0f));
    
    auto textBounds = bounds;
    g.drawText(button.getButtonText(), textBounds, juce::Justification::centredLeft);
}

//==============================================================================
void PedalLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                            juce::Button& button,
                                            const juce::Colour& backgroundColour,
                                            bool shouldDrawButtonAsHighlighted,
                                            bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(2);
    auto cornerSize = 4.0f;
    
    // Base color
    auto baseColour = backgroundColour;
    if (shouldDrawButtonAsDown)
        baseColour = baseColour.darker(0.3f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter(0.1f);
    
    // Draw button with gradient
    juce::ColourGradient gradient(baseColour.brighter(0.2f), bounds.getX(), bounds.getY(),
                                 baseColour.darker(0.2f), bounds.getX(), bounds.getBottom(),
                                 false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, cornerSize);
    
    // Draw border
    g.setColour(baseColour.darker(0.5f));
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
    
    // Draw highlight on top
    if (!shouldDrawButtonAsDown)
    {
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.fillRoundedRectangle(bounds.removeFromTop(bounds.getHeight() * 0.4f), cornerSize);
    }
}
