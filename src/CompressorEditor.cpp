#include "CompressorProcessor.h"
#include "CompressorEditor.h"

CompressorAudioProcessorEditor::CompressorAudioProcessorEditor(CompressorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Threshold knob
    thresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(thresholdSlider);
    
    thresholdLabel.setText("THRESH", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(thresholdLabel);

    // Ratio knob
    ratioSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ratioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(ratioSlider);
    
    ratioLabel.setText("RATIO", juce::dontSendNotification);
    ratioLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(ratioLabel);

    // Attack knob
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(attackSlider);
    
    attackLabel.setText("ATTACK", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(attackLabel);

    // Release knob
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(releaseSlider);
    
    releaseLabel.setText("RELEASE", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(releaseLabel);

    // Makeup gain knob
    makeupSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    makeupSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(makeupSlider);
    
    makeupLabel.setText("MAKEUP", juce::dontSendNotification);
    makeupLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(makeupLabel);

    // Bypass button
    bypassButton.setButtonText("BYPASS");
    addAndMakeVisible(bypassButton);

    // Attach parameters
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "threshold", thresholdSlider);
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "ratio", ratioSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "attack", attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "release", releaseSlider);
    makeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "makeup", makeupSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "bypass", bypassButton);

    setSize(600, 300);
}

CompressorAudioProcessorEditor::~CompressorAudioProcessorEditor()
{
}

void CompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    juce::ColourGradient gradient(
        juce::Colour(0xff2a2a2a), 0, 0,
        juce::Colour(0xff1a1a1a), 0, (float)getHeight(),
        false);
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());

    g.setColour(juce::Colours::lightblue);
    g.setFont(juce::Font(juce::FontOptions(28.0f, juce::Font::bold)));
    g.drawText("COMPRESSOR", getLocalBounds().removeFromTop(50), 
               juce::Justification::centred, true);

    g.setColour(juce::Colours::lightblue.darker(0.5f));
    g.drawRoundedRectangle(20, 60, getWidth() - 40, getHeight() - 80, 10, 2);
}

void CompressorAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(70);
    bounds.removeFromBottom(30);
    bounds.reduce(40, 20);

    auto knobWidth = bounds.getWidth() / 5;

    auto threshArea = bounds.removeFromLeft(knobWidth);
    thresholdLabel.setBounds(threshArea.removeFromTop(30));
    thresholdSlider.setBounds(threshArea);

    auto ratioArea = bounds.removeFromLeft(knobWidth);
    ratioLabel.setBounds(ratioArea.removeFromTop(30));
    ratioSlider.setBounds(ratioArea);

    auto attackArea = bounds.removeFromLeft(knobWidth);
    attackLabel.setBounds(attackArea.removeFromTop(30));
    attackSlider.setBounds(attackArea);

    auto releaseArea = bounds.removeFromLeft(knobWidth);
    releaseLabel.setBounds(releaseArea.removeFromTop(30));
    releaseSlider.setBounds(releaseArea);

    auto makeupArea = bounds.removeFromLeft(knobWidth);
    makeupLabel.setBounds(makeupArea.removeFromTop(30));
    makeupSlider.setBounds(makeupArea);

    bypassButton.setBounds(getWidth() / 2 - 50, getHeight() - 50, 100, 30);
}
