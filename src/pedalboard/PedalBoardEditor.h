#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PedalBoardProcessor.h"
#include "ui/PedalComponent.h"
#include "ui/PedalLookAndFeel.h"

/**
 * Enhanced editor for the OpenGuitar Pedal Board plugin.
 * Displays visual pedal components with drag-and-drop reordering.
 */
class PedalBoardEditor : public juce::AudioProcessorEditor,
                         public juce::Button::Listener,
                         public juce::ComboBox::Listener
{
public:
    explicit PedalBoardEditor(PedalBoardProcessor& p);
    ~PedalBoardEditor() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    // Button::Listener
    void buttonClicked(juce::Button* button) override;
    
    // ComboBox::Listener
    void comboBoxChanged(juce::ComboBox* comboBox) override;

private:
    PedalBoardProcessor& processor;
    PedalLookAndFeel pedalLookAndFeel;
    
    // UI Components
    juce::Viewport pedalViewport;
    juce::Component pedalContainer;
    
    juce::TextButton addEffectButton;
    juce::ComboBox effectSelector;
    juce::TextButton clearChainButton;
    juce::ToggleButton globalBypassButton;
    juce::TextButton removeLastButton;
    
    juce::Label titleLabel;
    
    // Pedal components
    juce::OwnedArray<PedalComponent> pedalComponents;
    
    // Global parameter controls
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::Label inputGainLabel;
    juce::Label outputGainLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> globalBypassAttachment;
    
    //==============================================================================
    void rebuildPedalComponents();
    void updatePedalLayout();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalBoardEditor)
};
