#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class OpenGuitarAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    OpenGuitarAudioProcessorEditor(OpenGuitarAudioProcessor&);
    ~OpenGuitarAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OpenGuitarAudioProcessor& audioProcessor;

    // UI Components
    juce::Slider gainSlider;
    juce::Slider toneSlider;
    juce::Slider levelSlider;
    juce::ToggleButton bypassButton;

    juce::Label gainLabel;
    juce::Label toneLabel;
    juce::Label levelLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGuitarAudioProcessorEditor)
};
