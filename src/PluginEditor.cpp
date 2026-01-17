#include "PluginProcessor.h"
#include "PluginEditor.h"

OpenGuitarAudioProcessorEditor::OpenGuitarAudioProcessorEditor(OpenGuitarAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Gain knob (Fuzz)
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(gainSlider);
    
    gainLabel.setText("GAIN", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);

    // Tone knob (Fuzz)
    toneSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    toneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(toneSlider);
    
    toneLabel.setText("TONE", juce::dontSendNotification);
    toneLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(toneLabel);

    // Level knob (Fuzz)
    levelSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    levelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(levelSlider);
    
    levelLabel.setText("LEVEL", juce::dontSendNotification);
    levelLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(levelLabel);

    // Bypass button
    bypassButton.setButtonText("BYPASS");
    addAndMakeVisible(bypassButton);

    // Attach parameters
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "gain", gainSlider);
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "tone", toneSlider);
    levelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "level", levelSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "bypass", bypassButton);

    setSize(400, 300);
}

OpenGuitarAudioProcessorEditor::~OpenGuitarAudioProcessorEditor()
{
}

void OpenGuitarAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background gradient
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    juce::ColourGradient gradient(
        juce::Colour(0xff2a2a2a), 0, 0,
        juce::Colour(0xff1a1a1a), 0, (float)getHeight(),
        false);
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());

    // Title
    g.setColour(juce::Colours::orange);
    g.setFont(juce::Font(28.0f, juce::Font::bold));
    g.drawText("FUZZ PEDAL", getLocalBounds().removeFromTop(50), 
               juce::Justification::centred, true);

    // Pedal outline
    g.setColour(juce::Colours::orange.darker(0.5f));
    g.drawRoundedRectangle(20, 60, getWidth() - 40, getHeight() - 80, 10, 2);
}

void OpenGuitarAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(70);
    bounds.removeFromBottom(30);
    bounds.reduce(40, 20);

    auto knobWidth = bounds.getWidth() / 3;

    // Gain knob
    auto gainArea = bounds.removeFromLeft(knobWidth);
    gainLabel.setBounds(gainArea.removeFromTop(30));
    gainSlider.setBounds(gainArea);

    // Tone knob
    auto toneArea = bounds.removeFromLeft(knobWidth);
    toneLabel.setBounds(toneArea.removeFromTop(30));
    toneSlider.setBounds(toneArea);

    // Level knob
    auto levelArea = bounds.removeFromLeft(knobWidth);
    levelLabel.setBounds(levelArea.removeFromTop(30));
    levelSlider.setBounds(levelArea);

    // Bypass button
    bypassButton.setBounds(getWidth() / 2 - 50, getHeight() - 50, 100, 30);
}
