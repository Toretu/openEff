#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../EffectFactory.h"
#include "../../effects/EffectBase.h"

/**
 * Visual component representing a single guitar effect pedal.
 * Displays parameters as rotary knobs and includes bypass control.
 */
class PedalComponent : public juce::Component
{
public:
    /**
     * Creates a pedal component for a specific effect.
     * @param effect The effect to visualize
     * @param apvts The parameter tree for parameter attachments
     * @param prefix The parameter prefix for this effect (e.g., "effect0_")
     * @param index The position of this effect in the chain
     */
    PedalComponent(EffectBase* effect,
                   juce::AudioProcessorValueTreeState& apvts,
                   const juce::String& prefix,
                   int index);
    
    ~PedalComponent() override;
    
    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    /**
     * Returns the effect index in the chain.
     */
    int getEffectIndex() const { return effectIndex; }
    
    /**
     * Returns the effect pointer.
     */
    EffectBase* getEffect() { return effect; }
    
    /**
     * Sets whether this pedal is being dragged.
     */
    void setBeingDragged(bool isDragged) { beingDragged = isDragged; repaint(); }

private:
    EffectBase* effect;
    juce::AudioProcessorValueTreeState& apvts;
    juce::String paramPrefix;
    int effectIndex;
    bool beingDragged = false;
    
    // UI Components
    juce::Label nameLabel;
    juce::ToggleButton bypassButton;
    
    // Dynamic parameter controls
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::Label> sliderLabels;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    
    //==============================================================================
    void createControlsForEffect();
    juce::Colour getPedalColour() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalComponent)
};
