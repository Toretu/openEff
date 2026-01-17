#include "ReverbEditor.h"

ReverbEditor::ReverbEditor(ReverbProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(700, 300);

    // Room Size slider
    roomSizeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(roomSizeSlider);
    roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    roomSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(roomSizeLabel);
    roomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "roomSize", roomSizeSlider);

    // Damping slider
    dampingSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    dampingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(dampingSlider);
    dampingLabel.setText("Damping", juce::dontSendNotification);
    dampingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dampingLabel);
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "damping", dampingSlider);

    // Wet Level slider
    wetLevelSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    wetLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(wetLevelSlider);
    wetLevelLabel.setText("Mix", juce::dontSendNotification);
    wetLevelLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(wetLevelLabel);
    wetLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "wetLevel", wetLevelSlider);

    // Width slider
    widthSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(widthSlider);
    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(widthLabel);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "width", widthSlider);
}

ReverbEditor::~ReverbEditor()
{
}

void ReverbEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a3a4a));

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("OpenGuitar Reverb", getLocalBounds().removeFromTop(50), juce::Justification::centred);
}

void ReverbEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(60);
    area.reduce(40, 20);

    auto knobWidth = area.getWidth() / 4;

    // Room Size
    auto roomSizeArea = area.removeFromLeft(knobWidth);
    roomSizeLabel.setBounds(roomSizeArea.removeFromTop(30));
    roomSizeSlider.setBounds(roomSizeArea.reduced(10, 10));

    // Damping
    auto dampingArea = area.removeFromLeft(knobWidth);
    dampingLabel.setBounds(dampingArea.removeFromTop(30));
    dampingSlider.setBounds(dampingArea.reduced(10, 10));

    // Wet Level
    auto wetLevelArea = area.removeFromLeft(knobWidth);
    wetLevelLabel.setBounds(wetLevelArea.removeFromTop(30));
    wetLevelSlider.setBounds(wetLevelArea.reduced(10, 10));

    // Width
    auto widthArea = area.removeFromLeft(knobWidth);
    widthLabel.setBounds(widthArea.removeFromTop(30));
    widthSlider.setBounds(widthArea.reduced(10, 10));
}
